<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'libdatastorage.inc');

class xivo_server extends xivo_libdatastorage_abstract
{
	var $_conf		= array();
	var $_dso		= null;
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_server()
	{
		$this->_load_config(dirname(__FILE__).XIVO_SEP_DIR.'server');
		$this->_load_dso();
	}

	function _prepare($data)
	{
		$data['disable'] = (bool) $data['disable'];
		$data['ssl'] = (bool) $data['ssl'];
		$data['identity'] = $data['name'].' ('.$data['host'].')';
		
		$data['url_http'] = '://'.$data['host'].':'.$data['port'];

		if($data['ssl'] === true)
			$data['url_http'] = 'https'.$data['url_http'];
		else
			$data['url_http'] = 'http'.$data['url_http'];

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);
		else if(empty($id) === false)
			$arr['dcreate'] = gmmktime();

		return(parent::add($arr,$id));
	}

	function _load_dso()
	{
		$dso = &xivo_gct::get('XIVO_DSO');
		$type = $dso->get_type();
		$class = 'xivo_server_'.$type;

		$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.'server'.XIVO_SEP_DIR.'datastorage';

		if(xivo::load_class($class,$dir,$type,false) === false)
		{
			trigger_error('Failed to load datastorage '.$type.' for server',E_USER_ERROR);
			return(false);
		}

		$this->_dso = new $class($dso);
		return(true);
	}
}

?>
