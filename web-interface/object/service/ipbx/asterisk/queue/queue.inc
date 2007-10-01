<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_queue extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'queue';
	var $_filter	= null;
	var $_origin	= false;

	function xivo_service_asterisk_queue(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get($id,$disable=null)
	{
		if(is_scalar($id) === false
		|| ($r = $this->_dso->get($id,$disable)) === false)
			return(false);

		$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function _prepare($data)
	{
		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function _origin($type,$val=null)
	{
		if($val !== null)
			$orig = $val;
		else
			$orig = $this->_origin;

		if(($type !== 'add' && $type !== 'edit') === true
		|| is_array($orig) === false)
			return(false);

		if($type === 'add')
			return($this->add($orig));
		else
			return($this->edit($orig['name'],$orig));
	}

	function add($arr)
	{
		if(is_array($arr) === false)
			return(false);

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_scalar($id) === false || is_array($arr) === false)
			return(false);

		return($this->_dso->edit($id,$arr,$disable));
	}

	function delete($id,$disable=null)
	{
		if(is_scalar($id) === false)
			return(false);

		return($this->_dso->delete($id));
	}
}

?>
