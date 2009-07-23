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

class xivo_application_service_asterisk_incall extends xivo_application_asterisk_abstract
{
	var $_dso		= null;
	var $_incall		= false;
	var $_dialaction	= false;
	var $_extenumbers	= false;
	var $_extensions	= false;
	var $_rightcall		= false;
	var $_contextnummember	= false;
	var $_callerid		= false;
	var $_contextlist	= array();
	var $_sound		= array();

	function xivo_application_service_asterisk_incall(&$service,&$dso,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if(($this->_incall = &$this->_service->get_module('incall')) === false
		|| ($this->_dialaction = $this->get_abstract('dialaction',array('category' => 'incall'))) === false
		|| ($this->_extenumbers = $this->get_abstract('extenumbers',array('type' => 'incall'))) === false
		|| ($this->_extensions = &$this->_service->get_module('extensions')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_rightcall = $this->get_abstract('rightcall',array('type' => 'incall'))) === false
		|| ($this->_contextnummember = $this->get_abstract('contextnummember',array('type' => 'incall'))) === false
		|| ($this->_callerid = $this->get_abstract('callerid',array('type' => 'incall'))) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['incall'] = $this->_incall->get($id,$disable)) === false
		|| $this->_get_dialaction($this->_info['incall']['id']) === false
		|| ($this->_info['extenumbers'] = $this->_get_extenumbers($this->_info['incall']['id'])) === false
		|| $this->_get_exten() === false)
			return(false);

		$this->_origin['incall'] = $this->_incall->get_origin();
		$this->_origin['extenumbers'] = $this->_extenumbers->get_origin();

		if($this->_components === false)
			return($this->_info);

		$this->_get_rightcall($this->_info['incall']['id']);
		$this->_get_contextnummember($this->_info['incall']['id']);
		$this->_get_callerid($this->_info['incall']['id']);

		return($this->_info);
	}

	function _get_exten()
	{
		$this->_info['exten'] = false;
		$this->_origin['exten'] = false;

		if(($this->_info['exten'] = $this->_extensions->get_exten($this->_info['incall']['exten'],
									  $this->_info['incall']['context'],
									  'macro')) !== false)
			$this->_origin['exten'] = $this->_extensions->get_origin();

		return($this->_info['exten']);
	}

	function get_sound()
	{
		if(empty($this->_sound) === false)
			return($this->_sound);
		else if(($sounds = &$this->_service->get_module('sounds')) === false)
			return(false);
		else if(($this->_sound = $sounds->get_list('playback','pathnoext')) !== false)
			asort($this->_sound);

		return($this->_sound);
	}

	function get_elements()
	{
		$r = array();

		$r['incall'] = $this->_incall->get_element();
		$r['extenumbers'] = $this->_extenumbers->get_element();
		$r['dialaction'] = $this->_dialaction->get_element();
		$r['callerid'] = $this->_callerid->get_element();

		return($r);
	}

	function get_incalls_search($search,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_incalls('search',$search,$disable,$order,$limit,$assoc));
	}

	function get_incalls_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_incalls('list',null,$disable,$order,$limit,$assoc));
	}

	function _get_incalls($action,$search='',$disable=null,$order=null,$limit=null,$assoc=false)
	{
		$action = (string) $action;
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
			$ref['exten'] = $this->_service->display_extension($ref['exten']);

			$identity['exten'] = $ref['exten'];
			$identity['context'] = $ref['context'];

			$ref['destination'] = $ref['action'];
			$ref['identity'] = $this->_incall->mk_identity($identity);
			$ref['commented'] = (bool) $ref['commented'];
			$ref['linked'] = (bool) $ref['linked'];

			$arr = array();

			switch($ref['action'])
			{
				case 'endcall:hangup':
				case 'endcall:busy':
				case 'endcall:congestion':
				case 'application:callbackdisa':
				case 'application:disa':
				case 'application:directory':
				case 'application:faxtomail':
				case 'application:voicemailmain':
					list($ref['destination'],$ref['destidentity']) = explode(':',$ref['action']);
					break;
				case 'user':
					$arr['firstname'] = $ref['userfirstname'];
					$arr['lastname'] = $ref['userlastname'];
					$arr['number'] = $ref['usernumber'];
					$arr['context'] = $ref['usercontext'];
					$arr['name'] = $ref['username'];
					$ref['destidentity'] = $this->_service->identity('userfeatures',$arr);
					break;
				case 'group':
					$arr['name'] = $ref['groupname'];
					$arr['number'] = $ref['groupnumber'];
					$arr['context'] = $ref['groupcontext'];
					$ref['destidentity'] = $this->_service->identity('groupfeatures',$arr);
					break;
				case 'queue':
					$arr['name'] = $ref['queuename'];
					$arr['number'] = $ref['queuenumber'];
					$arr['context'] = $ref['queuecontext'];
					$ref['destidentity'] = $this->_service->identity('queuefeatures',$arr);
					break;
				case 'meetme':
					$arr['name'] = $ref['meetmename'];
					$arr['number'] = $ref['meetmenumber'];
					$arr['context'] = $ref['meetmecontext'];
					$ref['destidentity'] = $this->_service->identity('meetmefeatures',$arr);
					break;
				case 'voicemail':
					$arr['fullname'] = $ref['voicemailfullname'];
					$arr['mailbox'] = $ref['voicemailmailbox'];
					$arr['context'] = $ref['voicemailcontext'];
					$ref['destidentity'] = $this->_service->identity('voicemail',$arr);
					break;
				case 'schedule':
					$arr['name'] = $ref['schedulename'];
					$arr['context'] = $ref['schedulecontext'];
					$ref['destidentity'] = $this->_service->identity('schedule',$arr);
					break;
				case 'voicemenu':
					$arr['name'] = $ref['voicemenuname'];
					$arr['number'] = $ref['voicemenunumber'];
					$arr['context'] = $ref['voicemenucontext'];
					$ref['destidentity'] = $this->_service->identity('voicemenu',$arr);
					break;
				case 'sound':
					$ref['destidentity'] = basename($ref['actionarg1']);
					break;
				default:
					$ref['destidentity'] = '-';
			}
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
		$r = $this->get_destination_list();
		$r['sounds'] = $this->_dialaction->get_sound();

		return($r);
	}

	function _mk_exten_appdata($exten)
	{
		$arr = array();
		$arr['appdata'][0] = XIVO_SRE_IPBX_AST_INCALL_MACRO;
		$arr['appdata'][1] = $this->_service->clean_extension($exten);

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
		|| xivo_issa('incall',$arr) === false)
			return(false);

		$this->set_incall($arr['incall']);

		$number = (string) $this->get_result_var('incall','exten');
		$context = (string) $this->get_result_var('incall','context');

		if(xivo_issa('dialaction',$arr) === false
		|| $this->set_dialaction($arr['dialaction']) === null
		|| is_array(($dialaction_answer = $this->get_result_var('dialaction','answer'))) === false)
			$this->_set_error('dialaction','invalid data');
		else if(xivo_ak('action',$dialaction_answer,true) === 'application:faxtomail')
			$this->_set_result_var('incall','faxdetectenable',false);

		if(($rsexten = $this->set_macro($action,'exten',$number,$this->_mk_exten_appdata($number),$context)) !== false)
			$this->_set_result('exten',$rsexten);

		$this->set_extenumbers($action,$number,$context);

		if(xivo_issa('rightcall',$arr) === true)
			$this->set_rightcall($arr['rightcall']);

		if($this->get_errnb() < 1)
			$this->set_contextnummember($context,$number);

		if(xivo_issa('callerid',$arr) === true)
			$this->set_callerid($arr['callerid']);

		if($this->get_errnb() < 1
		&& (bool) $this->get_result_var('incall','faxdetectenable') === true
		&& xivo_haslen($this->get_result_var('incall','faxdetectemail')) === false)
			$this->_set_result_var('incall','faxdetectenable',false);

		return(($this->get_errnb() < 1));
	}

	function set_incall($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('incall',null);
			$this->_set_error('incall','invalid data');
			return(false);
		}

		$arr['commented'] = false;

		if(($rs = $this->_incall->chk_values($arr)) === false)
		{
			$this->_set_result('incall',$this->_incall->get_filter_result());
			$this->_set_error('incall',$this->_incall->get_filter_error());
			return(false);
		}

		$this->_set_result('incall',$rs);

		return(true);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$incallid = null;

		if($this->get_errnb() > 0
		|| ($incallid = $this->add_incall()) === false
		|| $this->add_dialaction($incallid) === false
		|| $this->add_extenumbers($incallid) === false
		|| $this->add_macro('exten') === false
		|| $this->add_rightcall($incallid) === false
		|| $this->add_contextnummember($incallid) === false
		|| $this->add_callerid($incallid) === false)
		{
			$r = false;
			if($incallid !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_incall()
	{
		if(($rs = $this->get_result('incall')) === null
		|| $rs === false)
			return(false);

		return(($this->_return['incall'] = $this->_incall->add($rs)));
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
				case 'incall':
					$this->_incall->delete($val);
					break;
				case 'dialaction':
					$this->_dialaction->reverse_add();
					break;
				case 'exten':
					$this->_extensions->delete_exten($val);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_add();
					break;
				case 'rightcall':
					$this->_rightcall->reverse_add();
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
		$r = true;

		$this->_status = 'edit';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_incall() === false
		|| $this->edit_dialaction() === false
		|| $this->edit_extenumbers() === false
		|| $this->edit_macro('exten') === false
		|| $this->edit_rightcall() === false
		|| $this->edit_contextnummember() === false
		|| $this->edit_callerid() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_incall()
	{
		if(xivo_issa('incall',$this->_info) === false
		|| ($rs = $this->get_result('incall')) === null
		|| $rs === false)
			return(false);

		$rs['commented'] = $this->_info['incall']['commented'];

		return(($this->_return['incall'] = $this->_incall->edit($this->_info['incall']['id'],$rs)));
	}

	function edit_dialaction()
	{
		if(xivo_issa('incall',$this->_info) === false)
			return(false);

		return(parent::edit_dialaction($this->_info['incall']['id']));
	}

	function edit_rightcall()
	{
		if(xivo_issa('incall',$this->_info) === false)
			return(false);

		return(parent::edit_rightcall($this->_info['incall']['id']));
	}

	function edit_contextnummember()
	{
		if(xivo_issa('incall',$this->_info) === false)
			return(false);

		return(parent::edit_contextnummember($this->_info['incall']['id']));
	}

	function edit_callerid()
	{
		if(xivo_issa('incall',$this->_info) === false)
			return(false);

		return(parent::edit_callerid($this->_info['incall']['id']));
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
				case 'incall':
					if(isset($this->_origin['incall']) === false
					|| $this->_origin['incall'] === false)
						$this->delete_incall();
					else
						$this->_incall->edit_origin($this->_origin['incall']);
					break;
				case 'dialaction':
					$this->_dialaction->reverse_edit($this->_status);
					break;
				case 'exten':
					if(isset($this->_origin['exten']) === false
					|| $this->_origin['exten'] === false)
						$this->delete_macro('exten');
					else
						$this->_extensions->edit_origin($this->_origin['exten']);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_edit($this->_status);
					break;
				case 'rightcall':
					$this->_rightcall->reverse_edit($this->_status);
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

		if($this->delete_incall() === false
		|| $this->delete_dialaction() === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_macro('exten') === false
		|| $this->delete_rightcall() === false
		|| $this->delete_contextnummember() === false
		|| $this->delete_callerid() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
	}

	function delete_incall()
	{
		if(xivo_issa('incall',$this->_info) === false)
			return(false);

		$r = $this->_incall->delete($this->_info['incall']['id']);

		if($this->_status === 'delete')
			$this->_return['incall'] = $r;

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
				case 'incall':
					if(isset($this->_origin['incall']) === true
					&& $this->_origin['incall'] !== false)
						$this->_incall->add_origin($this->_origin['incall']);
					break;
				case 'dialaction':
					$this->_dialaction->add_origin_list();
					break;
				case 'exten':
					if(isset($this->_origin['exten']) === true
					&& $this->_origin['exten'] !== false)
						$this->_extensions->add_origin($this->_origin['exten']);
					break;
				case 'rightcall':
					$this->_rightcall->add_origin_list();
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === true
					&& $this->_origin['extenumbers'] !== false)
						$this->_extenumbers->add_origin($this->_origin['extenumbers']);
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

	function enable()
	{
		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_incall() === false
		|| $this->enable_exten() === false)
		{
			$this->_reverse_enable_disable();
			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function disable()
	{
		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_incall() === false
		|| $this->disable_exten() === false)
		{
			$this->_reverse_enable_disable();
			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function enable_incall()
	{
		return($this->_enable_disable_incall(false));
	}

	function disable_incall()
	{
		return($this->_enable_disable_incall(true));
	}

	function _enable_disable_incall($disable=false)
	{
		if(xivo_issa('incall',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_incall->enable($this->_info['incall']['id']);
		else
			$r = $this->_incall->disable($this->_info['incall']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['incall'] = $r;

		return($r);
	}

	function enable_exten()
	{
		return($this->_enable_disable_exten(false));
	}

	function disable_exten()
	{
		return($this->_enable_disable_exten(true));
	}

	function _enable_disable_exten($disable=false)
	{
		if(xivo_issa('incall',$this->_info) === false)
			return(false);

		$where = array();
		$where['exten'] = $this->_info['incall']['exten'];
		$where['context'] = $this->_info['incall']['context'];

		if((bool) $disable === false)
			$r = $this->_extensions->enable_where($where);
		else
			$r = $this->_extensions->disable_where($where);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['exten'] = $r;

		return($r);
	}

	function _reverse_enable_disable()
	{
		if(($this->_status !== 'enable' && $this->_status !== 'disable') === true
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		$disable = $this->_status === 'enable';

		$this->_status = '';

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'incall':
					$this->_enable_disable_incall($disable);
					break;
				case 'exten':
					$this->_enable_disable_exten($disable);
					break;
			}
		}

		return(true);
	}

	function add_from_json()
	{
		if(($data = $this->_get_data_from_json()) === false
		|| xivo_issa('incall',$data) === false
		|| xivo_haslen($data['incall'],'exten') === false)
			return(false);

		$data['dialaction'] = array();
		$data['dialaction']['answer'] = array();

		if(isset($data['incall']['destination']) === true)
			$data['dialaction']['answer']['actiontype'] = $data['incall']['destination'];

		if(isset($data['incall']['action']) === true)
			$data['dialaction']['answer']['action'] = $data['incall']['action'];

		if(isset($data['incall']['arg1']) === true)
			$data['dialaction']['answer']['actionarg1'] = $data['incall']['arg1'];

		if(isset($data['incall']['arg2']) === true)
			$data['dialaction']['answer']['actionarg2'] = $data['incall']['arg2'];

		if($this->set_add($data) === false
		|| $this->add() === false)
			return(false);

		return(true);
	}
}

?>
