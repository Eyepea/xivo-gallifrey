<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract.inc'),true);

class xivo_application_service_asterisk_rightcall_abstract extends xivo_application_service_asterisk_abstract
{
	var $_rightcall		= null;
	var $_rightcallmember	= null;

	function xivo_application_service_asterisk_rightcall_abstract(&$service)
	{
		$this->xivo_application_service_asterisk_abstract(&$service);
		
		if(($this->_rightcall = &$this->_service->get_module('rightcall')) === false
		|| ($this->_rightcallmember = &$this->_service->get_module('rightcallmember')) === false)
			return(false);

		return(true);
	}

	function get($type,$id)
	{
		$this->_info = false;
		$this->_origin = false;

		if(is_string($type) === false || ($id = xivo_uint($id)) === 0)
			return(false);

		$where = array();
		$where['type'] = $type;
		$where['typeval'] = $id;

		if(($this->_info = $this->_rightcallmember->get_all_where($where)) === false
		|| isset($this->_info[0]) === false)
			return(false);

		$this->_origin = $this->_rightcallmember->get_origin_list();

		return($this->_info);
	}

	function set($list,$type)
	{
		$this->_reset();

		if(is_array($list) === false || is_string($type) === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === 0)
			return(null);

		$info = array();
		$info['rightcallid'] = 0;
		$info['type'] = $type;
		$info['typeval'] = 0;
		
		$tmp = $reslist = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($rightcallid = $this->_rightcall->get_id($list[$i])) === false)
			{
				$this->_set_error('wrong rightcall');
				continue;
			}

			$info['rightcallid'] = $rightcallid;

			if(($rs = $this->_rightcallmember->chk_values($info)) === false
			|| isset($tmp[$rightcallid]) === true)
				continue;

			$tmp[$rightcallid] = 1;
			$reslist[] = $rs;
		}

		if(isset($reslist[0]) === false)
		{
			$this->_set_result(false);
			return(false);
		}

		$this->_set_result($reslist);

		return(true);
	}

	function add($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);

		$this->_return = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['typeval'] = $id;

			if(($this->_return[] = $this->_rightcallmember->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return[0]) === false)
			$this->_return = false;

		return(true);
	}

	function add_origin_list()
	{
		return($this->_rightcallmember->add_origin_list($this->_origin));
	}

	function edit($status,$type,$id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		if(empty($this->_info) === true)
			return($this->add($id));

		$this->_return = $this->delete($status,$type,$id);

		if(($rs = $this->get_result()) === null)
			return($this->_return);

		if($rs === false)
			return(false);

		$this->_return = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($this->_info[$i]) === true)
				$rid = $this->_info[$i]['id'];
			else
				$rid = 0;

			$rs[$i]['typeval'] = $id;

			if(($this->_return[] = $this->_rightcallmember->add($rs[$i],$rid)) === false)
				return(false);
		}

		if(isset($this->_return[0]) === false)
			$this->_return = false;

		return(true);
	}

	function delete($status,$type,$id)
	{
		if(empty($this->_info) === true)
			return(null);

		if(is_string($type) === false
		|| ($id = xivo_uint($id)) === 0)
			return(false);

		$rightcallmember_where = array();
		$rightcallmember_where['type'] = $type;
		$rightcallmember_where['typeval'] = $id;

		$r = $this->_rightcallmember->delete_where($rightcallmember_where);

		if($status === 'delete')
			$this->_return = $r;

		return($r);
	}

	function reverse_add()
	{
		if(is_array($this->_return) === false || ($nb = count($this->_return)) === 0)
			return(null);

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_return[$i] !== false)
				$this->_rightcallmember->delete($this->_return[$i]);
		}

		return(true);
	}

	function reverse_edit($status,$type,$id)
	{
		$this->delete($status,$type,$id);

		return($this->add_origin_list());
	}
}

?>
