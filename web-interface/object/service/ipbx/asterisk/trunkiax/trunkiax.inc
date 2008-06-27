<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_trunkiax extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'trunkiax';
	var $_filter	= false;
	var $_origin	= false;
	var $_identity	= false;

	function xivo_service_asterisk_trunkiax(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
		$this->_identity = &$this->_sre->get_identity($this->_name);
	}
	
	function _prepare($data)
	{
		$this->_identity->_prepare_identity($data);

		$data['commented'] = (bool) $data['commented'];

		if(empty($data['call-limit']) === true)
			$data['call-limit'] = '';

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_IAX;
		$arr['category'] = 'trunk';

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_IAX;
		$arr['category'] = 'trunk';

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::edit($id,$arr,$disable));
	}
}

?>
