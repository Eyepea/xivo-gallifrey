<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_voicemenu extends xivo_application_asterisk_abstract
{
	var $_voicemenu		= false;
	var $_contextmember	= false;
	var $_dialaction	= false;
	var $_extensions	= false;
	var $_extenumbers	= false;
	var $_voicemail		= null;
	var $_sound		= array();
	var $_musiconhold	= array();
	var $_contextlist	= array();
	var $_conf		= array();

	function xivo_application_service_asterisk_voicemenu(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_voicemenu = &$this->_service->get_module('voicemenu')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		$this->_load_config(dirname(__FILE__));

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_dialaction = &$this->_service->get_module('dialaction')) === false
		|| ($this->_extensions = &$this->_service->get_module('extensions')) === false
		|| ($this->_extenumbers = $this->get_abstract('extenumbers',array('type' => 'voicemenu'))) === false
		|| ($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'voicemenu'))) === false)
			return(false);

		$this->_voicemail = &$this->_service->get_application('voicemail');

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['voicemenu'] = $this->_voicemenu->get($id,$disable)) === false)
			return(false);

		$this->_origin['voicemenu'] = $this->_voicemenu->get_origin();

		if($this->_components === false)
			return($this->_info);

		$this->_get_voicemenumacro();
		$this->_get_extenumbers($this->_info['voicemenu']['id']);
		$this->_get_voicemenuflow();
		$this->_get_voicemenuevent();
		$this->_get_contextmember($this->_info['voicemenu']['id']);

		return($this->_info);
	}

	function _get_voicemenumacro()
	{
		$this->_info['voicemenumacro'] = false;
		$this->_origin['voicemenumacro'] = false;

		if(($this->_info['voicemenumacro'] = $this->_extensions->get_exten($this->_info['voicemenu']['number'],
										   $this->_info['voicemenu']['context'],
										   'macro')) !== false)
			$this->_origin['voicemenumacro'] = $this->_extensions->get_origin();

		return($this->_info['voicemenumacro']);
	}

	function _get_voicemenuflow()
	{
		$this->_info['voicemenuflow'] = false;
		$this->_origin['voicemenuflow'] = false;

		$context = XIVO_SRE_IPBX_AST_PRECONTEXT_VOICEMENU.$this->_info['voicemenu']['name'];

		if(($this->_info['voicemenuflow'] = $this->_extensions->get_context($context,
										    's',
										    null,
										    null,
										    array('priority' => SORT_ASC))) === false
		|| ($nb = count($this->_info['voicemenuflow'])) === 0)
				return(false);

		$this->_origin['voicemenuflow'] = $this->_extensions->get_origin_list();

		$this->_info['voicemenuflow-data'] = array();

		$refinfo = &$this->_info['voicemenuflow'];
		$refdata = &$this->_info['voicemenuflow-data'];

		for($i = 0;$i < $nb;$i++)
		{
			$refdata[$i] = $refinfo[$i]['app'];

			if(xivo_haslen($refinfo[$i]['appdata']) === true)
				$refdata[$i] .= '|'.$refinfo[$i]['appdata'];
		}

		return($this->_info['voicemenuflow']);
	}

	function _get_voicemenuevent()
	{
		$this->_info['voicemenuevent'] = array();
		$this->_info['voicemenuevent-data'] = array();
		$this->_origin['voicemenuevent'] = array();

		$context = XIVO_SRE_IPBX_AST_PRECONTEXT_VOICEMENU.$this->_info['voicemenu']['name'];

		$refinfo = &$this->_info['voicemenuevent'];
		$refdata = &$this->_info['voicemenuevent-data'];
		$reforig = &$this->_origin['voicemenuevent'];

		foreach($this->_conf['voicemenuevent']['value'] as $event)
		{
			if(($refinfo[$event] = $this->_extensions->get_exten($event,$context)) === false)
			{
				$reforig[$event] = false;
				continue;
			}

			$reforig[$event] = $this->_extensions->get_origin();
			$refdata[$event] = $refinfo[$event]['app'];

			if(xivo_haslen($refinfo[$event]['appdata']) === true)
				$refdata[$event] .= '|'.$refinfo[$event]['appdata'];
		}

		return($this->_info['voicemenuevent']);
	}

	function get_ipbxapplications()
	{
		if(isset($this->_conf) === false
		|| xivo_issa('ipbxapplication',$this->_conf) === false)
			return(false);

		return($this->_conf['ipbxapplication']);
	}

	function get_voicemail_list()
	{
		if(is_object($this->_voicemail) === false)
			return(false);

		$order = array();
		$order['fullname'] = SORT_ASC;
		$order['mailbox'] = SORT_ASC;
		$order['context'] = SORT_ASC;

		return($this->_voicemail->get_voicemail_list(null,$order));
	}

	function get_sound()
	{
		if(empty($this->_sound) === false)
			return($this->_sound);
		else if(($sounds = &$this->_service->get_module('sounds')) === false)
			return(false);
		else if(($this->_sound = $sounds->get_list('playback',true)) !== false)
			asort($this->_sound);

		return($this->_sound);
	}

	function get_elements()
	{
		$r = array();

		$r['dialaction'] = $this->_dialaction->get_element();
		$r['voicemenu'] = $this->_voicemenu->get_element();
		$r['voicemenuflow'] = array();
		$r['voicemenuflow']['actiontype'] = $this->_conf['actiontype']['voicemenuflow'];
		$r['voicemenuevent'] = array();
		$r['voicemenuevent']['event'] = $this->_conf['voicemenuevent'];
		$r['voicemenuevent']['actiontype'] = $this->_conf['actiontype']['voicemenuevent'];

		return($r);
	}

	function get_voicemenu_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		if(($rs = $this->_voicemenu->get_all($disable,true,$order,$limit,$assoc)) !== false
		&& empty($rs) === true)
			$rs = false;

		$this->_cnt = $this->_voicemenu->get_cnt();

		return($rs);
	}

	function get_destination_list()
	{
		$r = parent::get_destination_list();
		$r['sounds'] = $this->get_sound();

		return($r);
	}

	function is_valid_application($action,$actionarg1='',$actionarg2=null,$disable=null,$astapp=true)
	{
		switch($action)
		{
			case 'busy':
			case 'congestion':
			case 'hangup':
				return($this->_voicemenu->chk_dest_endcall($action,$actionarg1,$actionarg2));
			case 'user':
				return($this->_voicemenu->chk_dest_user($actionarg1,$actionarg2,null,$disable));
			case 'group':
				return($this->_voicemenu->chk_dest_group($actionarg1,$actionarg2,null,$disable));
			case 'queue':
				return($this->_voicemenu->chk_dest_queue($actionarg1,$actionarg2,null,$disable));
			case 'meetme':
				return($this->_voicemenu->chk_dest_meetme($actionarg1,null,$disable));
			case 'voicemail':
				return($this->_voicemenu->chk_dest_voicemail($actionarg1,$actionarg2,null,$disable));
			case 'schedule':
				return($this->_voicemenu->chk_dest_schedule($actionarg1,null,$disable));
			case 'voicemenu':
				return($this->_voicemenu->chk_dest_voicemenu($actionarg1,null,$disable));
			case 'extension':
				return($this->_voicemenu->chk_dest_extension($actionarg1,$actionarg2));
			case 'vmauthenticate':
				if((bool) $astapp === false)
					return(false);
			case 'callbackdisa':
			case 'disa':
			case 'directory':
			case 'faxtomail':
			case 'voicemailmain':
				return($this->_voicemenu->chk_dest_application($action,$actionarg1,$actionarg2));
			case 'sound':
				return($this->_voicemenu->chk_dest_sound('playback',$actionarg1,$actionarg2));
		}

		return(false);
	}

	function _real_application($val,$astapp=true)
	{
		if(xivo_haslen($val) === false)
			return(false);

		$expcomma = explode(',',$val,2);

		if(isset($expcomma[1]) === false)
			$actionarg1 = $actionarg2 = '';
		else
		{
			$exppipe = explode('|',$expcomma[1],2);
			$actionarg1 = $exppipe[0];

			if(isset($exppipe[1]) === true)
				$actionarg2 = $exppipe[1];
			else
				$actionarg2 = '';
		}

		$expcomma[0] = strtolower($expcomma[0]);

		if(isset($this->_conf['defapplication'][$expcomma[0]]) === false)
		{
			if(isset($this->_conf['ipbxapplication'][$expcomma[0]]) === false)
				return(false);
			else if($expcomma[0] === 'background' || $expcomma[0] === 'playback')
			{
				if(($rs = $this->_voicemenu->chk_dest_sound('playback',
									    $actionarg1,
									    null,
									    'filename')) === false
				|| ($sounds = &$this->_service->get_module('sounds')) === false
				|| ($dirpath = $sounds->get_dir_path('playback')) === false)
					return(false);

				$expcomma[1] = $dirpath.XIVO_SEP_DIR.$rs['actionarg1'];

				if($actionarg2 !== '')
					$expcomma[2] = $actionarg2;
			}

			return(((bool) $astapp === true ? implode('|',$expcomma) : false));
		}
		else if(($rs = $this->is_valid_application($this->_conf['defapplication'][$expcomma[0]],
							   $actionarg1,
							   $actionarg2,
							   null,
							   $astapp)) !== false)
		{
			$rs['action'] = $expcomma[0];
			return(implode('|',$rs));
		}

		return(false);
	}

	function _mk_voicemenumacro_appdata($voicemenuid,$priority=1)
	{
		$arr = array();
		$arr['appdata'][0] = XIVO_SRE_IPBX_AST_VOICEMENU_MACRO;
		$arr['appdata'][1] = xivo_ulongint($voicemenuid);
		$arr['appdata'][2] = xivo_uint($priority,1);

		return($arr);
	}

	function set_add($arr)
	{
		return($this->_set('add',$arr));
	}

	function set_edit($arr)
	{
		if(empty($this->_info) === true)
			return(false);

		return($this->_set('edit',$arr));
	}

	function _set($action,$arr)
	{
		$this->_reset();

		if(($action !== 'add' && $action !== 'edit') === true
		|| xivo_issa('voicemenu',$arr) === false)
			return(false);

		$this->set_voicemenu($arr['voicemenu']);

		if(isset($arr['voicemenuflow']) === true)
			$this->set_voicemenuflow($arr['voicemenuflow']);
		else
		{
			$this->_set_result('voicemenuflow-data',null);
			$this->_set_error('voicemenuflow','invalid data');
		}

		if(xivo_issa('voicemenuevent',$arr) === true)
			$this->set_voicemenuevent($arr['voicemenuevent']);

		$this->set_contextmember();

		$number = (string) $this->get_result_var('voicemenu','number');
		$context = (string) $this->get_result_var('voicemenu','context');

		if($this->_errnb === 0 && xivo_haslen($number) === true)
		{
			if(($rsvoicemenumacro = $this->chk_macro('voicemenumacro',
								 $number,
								 $this->_mk_voicemenumacro_appdata(0),
								 $context)) !== false)
				$this->_set_result('voicemenumacro',$rsvoicemenumacro);

			$this->set_extenumbers($action,$number,$context);
		}

		return(($this->get_errnb() < 1));
	}

	function set_voicemenu($arr,$setval=true)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('voicemenu',null);
			$this->_set_error('voicemenu','invalid data');
			return(false);
		}

		$arr['commented'] = false;

		if(($rs = $this->_voicemenu->chk_values($arr,true,$setval)) === false)
		{
			$this->_set_result('voicemenu',$this->_voicemenu->get_filter_result());
			$this->_set_error('voicemenu',$this->_voicemenu->get_filter_error());
			return(false);
		}

		$this->_set_result('voicemenu',$rs);

		return(true);
	}

	function _set_voicemenumacro($action,$voicemenuid)
	{
		$rs = false;

		if(($voicemenuid = xivo_ulongint($voicemenuid)) === 0
		|| ($rs = $this->get_result('voicemenumacro')) === null
		|| $rs === false)
			return($rs);
		else if(($rsmacro = $this->set_macro($action,
						     'voicemenumacro',
						     $rs['exten'],
						     $this->_mk_voicemenumacro_appdata($voicemenuid),
						     $rs['context'])) === false)
			return(false);

		$this->_set_result('voicemenumacro',$rsmacro);

		return(true);
	}

	function set_voicemenuflow($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('voicemenuflow-data',null);
			$this->_set_error('voicemenuflow','invalid data');
			return(false);
		}

		$arr = array_values($arr);

		if(($nb = count($arr)) === 0)
		{
			$this->_set_result('voicemenuflow-data',null);
			$this->_set_error('voicemenuflow','invalid data');
			return(false);
		}

		$res = $err = $data = array();

		$data['name'] = '';
		$context = XIVO_SRE_IPBX_AST_PRECONTEXT_VOICEMENU.
			   (string) $this->get_result_var('voicemenu','name');

		$eid = false;

		for($i = $j = 0;$i < $nb;$i++)
		{
			if(($rs = $this->_real_application($arr[$i])) === false)
			{
				$res[$i] = array($arr[$i] => 'ERR');
				$err[$j++] = $i;
				continue;
			}

			$error = true;

			$exp = explode('|',$rs,2);

			if(isset($exp[1]) === true)
				$data['appdata'] = explode('|',$exp[1]);
			else
				unset($data['appdata']);

			if($i === 0)
			{
				if(($eid = $this->_extensions->new_context($context,'s',$exp[0],$data)) !== false)
					$error = false;
			}
			else if($eid !== false)
			{
				if($this->_extensions->set_context('s',$exp[0],$eid,$data) !== false)
					$error = false;
			}
			else if($this->_extensions->chk_context($context,'s',$exp[0],$data) !== false)
				$error = false;

			if($error === false)
				$res[$i] = $arr[$i];
			else
			{
				$res[$i] = array($arr[$i] => 'ERR');
				$err[$j++] = $i;
			}
		}

		if(empty($err) === false)
		{
			$this->_set_result('voicemenuflow-data',$res);
			$this->_set_error('voicemenuflow',$err);
			return(false);
		}
		else if($eid === false)
		{
			$this->_set_result('voicemenuflow-data',null);
			$this->_set_error('voicemenuflow','invalid data');
			return(false);
		}

		$this->_set_result('voicemenuflow-data',$arr);
		$this->_set_result('voicemenuflow',$eid);

		return(true);
	}

	function set_voicemenuevent($arr)
	{
		if(is_array($arr) === false)
			return(null);

		$context = XIVO_SRE_IPBX_AST_PRECONTEXT_VOICEMENU.
			   (string) $this->get_result_var('voicemenu','name');

		$i = 0;
		$eid = null;
		$res = $err = $data = array();

		$data['name'] = '';
		$data['priority'] = 1;

		foreach($arr as $event => $eventdata)
		{
			if(xivo_haslen($eventdata) === false)
				continue;
			else if(($rs = $this->_real_application($eventdata,false)) === false)
			{
				$res[$event] = $eventdata;
				$err[$event] = 1;
				$i++;
				continue;
			}

			$error = true;

			$exp = explode('|',$rs,2);

			if(isset($exp[1]) === true)
				$data['appdata'] = explode('|',$exp[1]);
			else
				unset($data['appdata']);

			if($i === 0)
			{
				if(($eid = $this->_extensions->new_context($context,$event,$exp[0],$data)) !== false)
					$error = false;
			}
			else if($eid !== false && $eid !== null)
			{
				if($this->_extensions->set_context($event,$exp[0],$eid,$data) !== false)
					$error = false;
			}
			else if($this->_extensions->chk_context($context,$event,$exp[0],$data) !== false)
				$error = false;

			if($error === false)
				$res[$event] = $eventdata;
			else
			{
				$res[$event] = $eventdata;
				$err[$event] = 1;
			}
			$i++;
		}

		if(empty($err) === false)
		{
			$this->_set_result('voicemenuevent-data',$res);
			$this->_set_error('voicemenuevent',$err);
			return(false);
		}
		else if($eid === false)
		{
			$this->_set_result('voicemenuevent-data',null);
			$this->_set_error('voicemenuevent','invalid data');
			return(false);
		}

		$this->_set_result('voicemenuevent-data',$arr);
		$this->_set_result('voicemenuevent',$eid);

		return(true);
	}

	function set_contextmember()
	{
		$rs = array();
		$rs['voicemenu-context']['context'] = (string) $this->get_result_var('voicemenu','context');
		$rs['voicemenu-context']['typeval'] = 0;
		$rs['voicemenu-context']['varname'] = 'context';

		return(parent::set_contextmember($rs));
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$voicemenuid = null;

		if($this->get_errnb() > 0
		|| ($voicemenuid = $this->add_voicemenu()) === false
		|| $this->add_extenumbers($voicemenuid) === false
		|| $this->add_voicemenumacro($voicemenuid) === false
		|| $this->add_voicemenuflow() === false
		|| $this->add_voicemenuevent() === false
		|| $this->add_contextmember($voicemenuid) === false)
		{
			$r = false;

			if($voicemenuid !== null)
				$this->_reverse_add($voicemenuid);
		}

		$this->_status = '';

		return($r);
	}

	function add_voicemenu()
	{
		if(($rs = $this->get_result('voicemenu')) === null
		|| $rs === false)
			return(false);

		return(($this->_return['voicemenu'] = $this->_voicemenu->add($rs)));
	}

	function add_voicemenumacro($voicemenuid)
	{
		if(($rs = $this->_set_voicemenumacro('add',$voicemenuid)) !== true)
			return($rs);

		return(parent::add_macro('voicemenumacro'));
	}

	function add_voicemenuflow()
	{
		if(($rs = $this->get_result('voicemenuflow')) === null
		|| $rs === false)
			return(false);

		return(($this->_return['voicemenuflow'] = $this->_extensions->add_context($rs)));
	}

	function add_voicemenuevent()
	{
		if(($rs = $this->get_result('voicemenuevent')) === null
		|| $rs === false)
			return($rs);

		return(($this->_return['voicemenuevent'] = $this->_extensions->add_context($rs)));
	}

	function _reverse_add($voicemenuid)
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'add'
		|| ($voicemenuid = xivo_ulongint($voicemenuid)) === 0
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'voicemenu':
					$this->_voicemenu->delete($voicemenuid);
					break;
				case 'voicemenuflow':
					$this->_extensions->delete_context($val);
					break;
				case 'voicemenuevent':
					$this->_extensions->delete_context($val);
					break;
				case 'voicemenumacro':
					$this->_extensions->delete_exten($val);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_add();
					break;
				case 'contextmember':
					$this->_contextmember->reverse_add();
					break;
			}
		}

		return(true);
	}

	function edit()
	{
		$r = true;

		$this->_status = 'edit';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_voicemenu() === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}
		else if($this->_info['voicemenu']['number'] !== '')
		{
			if($this->get_result('voicemenumacro') === null
			|| $this->get_result('extenumbers') === null)
			{
				$this->delete_macro('voicemenumacro');
				$this->delete_extenumbers();
			}
			else if($this->edit_voicemenumacro($this->_info['voicemenu']['id']) === false
			|| $this->edit_extenumbers() === false)
			{
				$this->_reverse_edit();
				$this->_status = '';
				return(false);
			}
		}
		else if($this->add_voicemenumacro($this->_info['voicemenu']['id']) === false
		|| $this->add_extenumbers($this->_info['voicemenu']['id']) === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		if($this->edit_voicemenuflow() === false
		|| $this->edit_voicemenuevent() === false
		|| $this->edit_contextmember() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_voicemenu()
	{
		if(xivo_issa('voicemenu',$this->_info) === false
		|| ($rs = $this->get_result('voicemenu')) === null
		|| $rs === false)
			return(false);

		$rs['commented'] = $this->_info['voicemenu']['commented'];

		return(($this->_return['voicemenu'] = $this->_voicemenu->edit($this->_info['voicemenu']['id'],$rs)));
	}

	function edit_voicemenumacro($voicemenuid)
	{
		if(($rs = $this->_set_voicemenumacro('edit',$voicemenuid)) !== true)
			return($rs);

		return(parent::edit_macro('voicemenumacro'));
	}

	function edit_voicemenuflow()
	{
		if(($rs = $this->get_result('voicemenuflow')) === null
		|| $rs === false
		|| $this->delete_voicemenuflow() === false)
			return(false);
		else if(($this->_return['voicemenuflow'] = $this->_extensions->add_context($rs)) === false)
			$this->_return['voicemenuflow'] = 'deleted';

		return($this->_return['voicemenuflow']);
	}

	function edit_voicemenuevent()
	{
		$del = false;

		if(($rs = $this->get_result('voicemenuevent')) === false
		|| ($del = $this->delete_voicemenuevent()) === false
		|| $del === 'error')
		{
			$this->_return['voicemenuevent'] = $del;
			return(false);
		}
		else if($rs === null)
			return(null);
		else if(($this->_return['voicemenuevent'] = $this->_extensions->add_context($rs)) === false)
			$this->_return['voicemenuevent'] = 'deleted';

		return($this->_return['voicemenuevent']);
	}

	function edit_contextmember()
	{
		if(xivo_issa('voicemenu',$this->_info) === false)
			return(false);

		return(parent::edit_contextmember($this->_info['voicemenu']['id']));
	}

	function _reverse_edit()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'edit'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'voicemenu':
					if(isset($this->_origin['voicemenu']) === false
					|| $this->_origin['voicemenu'] === false)
						$this->delete_voicemenu();
					else
						$this->_voicemenu->edit_origin($this->_origin['voicemenu']);
					break;
				case 'voicemenuflow':
					$this->delete_voicemenuflow();

					if(isset($this->_origin['voicemenuflow']) === true
					&& $this->_origin['voicemenuflow'] !== false)
						$this->_extensions->add_origin_list($this->_origin['voicemenuflow']);
					break;
				case 'voicemenuevent':
					$this->delete_voicemenuevent();

					if(isset($this->_origin['voicemenuevent']) === true
					&& $this->_origin['voicemenuevent'] !== false)
						$this->_extensions->add_origin_list($this->_origin['voicemenuevent']);
					break;
				case 'voicemenumacro':
					if(isset($this->_origin['voicemenumacro']) === false
					|| $this->_origin['voicemenumacro'] === false)
						$this->delete_macro('voicemenumacro');
					else
						$this->_extensions->edit_origin($this->_origin['voicemenumacro']);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_edit($this->_status);
					break;
				case 'contextmember':
					$this->_contextmember->reverse_edit($this->_status);
					break;
			}
		}

		return(true);
	}

	function delete()
	{
		$r = true;

		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_voicemenu() === false
		|| $this->delete_macro('voicemenumacro') === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_voicemenuflow() === false
		|| ($delvoicemenuevent = $this->delete_voicemenuevent()) === false
		|| $delvoicemenuevent === 'error'
		|| $this->delete_contextmember() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}
		else
			$this->unlinks();

		$this->_status = '';

		return($r);
	}

	function delete_voicemenu()
	{
		if(xivo_issa('voicemenu',$this->_info) === false)
			return(false);

		$r = $this->_voicemenu->delete($this->_info['voicemenu']['id']);

		if($this->_status === 'delete')
			$this->_return['voicemenu'] = $r;

		return($r);
	}

	function delete_voicemenuflow()
	{
		if(xivo_issa('voicemenu',$this->_info) === false
		|| xivo_issa('voicemenuflow',$this->_info) === false)
			return(false);

		$where = array();
		$where['context'] = XIVO_SRE_IPBX_AST_PRECONTEXT_VOICEMENU.
				    $this->_info['voicemenu']['name'];
		$where['exten']	= 's';

		$r = $this->_extensions->delete_where($where);

		if($this->_status === 'delete')
			$this->_return['voicemenuflow'] = $r;

		return($r);
	}

	function delete_voicemenuevent()
	{
		if(xivo_issa('voicemenu',$this->_info) === false)
			return(false);
		else if(xivo_issa('voicemenuevent',$this->_info) === false)
			return(null);

		$where = array();
		$where['context'] = XIVO_SRE_IPBX_AST_PRECONTEXT_VOICEMENU.
				    $this->_info['voicemenu']['name'];

		$r = true;

		reset($this->_info['voicemenuevent']);

		while(list($exten) = each($this->_info['voicemenuevent']))
		{
			$where['exten']	= $exten;

			if($this->_extensions->delete_where($where) === false)
				$r = 'error';
		}

		if($this->_status === 'delete')
			$this->_return['voicemenuflow'] = $r;

		return($r);
	}

	function _reverse_delete()
	{
		if($this->_status !== 'delete'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'voicemenu':
					if(isset($this->_origin['voicemenu']) === true
					&& $this->_origin['voicemenu'] !== false)
						$this->_voicemenu->add_origin($this->_origin['voicemenu']);
					break;
				case 'voicemenuflow':
					if(isset($this->_origin['voicemenuflow']) === true
					&& $this->_origin['voicemenuflow'] !== false)
						$this->_extensions->add_origin_list($this->_origin['voicemenuflow']);
					break;
				case 'voicemenuevent':
					if(isset($this->_origin['voicemenuevent']) === true
					&& $this->_origin['voicemenuevent'] !== false)
						$this->_extensions->add_origin_list($this->_origin['voicemenuevent']);
					break;
				case 'voicemenumacro':
					if(isset($this->_origin['voicemenumacro']) === true
					&& $this->_origin['voicemenumacro'] !== false)
						$this->_extensions->add_origin($this->_origin['voicemenumacro']);
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === true
					&& $this->_origin['extenumbers'] !== false)
						$this->_extenumbers->add_origin($this->_origin['extenumbers']);
					break;
				case 'contextmember':
					$this->_contextmember->add_origin_list();
					break;
			}
		}

		return(true);
	}

	function unlinks()
	{
		if(xivo_issa('voicemenu',$this->_info) === false)
			return(false);

		$this->_dialaction->unlinked('voicemenu',$this->_info['voicemenu']['id']);

		return(true);
	}

	function enable()
	{
		$r = true;
		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_voicemenu() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function disable()
	{
		$r = true;
		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_voicemenu() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function enable_voicemenu()
	{
		return($this->_enable_disable_voicemenu(false));
	}

	function disable_voicemenu()
	{
		return($this->_enable_disable_voicemenu(true));
	}

	function _enable_disable_voicemenu($disable=false)
	{
		if(xivo_issa('voicemenu',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_voicemenu->enable($this->_info['voicemenu']['id']);
		else
			$r = $this->_voicemenu->disable($this->_info['voicemenu']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['voicemenu'] = $r;

		return($r);
	}
}

?>
