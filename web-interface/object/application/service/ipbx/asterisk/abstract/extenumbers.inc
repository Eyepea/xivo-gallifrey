<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract','abstract.inc'),true);

class xivo_application_service_asterisk_extenumbers_abstract extends xivo_application_service_asterisk_abstract
{
	function get($type,$typeval)
	{
		$this->_info = false;
		$this->_origin = false;

		if(is_scalar($type) === false || is_scalar($typeval) === false)
			return(false);

		if(($extenumbers = &$this->_service->get_module('extenumbers')) === false)
			return(false);

		$extenumbers_where = array();
		$extenumbers_where['type'] = $type;
		$extenumbers_where['typeval'] = $typeval;

		$this->_info = $extenumbers->get_where($extenumbers_where);
		$this->_origin = $extenumbers->get_origin();

		return($this->_info);
	}

	function set($action,$type,$number,$context,$typeval='')
	{
		if(($action !== 'add' && $action !== 'edit' && $action !== 'save') === true
		|| ($extenumbers = &$this->_service->get_module('extenumbers')) === false)
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

		if(($rs = $extenumbers->chk_values($extenum)) === false
		|| ($exists = $extenumbers->exists_type($rs)) !== false)
		{
			$this->_set_result($extenumbers->get_filter_result());

			if($exists === false)
				$this->_set_error($extenumbers->get_filter_error());
			else
				$this->_set_error('exists');

			return(false);
		}

		$rs = $this->_set_result($rs);

		return(true);
	}

	function add($typeval='')
	{
		if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);

		if(xivo_haslen($typeval) === true)
			$rs['typeval'] = $typeval;

		$extenumbers = &$this->_service->get_module('extenumbers');
		$this->_return = $extenumbers->add($rs);

		return($this->_return);
	}

	function edit($id=0)
	{
		if(empty($this->_info) === false)
			$id = $this->_info['id'];
		else if(($id = xivo_uint($id)) === 0)
			return(false);

		if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);

		$extenumbers = &$this->_service->get_module('extenumbers');
		$this->_return = $extenumbers->edit($id,$rs);

		return($this->_return);
	}

	function save($status,$typeval=null)
	{
		if($status !== 'save')
			return(false);

		if(empty($this->_info) === true)
			return($this->add($typeval));
		else
			return($this->edit($typeval));
	}

	function delete($status,$id=0)
	{
		if(empty($this->_info) === false)
			$id = $this->_info['id'];
		else if(($id = xivo_uint($id)) === 0)
			return(null);

		if(($extenumbers = &$this->_service->get_module('extenumbers')) === false)
			return(false);

		$r = $extenumbers->delete($id);

		if($status === 'delete')
			$this->_return = $r;

		return($r);
	}
}

?>
