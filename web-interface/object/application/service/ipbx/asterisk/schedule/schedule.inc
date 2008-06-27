<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_schedule extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_schedule		= false;
	var $_contextmember	= false;
	var $_sound		= array();
	var $_contextlist	= array();

	function xivo_application_service_asterisk_schedule(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_schedule = &$this->_service->get_module('schedule')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'schedule'))) === false)
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

		$this->_get_contextmember($this->_info['schedule']['id']);

		return($this->_info);
	}

	function get_elements()
	{
		$r = array();
		$r['schedule'] = $this->_schedule->get_element();

		return($r);
	}

	function get_schedules_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		if(($rs = $this->_schedule->get_all($disable,true,$order,$limit,$assoc)) === false
		|| empty($rs) === true)
		{
			$this->_cnt = $this->_schedule->get_cnt();
			return(false);
		}

		$this->_cnt = $this->_schedule->get_cnt();

		$r = $info = array();

		foreach($rs as $key => $val)
		{
			$info['schedule'] = $val;
			$info['typetrue'] = $info['typefalse'] = false;

			if($info['schedule']['linked'] === true
			&& (($info['typetrue'] = $this->_schedule->is_valid($val['typetrue'],
								     $val['typevaltrue'],
								     $val['applicationvaltrue'],
								     $val['id'])) === false
			|| ($info['typefalse'] = $this->_schedule->is_valid($val['typefalse'],
								     $val['typevalfalse'],
								     $val['applicationvalfalse'],
								     $val['id'])) === false) === true)
				$info['schedule']['linked'] = false;

			if($info['typetrue'] === true)
				$info['typetrue'] = false;

			if($info['typefalse'] === true)
				$info['typefalse'] = false;

			$r[$key] = $info;
		}

		if(empty($r) === true)
			return(false);

		return($r);
	}

	function get_destination_list()
	{
		if(xivo_issa('schedule',$this->_info) === false)
			$id = 0;
		else
			$id = $this->_info['schedule']['id'];

		$r = parent::get_destination_list('schedule',$id);
		$r['sounds'] = $this->get_sound();

		return($r);
	}

	function get_sound()
	{
		if(empty($this->_sound) === false)
			return($this->_sound);
		else if(($sounds = &$this->_service->get_module('sounds')) === false)
			return(false);

		return(($this->_sound = $sounds->get_list('playback','pathnoext')));
	}

	function get_result_for_display()
	{
		$r = $this->get_result();

		if(is_array($r) === false)
			$r = array();

		if(xivo_issa('schedule',$r) === false)
			$r['schedule'] = array();

		$r['schedule']['endcall'] = null;
		$r['schedule']['user'] = null;
		$r['schedule']['group'] = null;
		$r['schedule']['queue'] = null;
		$r['schedule']['meetme'] = null;
		$r['schedule']['voicemail'] = null;
		$r['schedule']['schedule'] = null;
		$r['schedule']['application'] = null;
		$r['schedule']['sound'] = null;
		$r['schedule']['custom'] = null;

		if(xivo_haslen($r['schedule'],'typetrue') === true
		&& array_key_exists($r['schedule']['typetrue'],$r['schedule']) === true)
		{
			$typetrue = &$r['schedule']['typetrue'];

			$r['schedule'][$typetrue] = array('false' => '');

			if(isset($r['schedule']['typevaltrue']) === true)
				$r['schedule'][$typetrue]['true'] = $r['schedule']['typevaltrue'];
			else
				$r['schedule'][$typetrue]['true'] = '';
		}

		if(xivo_haslen($r['schedule'],'typefalse') === true
		&& array_key_exists($r['schedule']['typefalse'],$r['schedule']) === true)
		{
			$typefalse = &$r['schedule']['typefalse'];

			$r['schedule'][$typefalse] = array('true' => '');

			if(isset($r['schedule']['typevalfalse']) === true)
				$r['schedule'][$typefalse]['false'] = $r['schedule']['typevalfalse'];
			else
				$r['schedule'][$typefalse]['false'] = '';
		}

		$timebeg = $this->get_result_var('schedule','timebeg');
		$timeend = $this->get_result_var('schedule','timeend');

		$r['schedule']['timehourbeg'] = '';
		$r['schedule']['timeminbeg'] = '';
		$r['schedule']['timehourend'] = '';
		$r['schedule']['timeminend'] = '';

		if(xivo_haslen($timebeg) === true)
		{
			$timebeg = explode(':',$timebeg);
			$r['schedule']['timehourbeg'] = $timebeg[0];

			if(isset($timebeg[1]) === true)
				$r['schedule']['timeminbeg'] = $timebeg[1];
		}

		if(xivo_haslen($timeend) === true)
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

		$this->set_schedule($action,$arr['schedule']);

		$this->set_contextmember();

		return(($this->get_errnb() < 1));
	}

	function set_schedule($action,$arr)
	{
		$r = true;

		if(($action !== 'add' && $action !== 'edit') === true
		|| is_array($arr) === false
		|| isset($arr['timebeg'],$arr['timeend']) === false
		|| ($action === 'edit' && xivo_issa('schedule',$this->_info) === false) === true)
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

		$arr['linked'] = true;
		$arr['commented'] = false;

		if($action === 'edit')
			$validid = $this->_info['schedule']['id'];
		else
			$validid = null;

		$validtrue = $validfalse = null;

		if(($rs = $this->_schedule->chk_values($arr)) === false
		|| ($validtrue = $this->_schedule->is_valid($rs['typetrue'],
							    $rs['typevaltrue'],
							    $rs['applicationvaltrue'],
							    $validid)) === false
		|| ($validfalse = $this->_schedule->is_valid($rs['typefalse'],
							     $rs['typevalfalse'],
							     $rs['applicationvalfalse'],
							     $validid)) === false)
		{
			$r = false;

			if($rs === false)
			{
				$rs = $this->_schedule->get_filter_result();
				$this->_set_error('schedule',$this->_schedule->get_filter_error());
			}
			else if($validtrue === false)
				$this->_set_error('schedule','invalid true destination');
			else
				$this->_set_error('schedule','invalid false destination');
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

		$schedule_where = array();
		$schedule_where['typetrue'] = 'schedule';
		$schedule_where['typevaltrue'] = $this->_info['schedule']['id'];

		$this->_schedule->unlinked_where($schedule_where);

		$schedule_where = array();
		$schedule_where['typefalse'] = 'schedule';
		$schedule_where['typevalfalse'] = $this->_info['schedule']['id'];

		$this->_schedule->unlinked_where($schedule_where);

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
}

?>
