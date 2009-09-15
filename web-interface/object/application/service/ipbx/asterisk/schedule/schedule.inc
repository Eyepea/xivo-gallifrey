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

class xivo_application_service_asterisk_schedule extends xivo_application_asterisk_abstract
{
	var $_schedule		= false;
	var $_dialaction	= false;
	var $_contextmember	= false;
	var $_sound		= array();
	var $_contextlist	= array();

	function xivo_application_service_asterisk_schedule(&$service,&$dso,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if(($this->_schedule = &$this->_service->get_module('schedule')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_dialaction = $this->get_abstract('dialaction',array('category' => 'schedule'))) === false
		|| ($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'schedule'))) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['schedule'] = $this->_schedule->get($id,$disable)) === false)
			return(false);

		$this->_origin['schedule'] = $this->_schedule->get_origin();

		if($this->_components === false)
			return($this->_info);

		$this->_get_dialaction($this->_info['schedule']['id']);
		$this->_get_contextmember($this->_info['schedule']['id']);

		return($this->_info);
	}

	function get_elements()
	{
		$r = array();
		$r['schedule'] = $this->_schedule->get_element();
		$r['dialaction'] = $this->_dialaction->get_element();

		return($r);
	}

	function get_schedules_search($search,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_schedules('search',$search,$disable,$order,$limit,$assoc));
	}

	function get_schedules_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_schedules('list',null,$disable,$order,$limit,$assoc));
	}

	function _get_schedules($action,$search='',$disable=null,$order=null,$limit=null,$assoc=false)
	{
		$search = strval($search);

		switch($action)
		{
			case 'search':
				$rs = $this->_dso->search($search,$disable,$order,$limit);
				break;
			case 'list':
			default:
				$rs = $this->_dso->get_all($disable,$order,$limit);
		}

		$this->_cnt = $this->_dso->get_cnt();

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$r[$rs[$i]['id']] = $rs[$i];

			$ref = &$r[$rs[$i]['id']];

			$schedule = array();
			$schedule['name'] = $ref['name'];
			$schedule['context'] = $ref['context'];
			$schedule['timebeg'] = $ref['timebeg'];
			$schedule['timeend'] = $ref['timeend'];
			$schedule['daynamebeg'] = $ref['daynamebeg'];
			$schedule['daynameend'] = $ref['daynameend'];
			$schedule['daynumbeg'] = $ref['daynumbeg'];
			$schedule['daynumend'] = $ref['daynumend'];
			$schedule['monthbeg'] = $ref['monthbeg'];
			$schedule['monthend'] = $ref['monthend'];
			$schedule['publicholiday'] = $ref['publicholiday'];
			$schedule['commented'] = $ref['commented'];

			$ref = array_merge($ref,$this->_schedule->_prepare($schedule));

			$ref['destination'] = $ref['action'];
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
					$ref['destidentity'] = $this->_schedule->mk_identity($arr);
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
		if(xivo_issa('schedule',$this->_info) === false)
			$id = 0;
		else
			$id = $this->_info['schedule']['id'];

		$r = $this->get_destination_list('schedule',$id);
		$r['sounds'] = $this->_dialaction->get_sound();

		return($r);
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

	function get_result_for_display()
	{
		$r = $this->get_result();

		if(is_array($r) === false)
			$r = array();

		if(xivo_issa('schedule',$r) === false)
			$r['schedule'] = array();

		$timebeg = $this->get_result_var('schedule','timebeg');
		$timeend = $this->get_result_var('schedule','timeend');

		$r['schedule']['timehourbeg'] = '';
		$r['schedule']['timeminbeg'] = '';
		$r['schedule']['timehourend'] = '';
		$r['schedule']['timeminend'] = '';

		if(xivo_has_len($timebeg) === true)
		{
			$timebeg = explode(':',$timebeg);
			$r['schedule']['timehourbeg'] = $timebeg[0];

			if(isset($timebeg[1]) === true)
				$r['schedule']['timeminbeg'] = $timebeg[1];
		}

		if(xivo_has_len($timeend) === true)
		{
			$timeend = explode(':',$timeend);
			$r['schedule']['timehourend'] = $timeend[0];

			if(isset($timeend[1]) === true)
				$r['schedule']['timeminend'] = $timeend[1];
		}

		return($r);
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
		|| xivo_issa('schedule',$arr) === false)
			return(false);

		$this->set_schedule($arr['schedule']);

		if(xivo_issa('dialaction',$arr) === false
		|| $this->set_dialaction($arr['dialaction']) === null
		|| is_array($this->get_result_var('dialaction','inschedule')) === false
		|| is_array($this->get_result_var('dialaction','outschedule')) === false)
			$this->_set_error('dialaction','invalid data');

		$this->set_contextmember();

		return(($this->get_errnb() < 1));
	}

	function set_schedule($arr)
	{
		$r = true;

		if(is_array($arr) === false
		|| isset($arr['timebeg'],$arr['timeend']) === false)
		{
			$this->_set_result('schedule',null);
			$this->_set_error('schedule','invalid data');
			return(false);
		}
		else if(($time = $this->_schedule->mk_time($arr['timebeg'],$arr['timeend'])) === false)
		{
			$r = false;
			$this->_set_error('schedule','missing time');
		}
		else
		{
			$arr['timebeg'] = $time['beg'];
			$arr['timeend'] = $time['end'];
		}

		$arr['commented'] = false;

		if(($rs = $this->_schedule->chk_values($arr)) === false)
		{
			$r = false;
			$rs = $this->_schedule->get_filter_result();
			$this->_set_error('schedule',$this->_schedule->get_filter_error());
		}

		if(($date = $this->_schedule->chk_date($rs)) !== false)
			$rs = array_merge($rs,$date);
		else
		{
			$r = false;
			$this->_set_error('schedule','invalid date');
		}

		$this->_set_result('schedule',$rs);

		return($r);
	}

	function set_contextmember()
	{
		$rs = array();
		$rs['schedule-context']['context'] = (string) $this->get_result_var('schedule','context');
		$rs['schedule-context']['typeval'] = 0;
		$rs['schedule-context']['varname'] = 'context';

		return(parent::set_contextmember($rs));
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$scheduleid = null;

		if($this->get_errnb() > 0
		|| ($scheduleid = $this->add_schedule()) === false
		|| $this->add_dialaction($scheduleid) === false
		|| $this->add_contextmember($scheduleid) === false)
		{
			$r = false;

			if($scheduleid !== null)
				$this->_reverse_add($scheduleid);
		}

		$this->_status = '';

		return($r);
	}

	function add_schedule()
	{
		if(($rs = $this->get_result('schedule')) === null
		|| $rs === false)
			return(false);

		return(($this->_return['schedule'] = $this->_schedule->add($rs)));
	}

	function _reverse_add($scheduleid)
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'add'
		|| ($scheduleid = xivo_ulongint($scheduleid)) === 0
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'schedule':
					$this->_schedule->delete($scheduleid);
					break;
				case 'dialaction':
					$this->_dialaction->reverse_add();
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
		|| $this->edit_schedule() === false
		|| $this->edit_dialaction() === false
		|| $this->edit_contextmember() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_schedule()
	{
		if(xivo_issa('schedule',$this->_info) === false
		|| ($rs = $this->get_result('schedule')) === null
		|| $rs === false)
			return(false);

		$rs['commented'] = $this->_info['schedule']['commented'];

		return(($this->_return['schedule'] = $this->_schedule->edit($this->_info['schedule']['id'],$rs)));
	}

	function edit_dialaction()
	{
		if(xivo_issa('schedule',$this->_info) === false)
			return(false);

		return(parent::edit_dialaction($this->_info['schedule']['id']));
	}

	function edit_contextmember()
	{
		if(xivo_issa('schedule',$this->_info) === false)
			return(false);

		return(parent::edit_contextmember($this->_info['schedule']['id']));
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
				case 'schedule':
					if(isset($this->_origin['schedule']) === false
					|| $this->_origin['schedule'] === false)
						$this->delete_schedule();
					else
						$this->_schedule->edit_origin($this->_origin['schedule']);
					break;
				case 'dialaction':
					$this->_dialaction->reverse_edit($this->_status);
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

		if($this->delete_schedule() === false
		|| $this->delete_dialaction() === false
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

	function delete_schedule()
	{
		if(xivo_issa('schedule',$this->_info) === false)
			return(false);

		$r = $this->_schedule->delete($this->_info['schedule']['id']);

		if($this->_status === 'delete')
			$this->_return['schedule'] = $r;

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
				case 'schedule':
					if(isset($this->_origin['schedule']) === true
					&& $this->_origin['schedule'] !== false)
						$this->_schedule->add_origin($this->_origin['schedule']);
					break;
				case 'dialaction':
					$this->_dialaction->add_origin_list();
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
		if(xivo_issa('schedule',$this->_info) === false)
			return(false);

		$this->_dialaction->unlinked('schedule',$this->_info['schedule']['id']);

		return(true);
	}

	function enable()
	{
		$r = true;
		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_schedule() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function disable()
	{
		$r = true;
		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_schedule() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function enable_schedule()
	{
		return($this->_enable_disable_schedule(false));
	}

	function disable_schedule()
	{
		return($this->_enable_disable_schedule(true));
	}

	function _enable_disable_schedule($disable=false)
	{
		if(xivo_issa('schedule',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_schedule->enable($this->_info['schedule']['id']);
		else
			$r = $this->_schedule->disable($this->_info['schedule']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['schedule'] = $r;

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
}

?>
