<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_usersip extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'usersip';
	var $_filter	= false;	
	var $_origin	= false;

	function xivo_service_asterisk_usersip(&$sre,&$dso)
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
		$data['initialized'] = xivo_haslen($data['fullcontact']);
		$data['commented'] = (bool) $data['commented'];
		$data['callgroup'] = empty($data['callgroup']) === false ? explode(',',$data['callgroup']) : '';
		$data['pickupgroup'] = empty($data['pickupgroup']) === false ? explode(',',$data['pickupgroup']) : '';

		return($data);
	}

	function get_nb($arr=null,$disable=null,$initialized=null)
	{
		if($arr !== null && is_array($arr) === false)
			return(false);
		else if(is_array($arr) === true
		&& ($arr = $this->get_element_where($arr)) === false)
			return(false);

		return($this->_dso->get_nb($arr,$disable,$initialized));
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		$arr['pickupgroup'] = $arr['callgroup'];
		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_SIP;
		$arr['category'] = 'user';

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		if(isset($arr['callgroup']) === true)
			$arr['pickupgroup'] = $arr['callgroup'];

		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_SIP;
		$arr['category'] = 'user';

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::edit($id,$arr,$disable));
	}
}

?>
