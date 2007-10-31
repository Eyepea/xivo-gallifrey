<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract.inc'),true);

class xivo_application_service_asterisk_dialstatus_abstract extends xivo_application_service_asterisk_abstract
{
	var $_dialstatus	= null;

	function xivo_application_service_asterisk_dialstatus_abstract(&$service)
	{
		$this->xivo_application_service_asterisk_abstract(&$service);
		
		if(($this->_dialstatus = &$this->_service->get_module('dialstatus')) === false)
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
		$where['category'] = $type;
		$where['categoryval'] = $id;

		if(($info = $this->_dialstatus->get_all_where($where)) === false
		|| ($nb = count($info)) === 0)
			return(false);

		$this->_info = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($this->_info[$info[$i]['status']]) === true)
				continue;

			$this->_info[$info[$i]['status']] = $info[$i];
		}

		$this->_origin = $this->_dialstatus->get_origin_list();

		return($this->_info);
	}

	function get_sound()
	{
		if(($sounds = &$this->_service->get_module('sounds')) === false)
			return(false);

		return($sounds->get_list('dialstatus',true));
	}

	function get_element()
	{
		return($this->_dialstatus->get_element());
	}

	function set($list,$type)
	{
		$this->_reset();

		if(is_array($list) === false
		|| is_string($type) === false
		|| ($arr = xivo_get_aks($list,true)) === false)
			return(false);

		if($arr['cnt'] === 0)
			return(null);

		$info = array();
		$info['category'] = $type;
		$info['categoryval'] = 0;
		$info['linked'] = true;

		$reslist = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$list[$key];

			if(is_array($val) === false)
				continue;

			$info['status'] = $key;
			$info = array_merge($info,$val);

			if(($rs = $this->_dialstatus->chk_values($info)) === false
			|| isset($tmp[$rs['status']]) === true)
				continue;

			$reslist[$rs['status']] = $rs;
		}

		if(empty($reslist) === true)
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

		if(($arr = xivo_get_aks($rs)) === false)
			return(false);

		$this->_return = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$ref = &$rs[$key];
			$ref['categoryval'] = $id;

			if($this->_dialstatus->is_valid($ref['category'],
						 $ref['categoryval'],
						 $ref['type'],
						 $ref['typeval']) === false
			|| ($this->_return[$ref['status']] = $this->_dialstatus->add($ref)) === false)
				return(false);
		}

		if(empty($this->_return) === true)
			$this->_return = false;

		return(true);
	}

	function add_origin_list()
	{
		if(($rightcallmember = &$this->_service->get_module('rightcallmember')) === false)
			return(false);

		return($rightcallmember->add_origin_list($this->_origin));
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

		if(($arr = xivo_get_aks($rs)) === false)
			return(false);

		$this->_return = array();

		$list = xivo_get_aks($this->_info);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			if($list !== false && isset($list['keys'][$i]) === true)
				$did = $this->_info[$list['keys'][$i]]['id'];
			else
				$did = 0;

			$ref = &$rs[$arr['keys'][$i]];
			$ref['categoryval'] = $id;

			if($this->_dialstatus->is_valid($ref['category'],
							$ref['categoryval'],
							$ref['type'],
							$ref['typeval']) === false
			|| ($this->_return[$ref['status']] = $this->_dialstatus->add($ref,$did)) === false)
				return(false);
		}

		if(empty($this->_return) === true)
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

		$dialstatus_where = array();
		$dialstatus_where['category'] = $type;
		$dialstatus_where['categoryval'] = $id;

		$r = $this->_dialstatus->delete_where($dialstatus_where);

		if($status === 'delete')
			$this->_return = $r;

		return($r);
	}

	function reverse_add()
	{
		if(is_array($this->_return) === false || ($nb = count($this->_return)) === 0)
			continue;

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_return[$i] !== false)
				$this->_dialstatus->delete($this->_return[$i]);
		}

		return(true);
	}

	function reverse_edit($status,$type,$id)
	{
		$this->delete($status,$type,$id);

		return($this->add_origin_list());
	}

	function unlinked($type,$id)
	{
		if(is_string($type) === false
		|| ($id = xivo_uint($id)) === 0)
			return(false);

		$dialstatus_where = array();
		$dialstatus_where['type'] = $type;
		$dialstatus_where['typeval'] = $id;

		$r = $this->_dialstatus->unlinked_where($dialstatus_where);

		return($r);
	}
}

?>
