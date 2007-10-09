<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract','abstract.inc'),true);

class xivo_application_service_asterisk_extenumbers_abstract extends xivo_application_service_asterisk_abstract
{
	function get($number,$context)
	{
		$this->_info = false;
		$this->_origin = false;

		if(is_scalar($number) === false || is_scalar($context) === false)
			return(false);

		if(($extenumbers = &$this->_service->get_module('extenumbers')) === false)
			return(false);

		$extenumbers_where = array();
		$extenumbers_where['exten'] = $number;
		$extenumbers_where['context'] = $context;

		$this->_info = $extenumbers->get_where($extenumbers_where);
		$this->_origin = $extenumbers->get_origin();

		return($this->_info);
	}

	function set($action,$number,$context,$id=0)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| ($extenumbers = &$this->_service->get_module('extenumbers')) === false)
			return(false);

		if($action === 'edit')
		{
			if(empty($this->_info) === false)
				$id = $this->_info['id'];
			else if(($id = xivo_uint($id)) === 0)
				$id = null;
		}
		else
			$id = null;

		$extenum = array();
		$extenum['exten'] = $number;
		$extenum['context'] = $context;

		$exists = false;

		if(($rs = $extenumbers->chk_values($extenum)) === false
		|| ($exists = $extenumbers->exists($rs,$id)) !== false)
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

	function add()
	{
		if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);

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
