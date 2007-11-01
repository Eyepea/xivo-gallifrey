<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract.inc'),true);

class xivo_application_service_asterisk_serverfeatures extends xivo_application_service_asterisk_abstract
{
	var $_serverfeatures	= null;
	var $_server		= null;
	var $_type		= '';

	function xivo_application_service_asterisk_serverfeatures(&$service,$param=array())
	{
		$this->xivo_application_service_asterisk_abstract(&$service);

		if(is_array($param) === false
		|| isset($param['type']) === false
		|| ($this->_serverfeatures = &$this->_service->get_module('serverfeatures')) === false
		|| $this->_serverfeatures->chk_element_value('type',$param['type']) === false
		|| xivo::load_class('xivo_server',XIVO_PATH_OBJECT,null,false) === false
		|| ($this->_server = new xivo_server()) === false)
			return(false);

		$this->_type = $param['type'];

		return(true);
	}

	function get()
	{
		$this->_info = false;
		$this->_origin = false;

		$where = array();
		$where['type'] = $this->_type;

		if(($this->_info = $this->_serverfeatures->get_all_where($where)) === false)
			return(false);

		$this->_origin = $this->_serverfeatures->get_origin_list();

		return($this->_info);
	}

	function get_server_list()
	{
		if(($arr = $this->_server->get_all()) === false
		|| ($nb = count($arr)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[$arr[$i]['id']] = $arr[$i];

		return($r);
	}

	function get_elements()
	{
		return($this->_serverfeatures->get_element());
	}

	function set($list)
	{
		$this->_reset();

		if(is_array($list) === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === 0)
			return(null);

		$info = array();
		$info['type'] = $this->_type;
		
		$tmp = $reslist = $errlist = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($info['serverid'] = $this->_server->get_id($list[$i])) === false
			|| ($rs = $this->_serverfeatures->chk_values($info)) === false
			|| isset($tmp[$rs['serverid']]) === true)
			{
				$errlist[] = 'wrong server';
				continue;
			}

			$tmp[$rs['serverid']] = 1;
			$reslist[] = $rs;
		}

		if(isset($reslist[0]) === false)
			$reslist = false;

		if(isset($errlist[0]) === true || $reslist === false)
		{
			$this->_set_result($reslist);
			$this->_set_error($errlist);
			return(false);
		}

		$this->_set_result($reslist);

		return(true);
	}

	function add()
	{
		$this->_status = 'add';

		if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);

		$this->_return = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if(($this->_return[] = $this->_serverfeatures->add($rs[$i])) !== false)
				continue;

			if($this->_status === 'add')
				$this->_reverse_add();
			else if($this->_status === 'edit' || $this->_status === 'save')
				$this->_reverse_edit();

			return(false);
		}

		if(isset($this->_return[0]) === false)
			$this->_return = false;

		return(true);
	}

	function edit()
	{
		$this->_status = 'edit';

		return($this->_edit_save());
	}

	function save()
	{
		$this->_status = 'save';

		return($this->_edit_save());
	}

	function _edit_save()
	{
		if(empty($this->_info) === true)
			return($this->add());

		$this->_return = $this->delete();

		if(($rs = $this->get_result()) === null)
			return($this->_return);

		if($rs === false)
			return(false);

		$this->_return = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($this->_info[$i]) === true)
				$aid = $this->_info[$i]['id'];
			else
				$aid = 0;

			if(($this->_return[] = $this->_serverfeatures->add($rs[$i],$aid)) === false)
				return(false);
		}

		if(isset($this->_return[0]) === false)
			$this->_return = false;

		return(true);
	}

	function delete()
	{
		$this->_status = 'delete';

		if(empty($this->_info) === true)
			return(null);

		$where = array();
		$where['type'] = $this->_type;

		if(($r = $this->_serverfeatures->delete_where($where)) === false)
			$this->_reverse_delete();

		if($this->_status === 'delete')
			$this->_return = $r;

		return($r);
	}

	function _reverse_add()
	{
		if(is_array($this->_return) === false || ($nb = count($this->_return)) === 0)
			return(null);

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_return[$i] !== false)
				$this->_serverfeatures->delete($this->_return[$i]);
		}

		return(true);
	}

	function _reverse_edit()
	{
		$this->delete();

		return($this->_serverfeatures->add_origin_list());
	}

	function _reverse_delete()
	{
		return($this->_serverfeatures->add_origin_list());
	}
}

?>
