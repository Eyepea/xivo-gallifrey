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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_group extends xivo_application_asterisk_abstract
{
	var $_dso		= null;
	var $_groupfeatures	= false;
	var $_queue		= false;
	var $_extensions	= false;
	var $_extenumbers	= false;
	var $_rightcall		= false;
	var $_dialaction	= false;
	var $_contextnummember	= false;
	var $_callerid		= false;
	var $_queuemember	= null;
	var $_musiconhold	= array();
	var $_contextlist	= array();

	function xivo_application_service_asterisk_group(&$service,&$dso,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if(($this->_groupfeatures = &$this->_service->get_module('groupfeatures')) === false
		|| ($this->_queue = &$this->_service->get_module('queue')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_extensions = &$this->_service->get_module('extensions')) === false
		|| ($this->_extenumbers = $this->get_abstract('extenumbers',array('type' => 'group'))) === false
		|| ($this->_rightcall = $this->get_abstract('rightcall',array('type' => 'group'))) === false
		|| ($this->_dialaction = $this->get_abstract('dialaction',array('category' => 'group'))) === false
		|| ($this->_contextnummember = $this->get_abstract('contextnummember',array('type' => 'group'))) === false
		|| ($this->_callerid = $this->get_abstract('callerid',array('type' => 'group'))) === false)
			return(false);

		$this->_queuemember = &$this->_service->get_module('queuemember');

		return(true);
	}

	function get($id,$disable=null,$nocomponents=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['groupfeatures'] = $this->_groupfeatures->get($id)) === false
		|| ($this->_info['queue'] = $this->_queue->get($this->_info['groupfeatures']['name'],$disable)) === false)
			return(false);

		$this->_origin['groupfeatures'] = $this->_groupfeatures->get_origin();
		$this->_origin['queue'] = $this->_queue->get_origin();

		if($this->_components === false)
			return($this->_info);

		if(dwho_ak('groupmacro',$nocomponents,true) === false)
			$this->_get_groupmacro();

		if(dwho_ak('extenumbers',$nocomponents,true) === false)
			$this->_get_extenumbers($this->_info['groupfeatures']['id']);

		if(dwho_ak('user',$nocomponents,true) === false)
			$this->_get_user();

		if(dwho_ak('rightcall',$nocomponents,true) === false)
			$this->_get_rightcall($this->_info['groupfeatures']['id']);

		if(dwho_ak('dialaction',$nocomponents,true) === false)
			$this->_get_dialaction($this->_info['groupfeatures']['id']);

		if(dwho_ak('contextnummember',$nocomponents,true) === false)
			$this->_get_contextnummember($this->_info['groupfeatures']['id']);

		if(dwho_ak('callerid',$nocomponents,true) === false)
			$this->_get_callerid($this->_info['groupfeatures']['id']);

		return($this->_info);
	}

	function get_nb($arr=null,$disable=null)
	{
		if(is_array($arr) === false)
			$arr = array();

		$arr['category'] = 'group';

		return($this->_queue->get_nb($arr,$disable));
	}

	function _get_groupmacro()
	{
		$this->_info['groupmacro'] = false;
		$this->_origin['groupmacro'] = false;

		if(($this->_info['groupmacro'] = $this->_extensions->get_exten($this->_info['groupfeatures']['number'],
									       $this->_info['groupfeatures']['context'],
									       'macro')) !== false)
			$this->_origin['groupmacro'] = $this->_extensions->get_origin();

		return($this->_info['groupmacro']);
	}

	function _get_user()
	{
		$this->_info['user'] = false;
		$this->_origin['user'] = false;

		$where = array();
		$where['usertype'] = 'user';
		$where['category'] = 'group';
		$where['queue_name'] = $this->_info['groupfeatures']['name'];

		if($this->_queuemember === false
		|| ($this->_info['user'] = $this->_queuemember->get_all_where($where)) === false
		|| isset($this->_info['user'][0]) === false)
			return(false);

		$this->_origin['user'] = $this->_queuemember->get_origin_list();

		return($this->_info['user']);
	}

	function get_elements()
	{
		$r = array();

		$r['groupfeatures'] = $this->_groupfeatures->get_element();
		$r['queue'] = $this->_queue->get_element();
		$r['dialaction'] = $this->_dialaction->get_element();
		$r['callerid'] = $this->_callerid->get_element();

		return($r);
	}

	function get_groups_search($search,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_groups('search',$search,$disable,$order,$limit,$assoc));
	}

	function get_groups_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_groups('list',null,$disable,$order,$limit,$assoc));
	}

	function _get_groups($action,$search='',$disable=null,$order=null,$limit=null,$assoc=false)
	{
		$search = strval($search);

		switch($action)
		{
			case 'search':
				$rs = $this->_dso->search($search,$disable,$order,$limit);
				break;
			default:
				$rs = $this->_dso->get_all($disable,$order,$limit);
		}

		$this->_cnt = $this->_dso->get_cnt();

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $identity = array();

		for($i = 0;$i < $nb;$i++)
		{
			$r[$rs[$i]['id']] = $rs[$i];

			$ref = &$r[$rs[$i]['id']];

			$identity['name'] = $ref['name'];
			$identity['number'] = $ref['number'];
			$identity['context'] = $ref['context'];

			$ref['identity'] = $this->_groupfeatures->mk_identity($identity);
			$ref['deleted'] = (bool) $ref['deleted'];
			$ref['commented'] = (bool) $ref['commented'];
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function get_dialaction_result()
	{
		return($this->_dialaction->get_result_for_display());
	}

	function get_dialaction_destination_list()
	{
		if(dwho_issa('groupfeatures',$this->_info) === false)
			$id = 0;
		else
			$id = $this->_info['groupfeatures']['id'];

		$r = $this->get_destination_list('group',$id);
		$r['sounds'] = $this->_dialaction->get_sound();

		return($r);
	}

	function _mk_groupmacro_appdata($groupfeaturesid,$timeout='')
	{
		$arr = array();
		$arr['appdata'][0] = XIVO_SRE_IPBX_AST_GROUP_MACRO;
		$arr['appdata'][1] = dwho_ulongint($groupfeaturesid);
		$arr['appdata'][2] = dwho_has_len($timeout) === true ? dwho_ufloat($timeout) : '';

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
		|| dwho_issa('groupfeatures',$arr) === false
		|| dwho_issa('queue',$arr) === false)
			return(false);

		$this->set_groupfeatures($arr['groupfeatures']);

		$name = strtolower($this->get_result_var('groupfeatures','name'));
		$number = (string) $this->get_result_var('groupfeatures','number');
		$context = (string) $this->get_result_var('groupfeatures','context');

		$arr['queue']['name'] = $name;

		$this->set_queue($arr['queue']);

		if($this->_errnb === 0 && $number !== '')
		{
			if(($rsgroupmacro = $this->chk_macro('groupmacro',
							     $number,
							     $this->_mk_groupmacro_appdata(0),
							     $context)) !== false)
				$this->_set_result('groupmacro',$rsgroupmacro);

			$this->set_extenumbers($action,$number,$context);
		}

		if(dwho_issa('user',$arr) === true)
			$this->set_user($action,$arr['user'],$name);

		if(dwho_issa('rightcall',$arr) === true)
			$this->set_rightcall($arr['rightcall']);

		if(dwho_issa('dialaction',$arr) === true)
			$this->set_dialaction($arr['dialaction']);

		$this->set_contextnummember($context,$number);

		if(dwho_issa('callerid',$arr) === true)
			$this->set_callerid($arr['callerid']);

		return(($this->get_errnb() < 1));
	}

	function set_groupfeatures($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('groupfeatures',null);
			$this->_set_error('groupfeatures','invalid data');
			return(false);
		}
		else if(($rs = $this->_groupfeatures->chk_values($arr)) === false)
		{
			$this->_set_result('groupfeatures',$this->_groupfeatures->get_filter_result());
			$this->_set_error('groupfeatures',$this->_groupfeatures->get_filter_error());
			return(false);
		}

		$this->_set_result('groupfeatures',$rs);

		return(true);
	}

	function set_queue($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('queue',null);
			$this->_set_error('queue','invalid data');
			return(false);
		}

		$this->get_musiconhold();

		if(isset($arr['musiconhold'],$this->_musiconhold[$arr['musiconhold']]) === false)
			$arr['musiconhold'] = '';

		$arr['category'] = 'group';

		if(($rs = $this->_queue->chk_values($arr)) === false)
		{
			$this->_set_result('queue',$this->_queue->get_filter_result());
			$this->_set_error('queue',$this->_queue->get_filter_error());
			return(false);
		}

		$this->_set_result('queue',$rs);

		return(true);
	}

	function _set_groupmacro($action,$groupfeaturesid)
	{
		$rs = false;

		if(($groupfeaturesid = dwho_ulongint($groupfeaturesid)) === 0
		|| ($rs = $this->get_result('groupmacro')) === null
		|| $rs === false)
			return($rs);
		else if(($rsmacro = $this->set_macro($action,
						     'groupmacro',
						     $rs['exten'],
						     $this->_mk_groupmacro_appdata($groupfeaturesid),
						     $rs['context'])) === false)
			return(false);

		$this->_set_result('groupmacro',$rsmacro);

		return(true);
	}

	function set_user($action,$list,$name)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| is_array($list) === false
		|| is_scalar($name) === false
		|| ($userfeatures = &$this->_service->get_module('userfeatures')) === false
		|| $this->_queuemember === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === 0)
			return(null);

		$where = array();
		$where['usertype'] = 'user';
		$where['userid'] = 0;
		$where['category'] = 'group';

		if($action === 'edit')
		{
			if(dwho_issa('groupfeatures',$this->_info) === false)
				return(false);

			$where['queue_name'] = $this->_info['groupfeatures']['name'];
		}

		$tmp = $reslist = $callgroup = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($uinfo = $userfeatures->get($list[$i])) === false)
			{
				$this->_set_error('user','wrong user');
				continue;
			}

			$where['userid'] = $uinfo['id'];

			$info = $where;
			$info['penalty'] = $info['call-limit'] = 0;
			$info['paused'] = null;

			if($action === 'edit'
			&& $this->_queuemember->get_where($where) !== false
			&& ($orig = $this->_queuemember->get_origin()) !== false)
				$info = array_merge($info,$orig);
			else
			{
				$interface = $this->mk_interface($uinfo['name'],
								 null,
								 $uinfo['context'],
								 $uinfo['number'],
								 $uinfo['protocol']);

				if($interface === false)
					continue;

				$info['interface'] = $interface['interface'];
				$info['channel'] = $interface['channel'];
			}

			$info['queue_name'] = $name;

			if(($rs = $this->_queuemember->chk_values($info)) === false
			|| isset($tmp[$uinfo['id']]) === true)
				continue;
			else if($uinfo['protocol'] !== XIVO_SRE_IPBX_AST_PROTO_SIP)
				$callgroup[$uinfo['id']] = false;
			else
			{
				$callgroup[$uinfo['id']] = array('protocol' => $uinfo['protocol'],
								 'protocolid' => $uinfo['protocolid']);
			}

			$tmp[$uinfo['id']] = 1;
			$reslist[] = $rs;
		}

		if(isset($reslist[0]) === false)
		{
			$this->_set_result('user',false);
			return(false);
		}

		if(empty($callgroup) === false)
			$this->_set_result('user_callgroup',$callgroup);
		else
			$this->_set_result('user_callgroup',null);

		$this->_set_result('user',$reslist);

		return(true);
	}

	function add()
	{
		$this->_status = 'add';
		$this->_return = array();
		$groupfeaturesid = null;

		if($this->get_errnb() > 0
		|| ($groupfeaturesid = $this->add_groupfeatures()) === false
		|| $this->add_queue() === false
		|| $this->add_groupmacro($groupfeaturesid) === false
		|| $this->add_extenumbers($groupfeaturesid) === false
		|| $this->add_user() === false
		|| $this->add_rightcall($groupfeaturesid) === false
		|| $this->add_dialaction($groupfeaturesid) === false
		|| $this->add_contextnummember($groupfeaturesid) === false
		|| $this->add_callerid($groupfeaturesid) === false)
		{
			if($groupfeaturesid !== null)
				$this->_reverse_add();

			$this->_status = '';
			return(false);
		}

		$this->add_user_callgroup($groupfeaturesid);

		$this->_status = '';

		return(true);
	}

	function add_groupfeatures()
	{
		if(($rs = $this->get_result('groupfeatures')) === null
		|| $rs === false)
			return(false);

		$this->_return['groupfeatures'] = $this->_groupfeatures->add($rs);

		return($this->_return['groupfeatures']);
	}

	function add_queue()
	{
		if(($rs = $this->get_result('queue')) === null
		|| $rs === false
		|| $this->_queue->add($rs) === false)
			return(false);

		$this->_return['queue'] = $rs['name'];

		return($this->_return['queue']);
	}

	function add_groupmacro($groupfeaturesid)
	{
		if(($rs = $this->_set_groupmacro('add',$groupfeaturesid)) !== true)
			return($rs);

		return(parent::add_macro('groupmacro'));
	}

	function add_user()
	{
		if(($rs = $this->get_result('user')) === null || $rs === false)
			return($rs);

		$ret = $this->_return['user'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_queuemember->add($rs[$i]) === false)
				return(false);

			$ret['queue_name'] = $rs[$i]['queue_name'];
			$ret['interface'] = $rs[$i]['interface'];

			$this->_return['user'][] = $ret;
		}

		if(isset($this->_return['user'][0]) === false)
			$this->_return['user'] = false;

		return(true);
	}

	function add_user_callgroup($groupfeaturesid)
	{
		$rs = false;

		if(($groupfeaturesid = dwho_ulongint($groupfeaturesid)) === 0
		|| ($rs = $this->get_result('user_callgroup')) === null)
			return($rs);
		else if(is_array($rs) === false || empty($rs) === true)
			return(false);

		$callgroup = array('callgroup' => '');

		foreach($rs as $val)
		{
			if($val === false
			|| ($protocol = &$this->_service->get_protocol_module($val['protocol'])) === false
			|| ($info = $protocol->get($val['protocolid'])) === false
			|| array_key_exists('callgroup',$info) === false)
				continue;
			else if(is_array($info['callgroup']) === false)
				$callgroup['callgroup'] = $groupfeaturesid;
			else if(in_array($groupfeaturesid,$info['callgroup']) === true)
				continue;
			else
			{
				$callgroup['callgroup'] = $info['callgroup'];
				$callgroup['callgroup'][] = $groupfeaturesid;
				$callgroup['callgroup'] = implode(',',$callgroup['callgroup']);
			}

			$protocol->edit($info['id'],$callgroup);
		}

		return(true);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'add'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'groupfeatures':
					$this->_groupfeatures->delete($val);
					break;
				case 'queue':
					$this->_queue->delete($val);
					break;
				case 'groupmacro':
					$this->_extensions->delete_exten($val);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_add();
					break;
				case 'user':
					if(is_array($val) === false)
						continue;

					foreach($val as $queuememberval)
						$this->_queuemember->delete($queuememberval);
					break;
				case 'rightcall':
					$this->_rightcall->reverse_add();
					break;
				case 'dialaction':
					$this->_dialaction->reverse_add();
					break;
				case 'contextnummember':
					$this->_contextnummember->reverse_add();
					break;
				case 'callerid':
					$this->_callerid->reverse_add();
					break;
			}
		}

		return(true);
	}

	function edit()
	{
		$this->_status = 'edit';
		$this->_return = array();

		if(is_array($this->_info) === false
		|| empty($this->_info) === true
		|| $this->get_errnb() > 0
		|| $this->edit_groupfeatures() === false)
		{
			$this->_status = '';
			return(false);
		}
		else if($this->edit_queue() === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}
		else if($this->_info['groupfeatures']['number'] !== '')
		{
			if($this->get_result('groupmacro') === null
			|| $this->get_result('extenumbers') === null)
			{
				$this->delete_macro('groupmacro');
				$this->delete_extenumbers();
			}
			else if($this->edit_groupmacro($this->_info['groupfeatures']['id']) === false
			|| $this->edit_extenumbers() === false)
			{
				$this->_reverse_edit();
				$this->_status = '';
				return(false);
			}
		}
		else if($this->add_groupmacro($this->_info['groupfeatures']['id']) === false
		|| $this->add_extenumbers($this->_info['groupfeatures']['id']) === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		if($this->edit_user($this->_info['groupfeatures']['name']) === false
		|| $this->edit_rightcall() === false
		|| $this->edit_dialaction() === false
		|| $this->edit_contextnummember() === false
		|| $this->edit_callerid() === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		$this->edit_user_callgroup($this->_info['groupfeatures']['id']);

		$this->_status = '';

		return(true);
	}

	function edit_groupfeatures()
	{
		if(dwho_issa('groupfeatures',$this->_info) === false
		|| ($rs = $this->get_result('groupfeatures')) === null
		|| $rs === false)
			return(false);

		$this->_return['groupfeatures'] = $this->_groupfeatures->edit($this->_info['groupfeatures']['id'],$rs);

		return($this->_return['groupfeatures']);
	}

	function edit_queue()
	{
		if(dwho_issa('queue',$this->_info) === false
		|| ($rs = $this->get_result('queue')) === null
		|| $rs === false
		|| $this->_queue->edit($this->_info['queue']['name'],$rs) === false)
			return(false);

		$this->_return['queue'] = $rs['name'];

		return($this->_return['queue']);
	}

	function edit_groupmacro($groupfeaturesid)
	{
		if(($rs = $this->_set_groupmacro('edit',$groupfeaturesid)) !== true)
			return($rs);

		return(parent::edit_macro('groupmacro'));
	}

	function edit_user($name)
	{
		if(is_scalar($name) === false)
			return(false);
		else if(dwho_issa('user',$this->_info) === false)
			return($this->add_user());

		$this->_return['user'] = $this->delete_user($name);

		if(($rs = $this->get_result('user')) === null)
			return($this->_return['user']);
		else if($rs === false)
			return(false);

		$ret = $this->_return['user'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_queuemember->add($rs[$i]) === false)
				return(false);

			$ret['queue_name'] = $rs[$i]['queue_name'];
			$ret['interface'] = $rs[$i]['interface'];

			$this->_return['user'][] = $ret;
		}

		if(isset($this->_return['user'][0]) === false)
			$this->_return['user'] = false;

		return(true);
	}

	function edit_rightcall()
	{
		if(dwho_issa('groupfeatures',$this->_info) === false)
			return(false);

		return(parent::edit_rightcall($this->_info['groupfeatures']['id']));
	}

	function edit_dialaction()
	{
		if(dwho_issa('groupfeatures',$this->_info) === false)
			return(false);

		return(parent::edit_dialaction($this->_info['groupfeatures']['id']));
	}

	function edit_contextnummember()
	{
		if(dwho_issa('groupfeatures',$this->_info) === false)
			return(false);

		return(parent::edit_contextnummember($this->_info['groupfeatures']['id']));
	}

	function edit_callerid()
	{
		if(dwho_issa('groupfeatures',$this->_info) === false)
			return(false);

		return(parent::edit_callerid($this->_info['groupfeatures']['id']));
	}

	function edit_user_callgroup($groupfeaturesid)
	{
		if(($groupfeaturesid = dwho_ulongint($groupfeaturesid)) === 0
		|| isset($this->_origin['user']) === false)
			return(false);
		else if(is_array($this->_origin['user']) === false
		|| ($nb = count($this->_origin['user'])) === 0)
			return($this->add_user_callgroup($groupfeaturesid));

		$userfeatures = &$this->_service->get_module('userfeatures');

		$rs = $this->get_result('user_callgroup');

		$callgroup = array('callgroup' => '');

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$this->_origin['user'][$i];

			if(is_array($rs) === false || isset($rs[$ref['userid']]) === false)
			{
				if(($uinfo = $userfeatures->get($ref['userid'])) === false
				|| $uinfo['protocol'] !== XIVO_SRE_IPBX_AST_PROTO_SIP
				|| ($protocol = &$this->_service->get_protocol_module($uinfo['protocol'])) === false
				|| ($pinfo = $protocol->get($uinfo['protocolid'])) === false
				|| array_key_exists('callgroup',$pinfo) === false
				|| is_array($pinfo['callgroup']) === false
				|| ($key = array_search($groupfeaturesid,$pinfo['callgroup'])) === false)
					continue;

				$callgroup['callgroup'] = $pinfo['callgroup'];
				unset($callgroup['callgroup'][$key]);
				$callgroup['callgroup'] = implode(',',$callgroup['callgroup']);

				$protocol->edit($pinfo['id'],$callgroup);
			}
			else if($rs[$ref['userid']] !== false)
			{
				$res = $rs[$ref['userid']];
				unset($rs[$ref['userid']]);

				if(($protocol = &$this->_service->get_protocol_module($res['protocol'])) === false
				|| ($pinfo = $protocol->get($res['protocolid'])) === false
				|| array_key_exists('callgroup',$pinfo) === false)
					continue;
				else if(is_array($pinfo['callgroup']) === false)
					$callgroup['callgroup'] = $groupfeaturesid;
				else if(in_array($groupfeaturesid,$pinfo['callgroup']) === true)
					continue;
				else
				{
					$callgroup['callgroup'] = $pinfo['callgroup'];
					$callgroup['callgroup'][] = $groupfeaturesid;
					$callgroup['callgroup'] = implode(',',$callgroup['callgroup']);
				}

				$protocol->edit($pinfo['id'],$callgroup);
			}
		}

		if(empty($rs) === false)
		{
			$this->_set_result('user_callgroup',$rs);
			$this->add_user_callgroup($groupfeaturesid);
		}

		return(true);
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
				case 'groupfeatures':
					if(isset($this->_origin['groupfeatures']) === false
					|| $this->_origin['groupfeatures'] === false)
						$this->delete_groupfeatures();
					else
						$this->_groupfeatures->edit_origin($this->_origin['groupfeatures']);
					break;
				case 'queue':
					if(isset($this->_origin['queue']) === false
					|| $this->_origin['queue'] === false)
						$this->_queue->delete($val);
					else
						$this->_queue->edit_origin($this->_origin['queue']);
					break;
				case 'groupmacro':
					if(isset($this->_origin['groupmacro']) === false
					|| $this->_origin['groupmacro'] === false)
						$this->delete_macro('groupmacro');
					else
						$this->_extensions->edit_origin($this->_origin['groupmacro']);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_edit($this->_status);
					break;
				case 'user':
					if(dwho_issa('groupfeatures',$this->_info) === false)
						continue;

					$this->delete_user($this->_info['groupfeatures']['name']);

					if(isset($this->_origin['user']) === true
					&& $this->_origin['user'] !== false)
						$this->_queuemember->add_origin_list($this->_origin['user']);
					break;
				case 'rightcall':
					$this->_rightcall->reverse_edit($this->_status);
					break;
				case 'dialaction':
					$this->_dialaction->reverse_edit($this->_status);
					break;
				case 'contextnummember':
					$this->_contextnummember->reverse_edit($this->_status);
					break;
				case 'callerid':
					$this->_callerid->reverse_edit($this->_status);
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

		if($this->delete_groupfeatures() === false
		|| $this->delete_queue() === false
		|| $this->delete_macro('groupmacro') === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_user($this->_info['groupfeatures']['name']) === false
		|| $this->delete_rightcall() === false
		|| $this->delete_dialaction() === false
		|| $this->delete_contextnummember() === false
		|| $this->delete_callerid() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}
		else
			$this->unlinks();

		$this->_status = '';

		return($r);
	}

	function delete_groupfeatures()
	{
		if(dwho_issa('groupfeatures',$this->_info) === false)
			return(false);

		$r = $this->_groupfeatures->delete($this->_info['groupfeatures']['id']);

		if($this->_status === 'delete')
			$this->_return['groupfeatures'] = $r;

		return($r);
	}

	function delete_queue()
	{
		if(dwho_issa('queue',$this->_info) === false)
			return(false);

		$r = $this->_queue->delete($this->_info['queue']['name']);

		if($this->_status === 'delete')
			$this->_return['queue'] = $r;

		return($r);
	}

	function delete_user($name)
	{
		if(dwho_issa('user',$this->_info) === false)
			return(null);
		else if(dwho_issa('groupfeatures',$this->_info) === false
		|| is_scalar($name) === false
		|| $this->_queuemember === false)
			return(false);

		$where = array();
		$where['usertype'] = 'user';
		$where['category'] = 'group';
		$where['queue_name'] = $name;

		$r = $this->_queuemember->delete_where($where);

		if($this->_status === 'delete')
			$this->_return['user'] = $r;

		return($r);
	}

	function delete_user_callgroup($groupfeaturesid)
	{
		$this->_set_result('user_callgroup',null);

		return($this->edit_user_callgroup($groupfeaturesid));
	}

	function _reverse_delete()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'delete'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'groupfeatures':
					if(dwho_issa('groupfeatures',$this->_origin) === true)
						$this->_groupfeatures->recover($this->_origin['groupfeatures']['id']);
					break;
				case 'queue':
					if(isset($this->_origin['queue']) === true
					&& $this->_origin['queue'] !== false)
						$this->_queue->add_origin($this->_origin['queue']);
					break;
				case 'groupmacro':
					if(isset($this->_origin['groupmacro']) === true
					&& $this->_origin['groupmacro'] !== false)
						$this->_extensions->add_origin($this->_origin['groupmacro']);
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === true
					&& $this->_origin['extenumbers'] !== false)
						$this->_extenumbers->add_origin($this->_origin['extenumbers']);
					break;
				case 'user':
					if(isset($this->_origin['user']) === true
					&& $this->_origin['user'] !== false)
						$this->_queuemember->add_origin_list($this->_origin['user']);
					break;
				case 'rightcall':
					$this->_rightcall->add_origin_list();
					break;
				case 'dialaction':
					$this->_dialaction->add_origin_list();
					break;
				case 'contextnummember':
					$this->_contextnummember->add_origin();
					break;
				case 'callerid':
					$this->_callerid->add_origin();
					break;
			}
		}

		return(true);
	}

	function unlinks()
	{
		if(dwho_issa('groupfeatures',$this->_info) === false)
			return(false);

		$this->delete_user_callgroup($this->_info['groupfeatures']['id']);
		$this->_dialaction->unlinked('group',$this->_info['groupfeatures']['id']);

		return(true);
	}

	function add_from_json()
	{
		if(($data = $this->_get_data_from_json()) === false
		|| $this->set_add($data) === false
		|| $this->add() === false)
			return(false);

		return(true);
	}

	function edit_from_json()
	{
		if(($data = $this->_get_data_from_json()) === false
		|| $this->set_edit($data) === false
		|| $this->edit() === false)
			return(false);

		return(true);
	}
}

?>
