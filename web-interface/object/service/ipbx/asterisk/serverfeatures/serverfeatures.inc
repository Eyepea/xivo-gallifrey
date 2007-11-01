<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_serverfeatures extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'serverfeatures';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_serverfeatures(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		if($this->exists($arr['serverid'],$arr['type']) !== false)
			return(false);

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0
		|| is_array($arr) === false
		|| ($info = $this->get($id)) === false)
			return(false);

		if(isset($arr['serverid']) === false)
			$serverid = $info['serverid'];
		else
			$serverid = $arr['serverid'];

		if(isset($arr['type']) === false)
			$type = $info['type'];
		else
			$type = $arr['type'];

		if($this->exists($serverid,$type,$id) !== false)
			return(false);

		return($this->_dso->edit($id,$arr));
	}

	function exists($serverid,$type,$id=null)
	{
		$id = $id !== null ? xivo_uint($id) : null;

		if(($serverid = xivo_uint($serverid)) === 0
		|| $this->chk_element_value('type',$type) === false)
			return(null);

		$where = array();
		$where['serverid'] = $serverid;
		$where['type'] = $type;

		if(($whereid = $this->get_id($where)) !== false
		&& ($id === null || $whereid !== $id) === true)
			return(true);

		return(false);
	}
}

?>
