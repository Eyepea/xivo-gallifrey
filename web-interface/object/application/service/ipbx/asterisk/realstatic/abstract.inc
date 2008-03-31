<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_realstatic_abstract extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_conf		= array();
	var $_filter		= false;
	var $_realstatic	= null;
	var $_status		= '';

	function _init(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		$param = array();
		$param['filename'] = $this->_filename;

		if(isset($this->_cat_metric) === true)
			$param['cat_metric'] = $this->_cat_metric;

		if(isset($this->_var_metric) === true)
			$param['var_metric'] = $this->_var_metric;

		if(isset($this->_category) === true)
			$param['category'] = $this->_category;

		if(isset($this->_var_name) === true)
			$param['var_name'] = $this->_var_name;

		$this->_realstatic = $this->_service->get_realstatic($this->_modulename,$param);

		if($this->_realstatic === false)
			return(false);

		$this->_load_config();

		return(true);
	}

	function get($name,$disable=null)
	{
		$this->_info = $this->_origin = array();

		$this->_get_realstatic($name,$disable);

		return($this->_info);
	}

	function get_nb($arr=null,$disable=null)
	{
		return($this->_realstatic->get_nb($arr,$disable));
	}

	function get_all($disable=null,$prepare=true)
	{
		return($this->_realstatic->get_all($disable,$prepare));
	}

	function get_all_where($arr,$disable=null)
	{
		return($this->_realstatic->get_all_where($arr,$disable));
	}

	function get_all_by_category($disable=null)
	{
		$this->_info = $this->_origin = array();

		return($this->_realstatic->get_all_by_category(null,$disable));
	}

	function enable($id)
	{
		return($this->_realstatic->enable($id));
	}

	function disable($id)
	{
		return($this->_realstatic->disable($id));
	}

	function get_elements()
	{
		return($this->get_element());
	}

	function _get_realstatic($name,$disable=null)
	{
		$this->_info[$this->_name] = false;
		$this->_origin[$this->_name] = false;

		$arr = array();
		$arr['var_name'] = $name;
		$arr['category'] = $this->_category;

		$this->_info[$this->_name] = $this->_realstatic->get_where($arr,$disable);
		$this->_origin[$this->_name] = $this->_realstatic->get_origin();

		return($this->_info[$this->_name]);
	}

	function set($arr)
	{
		$this->_reset();

		if(is_array($arr) === false || isset($arr['var_name']) === false)
			return(false);

		$this->get($arr['var_name']);

		$this->set_realstatic($arr);

		return(($this->get_errnb() < 1));
	}

	function set_realstatic($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['var_name'],$arr['var_val']) === false)
			return(false);

		$rs = array();
		$rs['var_name'] = $arr['var_name'];
		$rs['commented'] = null;

		if(isset($arr['commented']) === true)
			$rs['commented'] = (bool) $arr['commented'];

		if(($value = $this->set_chk_value($arr['var_name'],$arr['var_val'],true)) === false)
		{
			if(($value = $this->get_filter_result()) !== false)
				$rs['var_val'] = $value[$arr['var_name']];

			$this->_set_result($this->_name,$rs);
			$this->_set_error($this->_name,$this->get_filter_error());
			return(false);
		}

		$rs['var_val'] = $value;

		$this->_set_result($this->_name,$rs);

		return(true);
	}

	function save()
	{
		$r = true;

		$this->_status = 'save';
		$this->_return = array();

		if(is_array($this->_info) === false
		|| empty($this->_info) === true
		|| $this->get_errnb() > 0
		|| ($name = $this->save_realstatic()) === false)
		{
			$r = false;
			$this->_reverse_save();
		}

		$this->_status = '';

		return($r);
	}

	function _chk_pre_save()
	{
		return(null);
	}

	function set_save_all($arr,$disable=null)
	{
		if(is_array($arr) === false
		|| ($list = $this->get_element()) === false
		|| is_array($list) === false)
			return(false);
		else if($disable !== null)
			$disable = (bool) $disable;

		$r = array();
		$r['result'] = array();
		$r['error'] = array();

		$rs = array();
		$rs['commented'] = $disable;

		reset($list);

		while(list($key) = each($list))
		{
			if($this->unset_element_value($key) === true)
				continue;

			$rs['var_name'] = $key;

			if(isset($arr[$key]) === true)
				$rs['var_val'] = $arr[$key];
			else
				$rs['var_val'] = '';

			if($this->set($rs) === false
			|| $this->_chk_pre_save($rs) === false
			|| $this->save() === false)
				$r['error'][] = $key;

			$r['result'][$key] = $this->get_result($this->_name);
		}

		return($r);
	}

	function save_realstatic()
	{
		if(isset($this->_info[$this->_name]) === false)
			return(false);
		else if($this->_info[$this->_name] === false)
			return($this->_add_realstatic());
		else if(($rs = $this->get_result($this->_name)) === null
		|| $rs === false)
			return(false);

		$realstatic_where = array();
		$realstatic_where['category'] = $this->_category;
		$realstatic_where['var_name'] = $rs['var_name'];

		$r = $this->_realstatic->edit_where($realstatic_where,$rs);

		if($r === false)
			$this->_return[$this->_name] = false;
		else
			$this->_return[$this->_name] = $rs['var_name'];

		return($this->_return[$this->_name]);
	}

	function _add_realstatic()
	{
		if(($rs = $this->get_result($this->_name)) === null
		|| $rs === false)
			return(false);

		$rs['category'] = $this->_category;
		$rs['cat_metric'] = $this->_cat_metric;

		if($this->_realstatic->add($rs) === false)
			$this->_return[$this->_name] = false;
		else
			$this->_return[$this->_name] = $rs['var_name'];

		return($this->_return[$this->_name]);
	}

	function _reverse_save()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'save'
		|| is_array($this->_return) === false
		|| array_key_exists($this->_name,$this->_return) === false
		|| $this->_return[$this->_name] === false)
			return(false);

		return($this->_reverse_realstatic($this->_return[$this->_name]));
	}

	function _reverse_realstatic($val)
	{
		if(is_scalar($val) === false
		|| $val === false
		|| isset($this->_origin[$this->_name]) === false)
			return(false);

		if($this->_origin[$this->_name] !== false)
			$this->_realstatic->edit_origin($this->_origin[$this->_name]);
		else
		{
			$realstatic_where = array();
			$realstatic_where['category'] = $this->_category;
			$realstatic_where['var_name'] = $val;

			$this->_realstatic->delete_where($realstatic_where);
		}

		return(true);
	}

	function add_origin($origin=null)
	{
		if(empty($this->_origin[$this->_name]) === false
		&& $origin === null)
			$origin = $this->_origin[$this->_name];
		else if(is_array($origin) === false)
			return(false);
			
		return($this->_realstatic->add_origin($origin));
	}

	function edit_origin($origin=null)
	{
		if(empty($this->_origin[$this->_name]) === false
		&& $origin === null)
			$origin = $this->_origin[$this->_name];
		else if(is_array($origin) === false)
			return(false);
			
		return($this->_realstatic->edit_origin($origin));
	}
}

?>
