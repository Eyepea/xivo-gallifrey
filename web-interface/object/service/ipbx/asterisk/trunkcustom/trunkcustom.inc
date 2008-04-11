<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_trunkcustom extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'trunkcustom';
	var $_filter	= false;
	var $_origin	= false;

	function xivo_service_asterisk_trunkcustom(&$sre,&$dso)
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
		$data['commented'] = (bool) $data['commented'];

		if(xivo_haslen($data['intfsuffix']) === true)
			$data['identity'] = $data['name'].' ('.$data['interface'].'|'.$data['intfsuffix'].')';
		else
			$data['identity'] = $data['name'].' ('.$data['interface'].')';

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_CUSTOM;
		$arr['category'] = 'trunk';

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		if(isset($arr['interface']) === true)
			$arr['interface'] = strtolower($arr['interface']);

		if(isset($arr['intfsuffix']) === true)
			$arr['intfsuffix'] = strtolower($arr['intfsuffix']);

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_CUSTOM;
		$arr['category'] = 'trunk';

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		if(isset($arr['interface']) === true)
			$arr['interface'] = strtolower($arr['interface']);

		if(isset($arr['intfsuffix']) === true)
			$arr['intfsuffix'] = strtolower($arr['intfsuffix']);

		return(parent::edit($id,$arr,$disable));
	}
}

?>
