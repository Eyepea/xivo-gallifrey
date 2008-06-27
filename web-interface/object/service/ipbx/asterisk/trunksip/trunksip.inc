<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_trunksip extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'trunksip';
	var $_filter	= false;
	var $_origin	= false;

	function xivo_service_asterisk_trunksip(&$sre,&$dso)
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
		$this->_prepare_identity($data);

		if(empty($data['call-limit']) === true)
			$data['call-limit'] = '';

		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function _prepare_identity(&$data)
	{
		if(($channel = $this->_sre->get_channel_by_protocol('iax')) !== false)
			$data['identity'] = $data['name'].' ('.$channel.')';
		else
			$data['identity'] = $data['name'];

		return($data);
	}

	function mk_identity($name)
	{
		$arr = array();
		$arr['name'] = $name;

		$this->_prepare_identity($arr);

		return($arr['identity']);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_SIP;
		$arr['category'] = 'trunk';

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_SIP;
		$arr['category'] = 'trunk';

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::edit($id,$arr,$disable));
	}
}

?>
