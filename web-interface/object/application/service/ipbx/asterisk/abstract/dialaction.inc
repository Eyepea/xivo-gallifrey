<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract.inc'),true);

class xivo_application_service_asterisk_dialaction_abstract extends xivo_application_service_asterisk_abstract
{
	var $_dialaction	= false;
	var $_param		= array('category'	=> '');

	function xivo_application_service_asterisk_dialaction_abstract(&$service,$param=array())
	{
		$this->xivo_application_service_asterisk_abstract(&$service);
		
		if(($this->_dialaction = &$this->_service->get_module('dialaction')) === false)
			return(false);

		return($this->load_init($param));
	}

	function load_init($param=array())
	{
		$this->_reset_info();
		$this->_reset_params();
		$this->_reset();

		if($this->set_params($param) === false
		|| isset($param['category']) === false
		|| $this->_dialaction->chk_element_value('category',$param['category']) === false)
		{
			$this->_param['category'] = '';
			return(false);
		}

		$this->_param['category'] = $param['category'];

		return(true);
	}

	function get($categoryval)
	{
		$this->_info = false;
		$this->_origin = false;

		if(($categoryval = xivo_ulongint($categoryval)) === 0)
			return(false);

		$where = array();
		$where['category'] = $this->_param['category'];
		$where['categoryval'] = $categoryval;

		if(($info = $this->_dialaction->get_all_where($where)) === false
		|| ($nb = count($info)) === 0)
			return(false);

		$this->_info = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($this->_info[$info[$i]['event']]) === true)
				continue;

			$this->_info[$info[$i]['event']] = $info[$i];
		}

		$this->_origin = $this->_dialaction->get_origin_list();

		return($this->_info);
	}

	function get_sound()
	{
		if(($sounds = &$this->_service->get_module('sounds')) === false)
			return(false);

		return($sounds->get_list('playback','pathnoext'));
	}

	function get_element()
	{
		return($this->_dialaction->get_element());
	}

	function set($list)
	{
		$this->_reset();

		if(is_array($list) === false)
			return(false);
		else if(empty($list) === true)
			return(null);

		$info = $infotmp = array();
		$info['category'] = $this->_param['category'];
		$info['categoryval'] = 0;
		$info['action'] = '';
		$info['actiontype'] = '';
		$info['actionarg1'] = '';
		$info['actionarg2'] = '';
		$info['linked'] = true;

		$reslist = array();

		foreach($list as $key => $val)
		{
			if(is_array($val) === false)
				continue;

			$infotmp = array_merge($info,$val);
			$infotmp['event'] = $key;

			if($infotmp['actiontype'] === 'endcall' || $infotmp['actiontype'] === 'application')
				$infotmp['action'] = $infotmp['actiontype'].':'.$infotmp['action'];
			else
				$infotmp['action'] = $infotmp['actiontype'];

			if($infotmp['actiontype'] === 'sound' && xivo_issa('actionarg2',$infotmp) === true)
				$infotmp['actionarg2'] = implode('',$infotmp['actionarg2']);

			if(($rs = $this->_dialaction->chk_values($infotmp)) !== false
			&& isset($reslist[$rs['event']]) === false)
				$reslist[$rs['event']] = $rs;
		}

		if(empty($reslist) === true)
		{
			$this->_set_result(false);
			return(false);
		}

		$this->_set_result($reslist);

		return(true);
	}

	function get_result_for_display()
	{
		$r = array();

		if(($categoryevent = $this->_dialaction->get_categoryevent_value($this->_param['category'])) === false)
			return(null);

		$nb = count($categoryevent);

		for($i = 0;$i < $nb;$i++)
			$r[$categoryevent[$i]] = $this->_mk_result_for_display($categoryevent[$i]);

		return($r);
	}

	function _mk_result_for_display($event)
	{
		$arr = array();
		$arr['action'] = '';
		$arr['actionarg1'] = '';
		$arr['actionarg2'] = '';

		$r = array();
		$r['none'] = $arr;
		$r['endcall'] = $arr;
		$r['user'] = $arr;
		$r['group'] = $arr;
		$r['queue'] = $arr;
		$r['meetme'] = $arr;
		$r['voicemail'] = $arr;
		$r['schedule'] = $arr;
		$r['voicemenu'] = $arr;
		$r['application'] = $arr;
		$r['sound'] = $arr;
		$r['custom'] = $arr;
		$r['linked'] = false;

		if(xivo_issa($event,$this->_result) === false)
			return($r);

		if(isset($this->_result[$event]['linked']) === true)
			$r['linked'] = (bool) $this->_result[$event]['linked'];

		$exp = explode(':',$this->_result[$event]['action'],2);

		if(isset($exp[1]) === false)
			$r[$exp[0]]['action'] = $exp[0];
		else
		{
			$r[$exp[0]]['action'] = $exp[1];
			$r[$exp[1]]['actionarg1'] = $this->_result[$event]['actionarg1'];
			$r[$exp[1]]['actionarg2'] = $this->_result[$event]['actionarg2'];
		}

		$r['action'] = $exp[0];
		$r[$exp[0]]['actionarg1'] = $this->_result[$event]['actionarg1'];
		$r[$exp[0]]['actionarg2'] = $this->_result[$event]['actionarg2'];

		if($exp[0] === 'sound')
		{
			$r['sound']['actionarg2'] = array();

			if(strpos($this->_result[$event]['actionarg2'],'skip') !== false)
				$r['sound']['actionarg2']['skip'] = true;

			if(strpos($this->_result[$event]['actionarg2'],'noanswer') !== false)
				$r['sound']['actionarg2']['noanswer'] = true;

			if(strpos($this->_result[$event]['actionarg2'],'j') !== false)
				$r['sound']['actionarg2']['j'] = true;
		}

		return($r);
	}

	function add($categoryval)
	{
		if(($categoryval = xivo_ulongint($categoryval)) === 0)
			return(false);
		else if(($rs = $this->get_result()) === null)
			return($rs);
		else if(is_array($rs) === false || empty($rs) === true)
			return(false);

		$this->_return = array();

		foreach($rs as $val)
		{
			$val['categoryval'] = $categoryval;

			if($this->_dialaction->is_valid($val['event'],
							$val['category'],
							$val['categoryval'],
							$val['action'],
							$val['actionarg1'],
							$val['actionarg2']) === false
			|| $this->_dialaction->add($val) === false)
				return(($this->_return[$val['event']] = false));

			$this->_return[$val['event']] = array();
			$this->_return[$val['event']]['event'] = $val['event'];
			$this->_return[$val['event']]['category'] = $val['category'];
			$this->_return[$val['event']]['categoryval'] = $val['categoryval'];
		}

		if(empty($this->_return) === true)
			$this->_return = false;

		return(true);
	}

	function add_origin_list()
	{
		return($this->_dialaction->add_origin_list($this->_origin));
	}

	function edit($status,$categoryval)
	{
		if(($categoryval = xivo_ulongint($categoryval)) === 0)
			return(false);
		else if(xivo_issa(0,$this->_origin) === false)
			return($this->add($categoryval));

		$this->_return = $this->delete($status);

		if(($rs = $this->get_result()) === null)
			return($this->_return);
		else if(is_array($rs) === false || empty($rs) === true)
			return(false);

		$this->_return = array();

		$list = is_array($this->_origin) === true ? $this->_origin : false;

		$categoryval = $this->_origin[0]['categoryval'];

		foreach($rs as $val)
		{
			$val['categoryval'] = $categoryval;

			if($this->_dialaction->is_valid($val['event'],
							$val['category'],
							$val['categoryval'],
							$val['action'],
							$val['actionarg1'],
							$val['actionarg2']) === false
			|| $this->_dialaction->add($val) === false)
				return(($this->_return[$val['event']] = false));

			$this->_return[$val['event']] = array();
			$this->_return[$val['event']]['event'] = $val['event'];
			$this->_return[$val['event']]['category'] = $val['category'];
			$this->_return[$val['event']]['categoryval'] = $val['categoryval'];
		}

		if(empty($this->_return) === true)
			$this->_return = false;

		return(true);
	}

	function delete($status)
	{
		if(xivo_issa(0,$this->_origin) === false)
			return(null);

		$dialaction_where = array();
		$dialaction_where['category'] = $this->_origin[0]['category'];
		$dialaction_where['categoryval'] = $this->_origin[0]['categoryval'];

		$r = $this->_dialaction->delete_where($dialaction_where);

		if($status === 'delete')
			$this->_return = $r;

		return($r);
	}

	function reverse_add()
	{
		if(is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $val)
		{
			if($val !== false)
				$this->_dialaction->delete($val);
		}

		return(true);
	}

	function reverse_edit($status)
	{
		$this->delete($status);

		return($this->add_origin_list());
	}

	function unlinked($action,$actionarg1)
	{
		if($this->_dialaction->chk_element_value('action',$action) === false
		|| ($actionarg1 = xivo_ulongint($actionarg1)) === 0)
			return(false);

		$dialaction_where = array();
		$dialaction_where['action'] = $action;
		$dialaction_where['actionarg1'] = $actionarg1;

		return($this->_dialaction->unlinked_where($dialaction_where));
	}
}

?>
