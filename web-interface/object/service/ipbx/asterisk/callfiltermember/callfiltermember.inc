<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_callfiltermember extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'callfiltermember';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_callfiltermember(&$sre,&$dso)
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
		$data['active'] = (bool) $data['active'];

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false
		|| $this->exists($arr) !== false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0
		|| is_array($arr) === false
		|| $this->exists($arr,$id) !== false)
			return(false);

		return($this->_dso->edit($id,$arr));
	}

	function is_valid($type,$typeval,$disable=null)
	{
		$type = strval($type);

		switch($type)
		{
			case 'user':
				$typeval = (int) $typeval;
				$module = $this->_sre->get_module('userfeatures');
				break;
			default:
				return(false);
		}

		if(($info = $module->get($typeval,$disable)) === false)
			return(false);

		return($info);
	}

	function exists($arr,$id=null)
	{
		$id = $id !== null ? xivo_uint($id) : null;

		if(is_array($arr) === false
		|| isset($arr['callfilterid'],$arr['type'],$arr['typeval']) === false
		|| ($arr['callfilterid'] = xivo_uint($arr['callfilterid'])) === 0
		|| $this->chk_element_value('type',$arr['type']) === false
		|| xivo_haslen($arr['typeval']) === false)
			return(null);

		$where = array();
		$where['callfilterid'] = $arr['callfilterid'];
		$where['type'] = $arr['type'];
		$where['typeval'] = $arr['typeval'];

		if(($whereid = $this->get_id($where)) !== false
		&& ($id === null || $whereid !== $id) === true)
			return(true);

		return(false);
	}
}

?>
