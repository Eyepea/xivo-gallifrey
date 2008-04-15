<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract.inc'),true);

class xivo_application_service_asterisk_dialstatus_abstract extends xivo_application_service_asterisk_abstract
{
	var $_dialstatus	= false;
	var $_param		= array('category'	=> '');

	function xivo_application_service_asterisk_dialstatus_abstract(&$service,$param=array())
	{
		$this->xivo_application_service_asterisk_abstract(&$service);
		
		if(($this->_dialstatus = &$this->_service->get_module('dialstatus')) === false)
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
		|| $this->_dialstatus->chk_element_value('category',$param['category']) === false)
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

		if(($info = $this->_dialstatus->get_all_where($where)) === false
		|| ($nb = count($info)) === 0)
			return(false);

		$this->_info = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($this->_info[$info[$i]['status']]) === true)
				continue;

			$this->_info[$info[$i]['status']] = $info[$i];
		}

		$this->_origin = $this->_dialstatus->get_origin_list();

		return($this->_info);
	}

	function get_sound()
	{
		if(($sounds = &$this->_service->get_module('sounds')) === false)
			return(false);

		return($sounds->get_list('playback',true));
	}

	function get_element()
	{
		return($this->_dialstatus->get_element());
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
		$info['applicationval'] = '';
		$info['linked'] = true;

		$reslist = array();

		foreach($list as $key => $val)
		{
			if(is_array($val) === false)
				continue;

			$infotmp = array_merge($info,$val);
			$infotmp['status'] = $key;

			if(($rs = $this->_dialstatus->chk_values($infotmp)) !== false
			&& isset($reslist[$rs['status']]) === false)
				$reslist[$rs['status']] = $rs;
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

		$r['noanswer'] = $this->_mk_result_for_display('noanswer');
		$r['busy'] = $this->_mk_result_for_display('busy');
		$r['congestion'] = $this->_mk_result_for_display('congestion');
		$r['chanunavail'] = $this->_mk_result_for_display('chanunavail');

		return($r);
	}

	function _mk_result_for_display($status)
	{
		$r = array();
		$r['endcall'] = '';
		$r['user'] = '';
		$r['group'] = '';
		$r['queue'] = '';
		$r['meetme'] = '';
		$r['schedule'] = '';
		$r['application'] = '';
		$r['sound'] = '';
		$r['custom'] = ''; 

		if(xivo_issa($status,$this->_result) === true)
		{
			$r[$this->_result[$status]['type']] = $this->_result[$status]['typeval'];
			$r = array_merge($r,$this->_result[$status]);
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

			if($this->_dialstatus->is_valid($val['category'],
							$val['categoryval'],
							$val['type'],
							$val['typeval'],
							$val['applicationval']) === false
			|| ($this->_return[$val['status']] = $this->_dialstatus->add($val)) === false)
				return(false);
		}

		if(empty($this->_return) === true)
			$this->_return = false;

		return(true);
	}

	function add_origin_list()
	{
		return($this->_dialstatus->add_origin_list($this->_origin));
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

		$i = 0;

		$categoryval = $this->_origin[0]['categoryval'];

		foreach($rs as $val)
		{
			if($list !== false && isset($list[$i]) === true)
				$did = $list[$i++]['id'];
			else
				$did = 0;

			$val['categoryval'] = $categoryval;

			if($this->_dialstatus->is_valid($val['category'],
							$val['categoryval'],
							$val['type'],
							$val['typeval'],
							$val['applicationval']) === false
			|| ($this->_return[$val['status']] = $this->_dialstatus->add($val,$did)) === false)
				return(false);
		}

		if(empty($this->_return) === true)
			$this->_return = false;

		return(true);
	}

	function delete($status)
	{
		if(xivo_issa(0,$this->_origin) === false)
			return(null);

		$dialstatus_where = array();
		$dialstatus_where['category'] = $this->_origin[0]['category'];
		$dialstatus_where['categoryval'] = $this->_origin[0]['categoryval'];

		$r = $this->_dialstatus->delete_where($dialstatus_where);

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
				$this->_dialstatus->delete($val);
		}

		return(true);
	}

	function reverse_edit($status)
	{
		$this->delete($status);

		return($this->add_origin_list());
	}

	function unlinked($type,$typeval)
	{
		if($this->_dialstatus->chk_element_value('type',$type) === false
		|| ($typeval = xivo_ulongint($typeval)) === 0)
			return(false);

		$dialstatus_where = array();
		$dialstatus_where['type'] = $type;
		$dialstatus_where['typeval'] = $typeval;

		return($this->_dialstatus->unlinked_where($dialstatus_where));
	}
}

?>
