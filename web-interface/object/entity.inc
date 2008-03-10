<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'libdatastorage.inc');

class xivo_entity extends xivo_libdatastorage_abstract
{
	var $_conf		= array();
	var $_dso		= null;
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_entity()
	{
		$this->_load_config(dirname(__FILE__).XIVO_SEP_DIR.'entity');
		$this->_load_dso();
	}

	function _prepare($data)
	{
		$data['disable'] = (bool) $data['disable'];
		$data['identity'] = $data['name'];

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);
		else if(empty($id) === true || isset($arr['dcreate']) === false)
			$arr['dcreate'] = gmmktime();

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::edit($id,$arr,$disable));
	}

	function _load_dso()
	{
		$dso = &xivo_gct::get('XIVO_DSO');
		$type = $dso->get_type();
		$class = 'xivo_entity_'.$type;

		$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.'entity'.XIVO_SEP_DIR.'datastorage';

		if(xivo::load_class($class,$dir,$type,false) === false)
		{
			trigger_error('Failed to load datastorage '.$type.' for entity',E_USER_ERROR);
			return(false);
		}

		$this->_dso = new $class($dso);
		return(true);
	}
}

?>
