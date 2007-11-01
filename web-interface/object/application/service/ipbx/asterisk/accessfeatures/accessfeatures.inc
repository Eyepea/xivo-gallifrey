<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract.inc'),true);

class xivo_application_service_asterisk_accessfeatures extends xivo_application_service_asterisk_abstract
{
	var $_accessfeatures	= null;
	var $_type		= '';

	function xivo_application_service_asterisk_accessfeatures(&$service,$param=array())
	{
		$this->xivo_application_service_asterisk_abstract(&$service);

		if(is_array($param) === false
		|| isset($param['type']) === false
		|| ($this->_accessfeatures = &$this->_service->get_module('accessfeatures')) === false
		|| $this->_accessfeatures->chk_element_value('type',$param['type']) === false)
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

		if(($this->_info = $this->_accessfeatures->get_all_where($where)) === false)
			return(false);

		$this->_origin = $this->_accessfeatures->get_origin_list();

		return($this->_info);
	}

	function get_elements()
	{
		return($this->_accessfeatures->get_element());
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
			$info['host'] = $list[$i];

			if(($rs = $this->_accessfeatures->chk_values($info)) === false
			|| isset($tmp[$rs['host']]) === true)
			{
				$errlist[] = 'wrong server';
				continue;
			}

			$tmp[$rs['host']] = 1;
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
			if(($this->_return[] = $this->_accessfeatures->add($rs[$i])) !== false)
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

			if(($this->_return[] = $this->_accessfeatures->add($rs[$i],$aid)) === false)
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

		if(($r = $this->_accessfeatures->delete_where($where)) === false)
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
				$this->_accessfeatures->delete($this->_return[$i]);
		}

		return(true);
	}

	function _reverse_edit()
	{
		$this->delete();

		return($this->_accessfeatures->add_origin_list());
	}

	function _reverse_delete()
	{
		return($this->_accessfeatures->add_origin_list());
	}
}

?>
