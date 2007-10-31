<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'libdatastorage.inc');

class xivo_server extends xivo_libdatastorage_abstract
{
	var $_conf	= array();
	var $_dso	= null;
	var $_filter	= false;

	function xivo_server()
	{
		$this->_load_config(dirname(__FILE__).XIVO_SEP_DIR.'server');
		$this->_load_dso();
	}

	function _prepare($data)
	{
		$data['disable'] = (bool) $data['disable'];
		$data['ssl'] = (bool) $data['ssl'];

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		if($this->exists_host_port($arr['host'],$arr['port']) !== false)
			return(false);

		if($this->exists_name($arr['name']) !== false)
			return(false);

		$arr['dcreate'] = gmmktime();

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0
		|| is_array($arr) === false
		|| ($info = $this->get($id)) === false)
			return(false);

		if(isset($arr['host']) === false)
			$host = $info['host'];
		else
			$host = $arr['host'];

		if(isset($arr['port']) === false)
			$port = $info['port'];
		else
			$port = $arr['port'];

		if(isset($arr['name']) === true
		&& $this->exists_name($arr['name'],$id) !== false)
			return(false);

		if($this->exists_host_port($host,$port,$id) !== false)
			return(false);

		return($this->_dso->edit($id,$arr));
	}

	function exists_name($name,$id=null)
	{
		$id = $id !== null ? xivo_uint($id) : null;

		if(($whereid = $this->get_id(array('name' => $name))) !== false
		&& ($id === null || $whereid !== $id) === true)
			return(true);

		return(false);
	}

	function exists_host_port($host,$port,$id=null)
	{
		$id = $id !== null ? xivo_uint($id) : null;

		if(xivo_haslen($host) === false
		|| xivo_haslen($port) === false)
			return(null);

		$where = array();
		$where['host'] = $host;
		$where['port'] = $port;

		if(($whereid = $this->get_id($where)) !== false
		&& ($id === null || $whereid !== $id) === true)
			return(true);

		return(false);
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
