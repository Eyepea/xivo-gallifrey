<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_rightcallexten extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'rightcallexten';
	var $_filter		= null;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_rightcallexten(&$sre,&$dso)
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
		if(xivo_haslen($data,'exten') === true && $data['exten']{0} === '_')
			$data['exten'] = substr($data['exten'],1);

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		unset($arr['extenhash']);

		if(xivo_haslen($arr,'exten') === false
		|| ($exten = $this->_sre->chk_extension($arr['exten'])) === false)
			return(false);

		$arr['extenhash'] = sha1($exten);

		if($this->exists($arr['rightcallid'],$arr['extenhash']) !== false)
			return(false);

		$arr['exten'] = $this->_sre->clean_extension($arr['exten']);

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		unset($arr['extenhash']);

		if(xivo_haslen($arr,'exten') === true)
		{
			if(($exten = $this->_sre->chk_extension($arr['exten'])) === false)
				return(false);

			$arr['extenhash'] = sha1($exten);
			$arr['exten'] = $this->_sre->clean_extension($arr['exten']);
		}

		return($this->_dso->edit($id,$arr));
	}

	function exists($rightcallid,$extenhash)
	{
		if(($rightcallid = xivo_uint($rightcallid)) === 0)
			return(null);

		$where = array();
		$where['rightcallid'] = $rightcallid;
		$where['extenhash'] = $extenhash;

		if($this->_dso->exists_exten($where) !== false)
			return(true);

		return(false);
	}
}

?>
