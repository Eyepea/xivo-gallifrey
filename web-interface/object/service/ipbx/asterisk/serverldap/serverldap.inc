<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_serverldap extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'serverldap';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_serverldap(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function _prepare($data)
	{
		$data['ssl'] = (bool) $data['ssl'];
		$data['commented'] = (bool) $data['commented'];
		$data['identity'] = $data['name'].' ('.$data['host'].')';

		if(is_scalar($data['attrdisplayname']) === true)
			$data['attrdisplayname'] = explode(',',$data['attrdisplayname']);
		else
			$data['attrdisplayname'] = array();

		if(is_scalar($data['attrphonenumber']) === true)
			$data['attrphonenumber'] = explode(',',$data['attrphonenumber']);
		else
			$data['attrphonenumber'] = array();

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::edit($id,$arr,$disable));
	}
}

?>
