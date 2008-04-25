<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract.inc'),true);

class xivo_application_service_asterisk_rightcall_abstract extends xivo_application_service_asterisk_abstract
{
	var $_rightcall		= false;
	var $_rightcallmember	= false;
	var $_param		= array('type'	=> '');

	function xivo_application_service_asterisk_rightcall_abstract(&$service,$param=array())
	{
		$this->xivo_application_service_asterisk_abstract(&$service);
		
		if(($this->_rightcall = &$this->_service->get_module('rightcall')) === false
		|| ($this->_rightcallmember = &$this->_service->get_module('rightcallmember')) === false)
			return(false);

		return($this->load_init($param));
	}

	function load_init($param=array())
	{
		$this->_reset_info();
		$this->_reset_params();
		$this->_reset();

		if($this->set_params($param) === false
		|| isset($param['type']) === false
		|| $this->_rightcallmember->chk_element_value('type',$param['type']) === false)
		{
			$this->_param['type'] = '';
			return(false);
		}

		$this->_param['type'] = $param['type'];

		return(true);
	}

	function get($typeval)
	{
		$this->_info = false;
		$this->_origin = false;

		if(($typeval = xivo_ulongint($typeval)) === 0)
			return(false);

		$where = array();
		$where['type'] = $this->_param['type'];
		$where['typeval'] = $typeval;

		if(($this->_info = $this->_rightcallmember->get_all_where($where)) === false
		|| isset($this->_info[0]) === false)
			return(false);

		$this->_origin = $this->_rightcallmember->get_origin_list();

		return($this->_info);
	}

	function set($list)
	{
		$this->_reset();

		if(is_array($list) === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === 0)
			return(null);

		$info = array();
		$info['rightcallid'] = 0;
		$info['type'] = $this->_param['type'];
		$info['typeval'] = 0;
		
		$tmp = $reslist = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($rightcallid = $this->_rightcall->get_primary($list[$i])) === false)
			{
				$this->_set_error('wrong rightcall');
				continue;
			}

			$info['rightcallid'] = $rightcallid;

			if(($rs = $this->_rightcallmember->chk_values($info)) === false)
			{
				$this->_set_error('wrong rightcall');
				continue;
			}
			else if(isset($tmp[$rightcallid]) === true)
				continue;

			$tmp[$rightcallid] = 1;
			$reslist[] = $rs;
		}

		if($this->get_errnb() > 0 || isset($reslist[0]) === false)
		{
			$this->_set_result(false);
			return(false);
		}

		$this->_set_result($reslist);

		return(true);
	}

	function add($typeval)
	{
		if(($typeval = xivo_ulongint($typeval)) === 0)
			return(false);
		else if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);

		$this->_return = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['typeval'] = $typeval;

			if(($this->_return[] = $this->_rightcallmember->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return[0]) === false)
			$this->_return = false;

		return(true);
	}

	function add_origin_list()
	{
		return($this->_rightcallmember->add_origin_list($this->_origin));
	}

	function edit($status,$typeval)
	{
		if(($typeval = xivo_ulongint($typeval)) === 0)
			return(false);
		else if(xivo_issa(0,$this->_origin) === false)
			return($this->add($typeval));

		$this->_return = $this->delete($status);

		if(($rs = $this->get_result()) === null)
			return($this->_return);
		else if($rs === false)
			return(false);

		$this->_return = array();

		$nb = count($rs);

		$typeval = $this->_origin[0]['typeval'];

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($this->_origin[$i]) === true)
				$rid = $this->_origin[$i]['id'];
			else
				$rid = 0;

			$rs[$i]['typeval'] = $typeval;

			if(($this->_return[] = $this->_rightcallmember->add($rs[$i],$rid)) === false)
				return(false);
		}

		if(isset($this->_return[0]) === false)
			$this->_return = false;

		return(true);
	}

	function delete($status)
	{
		if(xivo_issa(0,$this->_origin) === false)
			return(null);

		$rightcallmember_where = array();
		$rightcallmember_where['type'] = $this->_origin[0]['type'];
		$rightcallmember_where['typeval'] = $this->_origin[0]['typeval'];

		$r = $this->_rightcallmember->delete_where($rightcallmember_where);

		if($status === 'delete')
			$this->_return = $r;

		return($r);
	}

	function reverse_add()
	{
		if(is_array($this->_return) === false || ($nb = count($this->_return)) === 0)
			return(null);

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_return[$i] !== false)
				$this->_rightcallmember->delete($this->_return[$i]);
		}

		return(true);
	}

	function reverse_edit($status)
	{
		$this->delete($status);

		return($this->add_origin_list());
	}
}

?>
