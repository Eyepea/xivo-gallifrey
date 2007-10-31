<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_rightcallmember extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'rightcallmember';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_rightcallmember(&$sre,&$dso)
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

		if($this->exists($arr['rightcallid'],$arr['type'],$arr['typeval']) !== false)
			return(false);

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0
		|| is_array($arr) === false
		|| ($info = $this->get($id)) === false)
			return(false);

		if(isset($arr['rightcallid']) === false)
			$rightcallid = $info['rightcallid'];
		else
			$rightcallid = $arr['rightcallid'];

		if(isset($arr['type']) === false)
			$type = $info['type'];
		else
			$type = $arr['type'];

		if(isset($arr['typeval']) === false)
			$typeval = $info['typeval'];
		else
			$typeval = $arr['typeval'];

		if($this->exists($rightcallid,$type,$typeval,$id) !== false)
			return(false);

		return($this->_dso->edit($id,$arr));
	}

	function is_valid($type,$typeval,$disable=null)
	{
		$type = strval($type);
		$module = false;

		switch($type)
		{
			case 'outcall':
				$typeval = (int) $typeval;

				$module = $this->_sre->get_module('outcall');
				break;
			case 'user':
			case 'group':
				$typeval = (int) $typeval;

				$module = $this->_sre->get_module($type.'features');
				break;
			default:
				return(false);
		}

		if($module === false || ($info = $module->get($typeval,$disable)) === false)
			return(false);

		return($info);
	}

	function exists($rightcallid,$type,$typeval,$id=null)
	{
		$id = $id !== null ? xivo_uint($id) : null;

		if(($rightcallid = xivo_uint($rightcallid)) === 0
		|| $this->chk_element_value('type',$type) === false
		|| xivo_haslen($typeval) === false)
			return(null);

		$where = array();
		$where['rightcallid'] = $rightcallid;
		$where['type'] = $type;
		$where['typeval'] = $typeval;

		if(($whereid = $this->get_id($where)) !== false
		&& ($id === null || $whereid !== $id) === true)
			return(true);

		return(false);
	}
}

?>
