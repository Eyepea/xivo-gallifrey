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

		if(($pos = strrpos($data['mailbox'],'@')) !== false)
		{
			$data['voicemailmailbox'] = substr($data['mailbox'],0,$pos);
			$data['voicemailcontext'] = substr($data['mailbox'],$pos+1);
		}

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

		$arr['username'] = null;
		$arr['fullname'] = null;
		$arr['cid_number'] = null;
		$arr['mohinterpret'] = null;
		$arr['mohsuggest'] = null;
		$arr['defaultip'] = null;
		$arr['port'] = null;
		$arr['regexten'] = null;
		$arr['subscribecontext'] = null;
		$arr['fullcontact'] = null;
		$arr['vmexten'] = null;
		$arr['callingpres'] = null;
		$arr['ipaddr'] = '';
		$arr['regseconds'] = 0;
		$arr['regserver'] = null;
		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_SIP;
		$arr['category'] = 'user';

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		if(isset($arr['callgroup']) === true)
			$arr['pickupgroup'] = $arr['callgroup'];

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_SIP;
		$arr['category'] = 'user';

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		if(isset($arr['callgroup']) === true)
			$arr['pickupgroup'] = $arr['callgroup'];

		if(isset($arr['setvar'],$arr['setvar']{0}) === false)
			unset($arr['setvar']);

		unset($arr['username'],
		      $arr['fullname'],
		      $arr['cid_number'],
		      $arr['mohinterpret'],
		      $arr['mohsuggest'],
		      $arr['defaultip'],
		      $arr['port'],
		      $arr['regexten'],
		      $arr['subscribecontext'],
		      $arr['fullcontact'],
		      $arr['vmexten'],
		      $arr['callingpres'],
		      $arr['ipaddr'],
		      $arr['regseconds'],
		      $arr['regserver']);

		return(parent::edit($id,$arr,$disable));
	}
}

?>
