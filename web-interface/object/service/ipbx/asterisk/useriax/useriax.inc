<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_useriax extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'useriax';
	var $_filter	= false;	
	var $_origin	= false;

	function xivo_service_asterisk_useriax(&$sre,&$dso)
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
		$data['initialized'] = false;
		$data['commented'] = (bool) $data['commented'];
		$data['callgroup'] = empty($data['callgroup']) === false ? explode(',',$data['callgroup']) : '';

		if(($pos = strrpos($data['mailbox'],'@')) !== false)
		{
			$data['voicemailmailbox'] = substr($data['mailbox'],0,$pos);
			$data['voicemailcontext'] = substr($data['mailbox'],$pos+1);
		}

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_IAX;
		$arr['category'] = 'user';

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_IAX;
		$arr['category'] = 'user';

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::edit($id,$arr,$disable));
	}
}

?>
