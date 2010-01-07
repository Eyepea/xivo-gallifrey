<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

require_once(dwho_file::joinpath(XIVO_PATH_OBJECT,'application','service','abstract.inc'));

class xivo_application_asterisk_abstract extends xivo_application_service_abstract
{
	var $_service		= null;
	var $_service_type	= 'ipbx';
	var $_service_name	= 'asterisk';

	function xivo_application_asterisk_abstract(&$service)
	{
		if(dwho_is_instance($service,'xivo_service_asterisk') === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_service = &$service;
	}

	function mk_interface($name,$chantype,$context='',$number='',$protocol='')
	{
		$chantype = strval($chantype);
		$number = strval($number);

		$r = array();

		if($protocol === XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
		{
			$r['interface'] = $this->_service->mk_interface($name,$protocol);
			$r['channel'] = XIVO_SRE_IPBX_AST_CHAN_UNKNOWN;

			if($r['interface'] === false)
				return(false);

			return($r);
		}

		switch($chantype)
		{
			case XIVO_SRE_IPBX_AST_CHAN_LOCAL:
				$r['interface'] = $this->_service->mk_interface($name,null,$number,$context);

				if($number === '' || $r['interface'] === false)
					return(false);

				$r['channel'] = XIVO_SRE_IPBX_AST_CHAN_LOCAL;
				break;
			default:
				$r['interface'] = $this->_service->mk_interface($name,$protocol);
				$r['channel'] = $this->_service->get_channel_by_protocol($protocol);

				if($r['interface'] === false || $r['channel'] === false)
					return(false);
		}

		return($r);
	}

	function mk_agent_interface($number,$group=false)
	{
		$r = array();
		$r['channel'] = XIVO_SRE_IPBX_AST_CHAN_AGENT;

		if(($r['interface'] = $this->_service->mk_agent_interface($number,$group)) === false)
			return(false);

		return($r);
	}

	function get_context_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		if(isset($this->_contextlist) === false)
			return(false);
		else if(empty($this->_contextlist) === false)
			return($this->_contextlist);
		else if(($context = &$this->_service->get_module('context')) === false)
			return(false);

		if($order === null)
		{
			$order = array();
			$order['displayname'] = SORT_ASC;
			$order['name'] = SORT_ASC;
		}

		return(($this->_contextlist = $context->get_all($disable,true,$order,$limit,$assoc)));
	}

	function get_musiconhold()
	{
		if(isset($this->_musiconhold) === false)
			return(false);
		else if(empty($this->_musiconhold) === false)
			return($this->_musiconhold);
		else if(($musiconhold = &$this->_service->get_module('musiconhold')) === false)
			return(false);
		else if(($this->_musiconhold = $musiconhold->get_all_category(null,false)) !== false)
			ksort($this->_musiconhold);

		return($this->_musiconhold);
	}

	function _get_extenumbers($typeval)
	{
		$this->_info['extenumbers'] = false;
		$this->_origin['extenumbers'] = false;

		if(($this->_info['extenumbers'] = $this->_extenumbers->get($typeval)) !== false)
			$this->_origin['extenumbers'] = $this->_extenumbers->get_origin();

		return($this->_info['extenumbers']);
	}

	function _get_rightcall($typeval)
	{
		$this->_info['rightcall'] = false;
		$this->_origin['rightcall'] = false;

		if(($this->_info['rightcall'] = $this->_rightcall->get($typeval)) !== false)
			$this->_origin['rightcall'] = $this->_rightcall->get_origin();

		return($this->_info['rightcall']);
	}

	function _get_dialaction($categoryval)
	{
		$this->_info['dialaction'] = false;
		$this->_origin['dialaction'] = false;

		if(($this->_info['dialaction'] = $this->_dialaction->get($categoryval)) !== false)
			$this->_origin['dialaction'] = $this->_dialaction->get_origin();

		return($this->_info['dialaction']);
	}

	function _get_contextnummember($typeval)
	{
		$this->_info['contextnummember'] = false;
		$this->_origin['contextnummember'] = false;

		if(($this->_info['contextnummember'] = $this->_contextnummember->get($typeval)) !== false)
			$this->_origin['contextnummember'] = $this->_contextnummember->get_origin();

		return($this->_info['contextnummember']);
	}

	function _get_contextmember($typeval='',$varname='')
	{
		$this->_info['contextmember'] = false;
		$this->_origin['contextmember'] = false;

		if(($this->_info['contextmember'] = $this->_contextmember->get($typeval,$varname)) !== false)
			$this->_origin['contextmember'] = $this->_contextmember->get_origin();

		return($this->_info['contextmember']);
	}

	function _get_callerid($typeval)
	{
		$this->_info['callerid'] = false;
		$this->_origin['callerid'] = false;

		if(($this->_info['callerid'] = $this->_callerid->get($typeval)) !== false)
			$this->_origin['callerid'] = $this->_callerid->get_origin();

		return($this->_info['callerid']);
	}

	function set_extenumbers($action,$number,$context='',$typeval='')
	{
		if(($r = $this->_extenumbers->set($action,$number,$context,$typeval)) === false)
			$this->_set_error('extenumbers',$this->_extenumbers->get_error());

		$this->_set_result('extenumbers',$this->_extenumbers->get_result());

		return($r);
	}

	function chk_macro($name,$exten,$arr,$context)
	{
		if(($r = $this->_extensions->chk_exten($exten,$context,'macro',$arr)) !== false)
			return($r);
		else if($this->_extensions->get_filter_errnb() > 0)
			$this->_set_error($name,$this->_extensions->get_filter_error());
		else
			$this->_set_error($name,'invalid macro');

		return($r);
	}

	function set_macro($action,$name,$exten,$arr,$context)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| dwho_has_len($name) === false)
			return(false);
		else if($action === 'edit' && dwho_issa($name,$this->_info) !== false)
			$r = $this->_extensions->chk_exten($exten,$context,'macro',$arr);
		else
			$r = $this->_extensions->new_exten($exten,$context,'macro',$arr);

		if($r !== false)
			return($r);
		else if($this->_extensions->get_filter_errnb() > 0)
			$this->_set_error($name,$this->_extensions->get_filter_error());
		else
			$this->_set_error($name,'invalid macro');

		return($r);
	}

	function set_rightcall($list)
	{
		if(($r = $this->_rightcall->set($list)) === false)
			$this->_set_error('rightcall',$this->_rightcall->get_error());

		$this->_set_result('rightcall',$this->_rightcall->get_result());

		return($r);
	}

	function set_dialaction($list)
	{
		if(($r = $this->_dialaction->set($list)) === false)
			$this->_set_error('dialaction',$this->_dialaction->get_error());

		$this->_set_result('dialaction',$this->_dialaction->get_result());

		return($r);
	}

	function set_contextnummember($context,$number='')
	{
		if(($r = $this->_contextnummember->set($context,$number)) === false)
			$this->_set_error('contextnummember',$this->_contextnummember->get_error());

		$this->_set_result('contextnummember',$this->_contextnummember->get_result());

		return($r);
	}

	function set_contextmember($list)
	{
		if(($r = $this->_contextmember->set($list)) === false)
			$this->_set_error('contextmember',$this->_contextmember->get_error());

		$this->_set_result('contextmember',$this->_contextmember->get_result());

		return($r);
	}

	function set_callerid($arr)
	{
		if(($r = $this->_callerid->set($arr)) === false)
			$this->_set_error('callerid',$this->_callerid->get_error());

		$this->_set_result('callerid',$this->_callerid->get_result());

		return($r);
	}

	function add_extenumbers($typeval)
	{
		if(($rs = $this->_extenumbers->add($typeval)) === null || $rs === false)
			return($rs);

		return(($this->_return['extenumbers'] = $this->_extenumbers->get_return()));
	}

	function add_macro($name)
	{
		if(($rs = $this->get_result($name)) === null || $rs === false)
			return($rs);

		return(($this->_return[$name] = $this->_extensions->add_exten($rs)));
	}

	function add_rightcall($typeval)
	{
		$r = $this->_rightcall->add($typeval);

		$this->_return['rightcall'] = $this->_rightcall->get_return();

		return($r);
	}

	function add_dialaction($categoryval)
	{
		$r = $this->_dialaction->add($categoryval);

		$this->_return['dialaction'] = $this->_dialaction->get_return();

		return($r);
	}

	function add_contextnummember($typeval)
	{
		$r = $this->_contextnummember->add($typeval);

		$this->_return['contextnummember'] = $this->_contextnummember->get_return();

		return($r);
	}

	function add_contextmember($typeval='')
	{
		$r = $this->_contextmember->add($typeval);

		$this->_return['contextmember'] = $this->_contextmember->get_return();

		return($r);
	}

	function add_callerid($typeval)
	{
		$r = $this->_callerid->add($typeval);

		$this->_return['callerid'] = $this->_callerid->get_return();

		return($r);
	}

	function edit_extenumbers()
	{
		if(($rs = $this->_extenumbers->edit()) === null || $rs === false)
			return($rs);

		return(($this->_return['extenumbers'] = $this->_extenumbers->get_return()));
	}

	function edit_macro($name)
	{
		$rs = false;

		if(dwho_issa($name,$this->_info) === false
		|| ($rs = $this->get_result($name)) === null
		|| $rs === false)
			return($rs);

		$this->_return[$name] = $this->_extensions->edit($this->_info[$name]['id'],$rs);

		return($this->_return[$name]);
	}

	function edit_rightcall($typeval)
	{
		$r = $this->_rightcall->edit($this->_status,$typeval);

		$this->_return['rightcall'] = $this->_rightcall->get_return();

		return($r);
	}

	function edit_dialaction($categoryval)
	{
		$r = $this->_dialaction->edit($this->_status,$categoryval);

		$this->_return['dialaction'] = $this->_dialaction->get_return();

		return($r);
	}

	function edit_contextnummember($typeval)
	{
		$r = $this->_contextnummember->edit($this->_status,$typeval);

		$this->_return['contextnummember'] = $this->_contextnummember->get_return();

		return($r);
	}

	function edit_contextmember($typeval='')
	{
		$r = $this->_contextmember->edit($this->_status,$typeval);

		$this->_return['contextmember'] = $this->_contextmember->get_return();

		return($r);
	}

	function edit_callerid($typeval)
	{
		$r = $this->_callerid->edit($this->_status,$typeval);

		$this->_return['callerid'] = $this->_callerid->get_return();

		return($r);
	}

	function save_extenumbers($typeval)
	{
		if(($rs = $this->_extenumbers->save($this->_status,$typeval)) === null || $rs === false)
			return($rs);

		return(($this->_return['extenumbers'] = $this->_extenumbers->get_return()));
	}

	function delete_extenumbers()
	{
		if(($rs = $this->_extenumbers->delete($this->_status)) === null
		|| $rs === false)
			return($rs);

		if($this->_status === 'delete')
			$this->_return['extenumbers'] = $rs;

		return($rs);
	}

	function delete_macro($name)
	{
		if(dwho_issa($name,$this->_info) === false)
			return(null);

		$r = $this->_extensions->delete($this->_info[$name]['id']);

		if($this->_status === 'delete')
			$this->_return[$name] = $r;

		return($r);
	}

	function delete_rightcall()
	{
		$r = $this->_rightcall->delete($this->_status);

		if($this->_status === 'delete')
			$this->_return['rightcall'] = $r;

		return($r);
	}

	function delete_dialaction()
	{
		$r = $this->_dialaction->delete($this->_status);

		if($this->_status === 'delete')
			$this->_return['dialaction'] = $r;

		return($r);
	}

	function delete_contextnummember()
	{
		$r = $this->_contextnummember->delete($this->_status);

		if($this->_status === 'delete')
			$this->_return['contextnummember'] = $r;

		return($r);
	}

	function delete_contextmember()
	{
		$r = $this->_contextmember->delete($this->_status);

		if($this->_status === 'delete')
			$this->_return['contextmember'] = $r;

		return($r);
	}

	function delete_callerid()
	{
		$r = $this->_callerid->delete($this->_status);

		if($this->_status === 'delete')
			$this->_return['callerid'] = $r;

		return($r);
	}

	function get_destination_list($type='',$id=0,$number=false)
	{
		$r = array();

		$userid = $groupid = $queueid = $scheduleid = 0;

		if($type === 'user')
			$userid = dwho_ulongint($id);
		else if($type === 'group')
			$groupid = dwho_ulongint($id);
		else if($type === 'queue')
			$queueid = dwho_ulongint($id);
		else if($type === 'schedule')
			$scheduleid = dwho_ulongint($id);

		$r['users'] = $this->get_destination_users($userid,$number);
		$r['groups'] = $this->get_destination_groups($groupid,$number);
		$r['queues'] = $this->get_destination_queues($queueid,$number);
		$r['meetme'] = $this->get_destination_meetme(0,$number);
		$r['voicemail'] = $this->get_destination_voicemail();
		$r['schedule'] = $this->get_destination_schedule($scheduleid);
		$r['voicemenu'] = $this->get_destination_voicemenu();

		return($r);
	}

	function get_destination_users($id=0,$number=false)
	{
		$order = array();
		$order['firstname'] = SORT_ASC;
		$order['lastname'] = SORT_ASC;
		$order['number'] = SORT_ASC;
		$order['context'] = SORT_ASC;
		$order['name'] = SORT_ASC;

		if(($userfeatures = &$this->_service->get_module('userfeatures')) !== false
		&& ($r = $userfeatures->get_all_except($id,$number,$order)) !== false)
			return($r);

		return(false);
	}

	function get_destination_groups($id=0,$number=false)
	{
		$order = array();
		$order['name'] = SORT_ASC;
		$order['number'] = SORT_ASC;
		$order['context'] = SORT_ASC;

		if(($groupfeatures = &$this->_service->get_module('groupfeatures')) !== false
		&& ($r = $groupfeatures->get_all_except($id,$number,$order)) !== false)
			return($r);

		return(false);
	}

	function get_destination_queues($id=0,$number=false)
	{
		$order = array();
		$order['name'] = SORT_ASC;
		$order['number'] = SORT_ASC;
		$order['context'] = SORT_ASC;

		if(($queuefeatures = &$this->_service->get_module('queuefeatures')) !== false
		&& ($r = $queuefeatures->get_all_except($id,$number,$order)) !== false)
			return($r);

		return(false);
	}

	function get_destination_meetme($id=0,$number=false)
	{
		$order = array();
		$order['name'] = SORT_ASC;
		$order['number'] = SORT_ASC;
		$order['context'] = SORT_ASC;

		if(($meetmefeatures = &$this->_service->get_module('meetmefeatures')) !== false
		&& ($r = $meetmefeatures->get_all_except($id,$number,$order)) !== false)
			return($r);

		return(false);
	}

	function get_destination_voicemail()
	{
		$order = array();
		$order['fullname'] = SORT_ASC;
		$order['mailbox'] = SORT_ASC;
		$order['context'] = SORT_ASC;

		if(($voicemail = &$this->_service->get_module('voicemail')) !== false
		&& ($r = $voicemail->get_all(null,true,$order)) !== false)
			return($r);

		return(false);
	}

	function get_destination_schedule($id=0)
	{
		$order = array();
		$order['name'] = SORT_ASC;
		$order['context'] = SORT_ASC;

		if(($schedule = &$this->_service->get_module('schedule')) !== false
		&& ($r = $schedule->get_all_except($id,$order)) !== false)
			return($r);

		return(false);
	}

	function get_destination_voicemenu()
	{
		$order = array();
		$order['name'] = SORT_ASC;
		$order['number'] = SORT_ASC;
		$order['context'] = SORT_ASC;

		if(($voicemenu = &$this->_service->get_module('voicemenu')) !== false
		&& ($r = $voicemenu->get_all(null,true,$order)) !== false)
			return($r);

		return(false);
	}
}

?>
