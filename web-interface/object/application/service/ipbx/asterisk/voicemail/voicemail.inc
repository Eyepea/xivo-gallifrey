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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_voicemail extends xivo_application_asterisk_abstract
{
	var $_dso		= null;
	var $_voicemail		= false;
	var $_voicemailfeatures	= false;
	var $_contextmember	= false;
	var $_zonemessages	= null;
	var $_timezones		= array();
	var $_contextlist	= array();
	var $_modules		= array();

	function xivo_application_service_asterisk_voicemail(&$service,&$dso,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if(($this->_voicemailfeatures = &$this->_service->get_module('voicemailfeatures')) === false
		|| ($this->_voicemail = &$this->_service->get_module('voicemail')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'voicemail'))) === false)
			return(false);
		else if(($appvoicemail = &$this->_service->get_apprealstatic('voicemail')) !== false)
			$this->_zonemessages = &$appvoicemail->get_module('zonemessages');

		return(true);
	}

	function get($id,$disable=null,$nocomponents=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['voicemail'] = $this->_voicemail->get($id,$disable)) === false
		|| ($this->_info['voicemailfeatures'] = $this->_voicemailfeatures->get_where(
							array('voicemailid' => $this->_info['voicemail']['uniqueid']))) === false)
			return(false);

		$this->_origin['voicemailfeatures'] = $this->_voicemailfeatures->get_origin();
		$this->_origin['voicemail'] = $this->_voicemail->get_origin();

		if($this->_components === false)
			return($this->_info);

		if(dwho_ak('contextmember',$nocomponents,true) === false)
			$this->_get_contextmember($this->_info['voicemail']['uniqueid']);

		return($this->_info);
	}

	function get_nb($arr=null,$disable=null)
	{
		return($this->_voicemail->get_nb($arr,$disable));
	}

	function get_by_mailbox_context($mailbox,$context,$disable=null,$nocomponents=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['voicemail'] = $this->_voicemail->get_where(
							array('mailbox'	=> $mailbox,
							      'context'	=> $context),$disable)) === false
		|| ($this->_info['voicemailfeatures'] = $this->_voicemailfeatures->get_where(
							array('voicemailid' => $this->_info['voicemail']['uniqueid']))) === false)
			return(false);

		$this->_origin['voicemailfeatures'] = $this->_voicemailfeatures->get_origin();
		$this->_origin['voicemail'] = $this->_voicemail->get_origin();

		if($this->_components === false)
			return($this->_info);

		if(dwho_ak('contextmember',$nocomponents,true) === false)
			$this->_get_contextmember($this->_info['voicemail']['uniqueid']);

		return($this->_info);
	}

	function get_timezones()
	{
		if(is_object($this->_zonemessages) === false)
			return(false);
		else if(is_array($this->_timezones) === true
		&& empty($this->_timezones) === false)
			return($this->_timezones);

		return(($this->_timezones = $this->_zonemessages->get_all_name()));
	}

	function get_elements()
	{
		$r = array();
		$r['voicemail'] = $this->_voicemail->get_element();
		$r['voicemailfeatures'] = $this->_voicemailfeatures->get_element();

		return($r);
	}

	function get_voicemail_search($search,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_voicemail('search',$disable,$search,$order,$limit,$assoc));
	}

	function get_voicemail_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_voicemail('list',$disable,null,$order,$limit,$assoc));
	}

	function _get_voicemail($action,$disable=null,$search='',$order=null,$limit=null,$assoc=false)
	{
		$search = strval($search);

		$rs = false;

		switch($action)
		{
			case 'search':
				$rs = $this->_dso->search($search,$disable,$order,$limit);

				$this->_cnt = $this->_dso->get_cnt();

				if($rs === false || ($nb = count($rs)) === 0)
					return(false);

				$r = $identity = array();

				for($i = 0;$i < $nb;$i++)
				{
					$r[$rs[$i]['uniqueid']] = $rs[$i];

					$ref = &$r[$rs[$i]['uniqueid']];

					$identity['mailbox'] = $ref['mailbox'];
					$identity['fullname'] = $ref['fullname'];
					$identity['context'] = $ref['context'];

					$ref['identity'] = $this->_voicemail->mk_identity($identity);
					$ref['commented'] = (bool) $ref['commented'];
				}

				if(empty($r) === true)
					return(false);
				else if((bool) $assoc === false)
					return(array_values($r));

				return($r);
			case 'list':
			default:
				if(($rs = $this->_voicemail->get_all($disable,true,$order,$limit,$assoc)) !== false
				&& empty($rs) === true)
					$rs = false;

				$this->_cnt = $this->_voicemail->get_cnt();
		}

		return($rs);
	}

	function set_add($arr)
	{
		return($this->_set('add',$arr));
	}

	function set_edit($arr,$setval=true)
	{
		if(empty($this->_info) === true)
			return(false);

		return($this->_set('edit',$arr,$setval));
	}

	function _set($action,$arr,$setval=true)
	{
		$this->_reset();

		if(($action !== 'add' && $action !== 'edit') === true
		|| dwho_issa('voicemail',$arr) === false)
			return(false);

		$this->set_voicemail($arr['voicemail'],$setval);

		if(dwho_issa('voicemailfeatures',$arr) === true)
			$this->set_voicemailfeatures($arr['voicemailfeatures']);
		else
			$this->set_voicemailfeatures();

		if($action === 'edit'
		&& (bool) $setval === false
		&& (string) $this->get_result_var('voicemail','context') !== '')
			$this->_set_result_var('voicemail','context',$this->_info['voicemail']['context']);

		$this->set_contextmember();

		return(($this->get_errnb() < 1));
	}

	function set_voicemail($arr,$setval=true)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('voicemail',null);
			$this->_set_error('voicemail','invalid data');
			return(false);
		}

		$arr['commented'] = false;

		if(($rs = $this->_voicemail->chk_values($arr,true,$setval)) === false)
		{
			$this->_set_result('voicemail',$this->_voicemail->get_filter_result());
			$this->_set_error('voicemail',$this->_voicemail->get_filter_error());
			return(false);
		}
		else if(dwho_has_len($rs,'tz') === true
		&& ($this->get_timezones() === false
		   || isset($this->_timezones[$rs['tz']]) === false) === true)
		{
			$this->_set_result('voicemail',$rs);
			$this->_set_error('voicemail','invalid tz');
			return(false);
		}

		$this->_set_result('voicemail',$rs);

		return(true);
	}

	function set_voicemailfeatures($arr=array())
	{
		if(is_array($arr) === false)
			$arr = array();

		$arr['voicemailid'] = 0;

		if(($rs = $this->_voicemailfeatures->chk_values($arr)) === false)
		{
			$this->_set_result('voicemailfeatures',$this->_voicemailfeatures->get_filter_result());
			$this->_set_error('voicemailfeatures',$this->_voicemailfeatures->get_filter_error());
			return(false);
		}

		$this->_set_result('voicemailfeatures',$rs);

		return(true);
	}

	function set_contextmember()
	{
		$rs = array();
		$rs['voicemail-context']['context'] = (string) $this->get_result_var('voicemail','context');
		$rs['voicemail-context']['typeval'] = 0;
		$rs['voicemail-context']['varname'] = 'context';

		if(($dialout = (string) $this->get_result_var('voicemail','dialout')) !== '')
		{
			$rs['voicemail-dialout']['context'] = $dialout;
			$rs['voicemail-dialout']['typeval'] = 0;
			$rs['voicemail-dialout']['varname'] = 'dialout';
		}

		if(($callback = (string) $this->get_result_var('voicemail','callback')) !== '')
		{
			$rs['voicemail-callback']['context'] = $callback;
			$rs['voicemail-callback']['typeval'] = 0;
			$rs['voicemail-callback']['varname'] = 'callback';
		}

		if(($exitcontext = (string) $this->get_result_var('voicemail','exitcontext')) !== '')
		{
			$rs['voicemail-exitcontext']['context'] = $exitcontext;
			$rs['voicemail-exitcontext']['typeval'] = 0;
			$rs['voicemail-exitcontext']['varname'] = 'exitcontext';
		}

		return(parent::set_contextmember($rs));
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$voicemailid = null;

		if($this->get_errnb() > 0
		|| ($voicemailid = $this->add_voicemail()) === false
		|| $this->add_voicemailfeatures($voicemailid) === false
		|| $this->add_contextmember($voicemailid) === false)
		{
			$r = false;

			if($voicemailid !== null)
				$this->reverse_add($voicemailid);
		}

		$this->_status = '';

		return($r);
	}

	function add_voicemail()
	{
		if(($rs = $this->get_result('voicemail')) === null
		|| $rs === false)
			return(false);

		return(($this->_return['voicemail'] = $this->_voicemail->add($rs)));
	}

	function add_voicemailfeatures($voicemailid)
	{
		if(($rs = $this->get_result('voicemailfeatures')) === null
		|| $rs === false
		|| ($rs['voicemailid'] = dwho_ulongint($voicemailid)) === 0)
			return(false);

		return(($this->_return['voicemailfeatures'] = $this->_voicemailfeatures->add($rs)));
	}

	function reverse_add($voicemailid)
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'add'
		|| ($voicemailid = dwho_ulongint($voicemailid)) === 0
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'voicemail':
					$this->_voicemail->delete($voicemailid);
					break;
				case 'voicemailfeatures':
					$this->_voicemailfeatures->delete($val);
					break;
				case 'contextmember':
					$this->_contextmember->reverse_add();
					break;
			}
		}

		return(true);
	}

	function edit($chkunique=true)
	{
		$r = true;

		$this->_status = 'edit';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_voicemail($chkunique) === false
		|| $this->edit_voicemailfeatures() === false
		|| $this->edit_contextmember() === false)
		{
			$r = false;
			$this->reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_voicemail($chkunique=true)
	{
		if(dwho_issa('voicemail',$this->_origin) === false
		|| ($rs = $this->get_result('voicemail')) === null
		|| $rs === false)
			return(false);

		$rs['commented'] = $this->_info['voicemail']['commented'];

		if(($this->_return['voicemail'] = $this->_voicemail->edit(
								$this->_origin['voicemail']['uniqueid'],
								$rs,
								null,
								$chkunique)) === false)
			return(false);

		if(isset($rs['mailbox'],$rs['context']) === true
		&& ($this->_info['voicemail']['mailbox'] !== (string) $rs['mailbox']
		   || $this->_info['voicemail']['context'] !== (string) $rs['context']) === true
		&& $this->_update_user_mailbox($this->_origin['voicemail']['mailbox'].
					       '@'.$this->_origin['voicemail']['context'],
					       $rs['mailbox'].
					       '@'.$rs['context']) === false)
			return(false);

		return($this->_return['voicemail']);
	}

	function edit_voicemailfeatures()
	{
		if(dwho_issa('voicemailfeatures',$this->_info) === false
		|| ($rs = $this->get_result('voicemailfeatures')) === null
		|| $rs === false)
			return(false);

		$rs['voicemailid'] = $this->_info['voicemailfeatures']['voicemailid'];

		return(($this->_return['voicemailfeatures'] = $this->_voicemailfeatures->edit($this->_info['voicemailfeatures']['id'],$rs)));
	}

	function edit_contextmember()
	{
		if(dwho_issa('voicemail',$this->_info) === false)
			return(false);

		return(parent::edit_contextmember($this->_info['voicemail']['uniqueid']));
	}

	function _update_user_mailbox($mailbox,$newmailbox)
	{
		if(dwho_issa('usermailbox',$this->_return) === false)
			$this->_return['usermailbox'] = array();

		$ref = &$this->_return['usermailbox'];

		$ref['_mailbox'] = $mailbox;
		$ref['_newmailbox'] = $newmailbox;

		$where = array('mailbox' => $mailbox);
		$value = array('mailbox' => $newmailbox);

		if(((isset($this->_modules['usersip']) === false
		    || is_object($this->_modules['usersip']) === false) === true
		   && ($this->_modules['usersip'] = &$this->_service->get_module('usersip')) === false) === true
		|| ((isset($this->_modules['useriax']) === false
		    || is_object($this->_modules['useriax']) === false) === true
		   && ($this->_modules['useriax'] = &$this->_service->get_module('useriax')) === false) === true
		|| ($ref['usersip'] = $this->_modules['usersip']->edit_where($where,$value,
									     null,null,false)) === false
		|| ($ref['useriax'] = $this->_modules['useriax']->edit_where($where,$value,
									     null,null,false)) === false)
			return(false);

		return(true);
	}

	function reverse_edit()
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
				case 'voicemail':
					if(isset($this->_origin['voicemail']) === false
					|| $this->_origin['voicemail'] === false)
						$this->delete_voicemail();
					else
						$this->_voicemail->edit_origin($this->_origin['voicemail']);
					break;
				case 'voicemailfeatures':
					if(isset($this->_origin['voicemailfeatures']) === false
					|| $this->_origin['voicemailfeatures'] === false)
						$this->delete_voicemailfeatures();
					else
						$this->_voicemailfeatures->edit_origin($this->_origin['voicemailfeatures']);
					break;
				case 'contextmember':
					$this->_contextmember->reverse_edit($this->_status);
					break;
				case 'usermailbox':
					if(is_array($val) === false
					|| isset($val['_mailbox'],$val['_newmailbox']) === false)
						continue;

					$this->_update_user_mailbox($val['_newmailbox'],$val['_mailbox']);
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

		if($this->delete_voicemail() === false
		|| $this->delete_voicemailfeatures() === false
		|| $this->delete_contextmember() === false)
		{
			$r = false;
			$this->reverse_delete();
		}
		else
			$this->unlinks();

		$this->_status = '';

		return($r);
	}

	function delete_voicemail()
	{
		if(dwho_issa('voicemail',$this->_info) === false)
			return(false);

		$r = $this->_voicemail->delete($this->_info['voicemail']['uniqueid']);

		if($this->_status === 'delete')
			$this->_return['voicemail'] = $r;

		return($r);
	}

	function delete_voicemailfeatures()
	{
		if(dwho_issa('voicemailfeatures',$this->_info) === false)
			return(false);

		$r = $this->_voicemailfeatures->delete($this->_info['voicemailfeatures']['id']);

		if($this->_status === 'delete')
			$this->_return['voicemailfeatures'] = $r;

		return($r);
	}

	function reverse_delete()
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
				case 'voicemail':
					if(isset($this->_origin['voicemail']) === true
					&& $this->_origin['voicemail'] !== false)
						$this->_voicemail->add_origin($this->_origin['voicemail']);
					break;
				case 'voicemailfeatures':
					if(isset($this->_origin['voicemailfeatures']) === true
					&& $this->_origin['voicemailfeatures'] !== false)
						$this->_voicemailfeatures->add_origin($this->_origin['voicemailfeatures']);
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
		if(dwho_issa('voicemail',$this->_info) === false)
			return(false);
		else if(($dialaction = &$this->_service->get_module('dialaction')) !== false)
		{
			$where = array();
			$where['type'] = 'voicemail';
			$where['typeval'] = $this->_info['voicemail']['uniqueid'];

			$dialaction->unlinked_where($where);
		}

		if(($userfeatures = &$this->_service->get_module('userfeatures')) !== false)
		{
			$where = array();
			$where['voicemailid'] = $this->_info['voicemail']['uniqueid'];

			$userfeatures->edit_where($where,array('voicemailid' => null));
		}

		if((isset($this->_modules['usersip']) === true
		   && is_object($this->_modules['usersip']) === true) === true
		|| ($this->_modules['usersip'] = &$this->_service->get_module('usersip')) !== false)
		{
			$where = array();
			$where['mailbox'] = $this->_info['voicemail']['mailbox'].
					    '@'.$this->_info['voicemail']['context'];

			$this->_modules['usersip']->edit_where($where,array('mailbox' => null));
		}

		if((isset($this->_modules['useriax']) === true
		   && is_object($this->_modules['useriax']) === true) === true
		|| ($this->_modules['useriax'] = &$this->_service->get_module('useriax')) !== false)
		{
			$where = array();
			$where['mailbox'] = $this->_info['voicemail']['mailbox'].
					    '@'.$this->_info['voicemail']['context'];

			$this->_modules['useriax']->edit_where($where,array('mailbox' => null));
		}

		return(true);
	}

	function enable()
	{
		$r = true;
		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_voicemail() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function disable()
	{
		$r = true;
		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_voicemail() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function enable_voicemail()
	{
		return($this->_enable_disable_voicemail(false));
	}

	function disable_voicemail()
	{
		return($this->_enable_disable_voicemail(true));
	}

	function _enable_disable_voicemail($disable=false)
	{
		if(dwho_issa('voicemail',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_voicemail->enable($this->_info['voicemail']['uniqueid']);
		else
			$r = $this->_voicemail->disable($this->_info['voicemail']['uniqueid']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['voicemail'] = $r;

		return($r);
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
