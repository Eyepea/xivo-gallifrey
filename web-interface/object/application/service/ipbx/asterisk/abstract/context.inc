<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract.inc'),true);

class xivo_application_service_asterisk_context_abstract extends xivo_application_service_asterisk_abstract
{
	var $_context		= false;
	var $_contextnumbers	= false;
	var $_contextmember	= false;
	var $_param		= array('type'	=> '');

	function xivo_application_service_asterisk_context_abstract(&$service,$param=array())
	{
		$this->xivo_application_service_asterisk_abstract(&$service);
		
		if(($this->_context = &$this->_service->get_module('context')) === false
		|| ($this->_contextnumbers = &$this->_service->get_module('contextnumbers')) === false
		|| ($this->_contextmember = &$this->_service->get_module('contextmember')) === false)
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
		|| $this->_contextmember->chk_element_value('type',$param['type']) === false)
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

		if(xivo_haslen($typeval) === false)
			return(false);

		$where = array();
		$where['type'] = $this->_param['type'];
		$where['typeval'] = $typeval;

		if(($this->_info = $this->_contextmember->get_where($where)) === false)
			return(false);

		$this->_origin = $this->_contextmember->get_origin();

		return($this->_info);
	}

	function set($context,$number='')
	{
		$this->_reset();

		$info = array();
		$info['context'] = $context;
		$info['type'] = $this->_param['type'];
		$info['typeval'] = 0;

		if(xivo_haslen($number) === false)
		{
			$chkerror = 'invalid context';

			$where = array();
			$where['name'] = $context;
			$where['entity'] = null;

			$chk = $this->_context->get_where($where);
		}
		else
		{
			$chkerror = 'out of interval';

			$chk = $this->_contextnumbers->interval_number_exists($context,
									     $this->_param['type'],
									     $number);
		}

		if($chk === false)
		{
			$this->_set_result($info);
			$this->_set_error($chkerror);
			return(false);
		}
		else if(($rs = $this->_contextmember->chk_values($info)) === false)
		{
			$this->_set_result($this->_contextmember->get_filter_result());
			$this->_set_error($this->_contextmember->get_filter_error());
			return(false);
		}

		$this->_set_result($rs);

		return(true);
	}

	function add($typeval)
	{
		if(xivo_haslen($typeval) === false)
			return(false);
		else if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);

		$rs['typeval'] = $typeval;

		if($this->_contextmember->add($rs) === false)
			return(($this->_return = false));

		$this->_return = array('type' => $rs['type'],'typeval' => $rs['typeval']);

		return(true);
	}

	function edit($status,$typeval)
	{
		if(xivo_haslen($typeval) === false)
			return(false);
		else if(empty($this->_origin) === true)
			return($this->add($typeval));

		$this->_return = $this->delete($status);
			
		if(($rs = $this->get_result()) === null)
			return($this->_return);
		else if($rs === false)
			return(false);

		$rs['typeval'] = $this->_origin['typeval'];

		if($this->_contextmember->add($rs) === false)
			return(($this->_return = false));

		$this->_return = array('type' => $rs['type'],'typeval' => $rs['typeval']);

		return(true);
	}

	function delete($status)
	{
		if(empty($this->_origin) === true)
			return(null);

		$contextmember_where = array();
		$contextmember_where['type'] = $this->_origin['type'];
		$contextmember_where['typeval'] = $this->_origin['typeval'];

		$r = $this->_contextmember->delete_where($contextmember_where);

		if($status === 'delete')
			$this->_return = $r;

		return($r);
	}

	function reverse_add()
	{
		if(is_array($this->_return) === false
		|| isset($this->_return['type'],$this->_return['typeval']) === false)
			return(null);

		$contextmember_where = array();
		$contextmember_where['type'] = $this->_return['type'];
		$contextmember_where['typeval'] = $this->_return['typeval'];

		$this->_contextmember->delete_where($contextmember_where);

		return(true);
	}

	function reverse_edit($status)
	{
		$this->delete($status);

		return($this->_contextmember->add_origin($this->_origin));
	}
}

?>
