<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract.inc'),true);

class xivo_application_service_asterisk_extenumbers_abstract extends xivo_application_service_asterisk_abstract
{
	var $_extenumbers	= false;

	function xivo_application_service_asterisk_extenumbers_abstract(&$service)
	{
		$this->xivo_application_service_asterisk_abstract(&$service);
		
		if(($this->_extenumbers = &$this->_service->get_module('extenumbers')) === false)
			return(false);

		return(true);
	}

	function get($type,$typeval)
	{
		$this->_info = false;
		$this->_origin = false;

		if(is_scalar($type) === false || is_scalar($typeval) === false)
			return(false);

		$extenumbers_where = array();
		$extenumbers_where['type'] = $type;
		$extenumbers_where['typeval'] = $typeval;

		$this->_info = $this->_extenumbers->get_where($extenumbers_where);
		$this->_origin = $this->_extenumbers->get_origin();

		return($this->_info);
	}

	function get_element()
	{
		return($this->_extenumbers->get_element());
	}

	function set($action,$type,$number,$context='',$typeval='')
	{
		$this->_reset();

		if($action !== 'add' && $action !== 'edit' && $action !== 'save')
			return(false);

		$extenum = array();
		$extenum['exten'] = $number;
		$extenum['context'] = $context;
		$extenum['type'] = $type;

		if(empty($this->_info) === false
		&& ($action === 'edit' || $action === 'save') === true)
			$extenum['typeval'] = $this->_info['typeval'];
		else
			$extenum['typeval'] = '';

		if($extenum['typeval'] === '' && $typeval !== '')
			$extenum['typeval'] = $typeval;

		$exists = false;

		if(($rs = $this->_extenumbers->chk_values($extenum)) === false
		|| ($exists = $this->_extenumbers->exists_type($rs)) !== false)
		{
			$this->_set_result($this->_extenumbers->get_filter_result());

			if($exists === false)
				$this->_set_error($this->_extenumbers->get_filter_error());
			else
				$this->_set_error('exists');

			return(false);
		}

		$this->_set_result($rs);

		return(true);
	}

	function add($typeval='',$id=0)
	{
		if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);

		if(xivo_haslen($typeval) === true)
			$rs['typeval'] = $typeval;

		$this->_return = $this->_extenumbers->add($rs,$id);

		return($this->_return);
	}

	function edit($id=0)
	{
		if(empty($this->_info) === false)
			$id = $this->_info['id'];
		else if(($id = xivo_ulongint($id)) === 0)
			return(false);

		if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);
		else if(($this->_return = $this->_extenumbers->edit($id,$rs)) !== false)
			$this->_return = $id;

		return($this->_return);
	}

	function save($status,$typeval=null)
	{
		if($status !== 'save')
			return(false);
		else if(empty($this->_info) === true)
			return($this->add($typeval));

		return($this->edit($typeval));
	}

	function delete($status,$id=0)
	{
		if(($id = xivo_ulongint($id)) === 0)
		{
			if(empty($this->_info) === true)
				return(null);
			else
				$id = $this->_info['id'];
		}

		$r = $this->_extenumbers->delete($id);

		if($status === 'delete')
			$this->_return = $r;

		return($r);
	}

	function add_origin($origin=null)
	{
		if(empty($this->_origin) === false
		&& $origin === null)
			$origin = $this->_origin;
		else if(is_array($origin) === false)
			return(false);
			
		return($this->add_origin($origin));
	}

	function edit_origin($origin=null)
	{
		if(empty($this->_origin) === false
		&& $origin === null)
			$origin = $this->_origin;
		else if(is_array($origin) === false)
			return(false);
			
		return($this->edit_origin($origin));
	}
}

?>
