<?php

class xivo_libconfig_abstract
{
	function _load_config($dir)
	{
		$dir = (string) $dir;

		if(isset($this->_conf) === false)
			return(false);

		if(empty($this->_conf) === false)
			return(null);

		$dir = $dir.XIVO_SEP_DIR.'config';

		if(($this->_conf = xivo_gat::get_file($dir)) !== false)
			return(true);

		return(false);
	}

	function _chk_exists_cfg($type,$name)
	{
		$type = (string) $type;
		$name = (string) $name;

		if(isset($this->_conf) === true
		&& xivo_issa($type,$this->_conf) === true
		&& xivo_ak($name,$this->_conf[$type]) === true)
			return(true);

		return(false);
	}

	function get_option()
	{
		if(isset($this->_conf) === false || xivo_issa('option',$this->_conf) === false)
			return(false);

		return($this->_conf['option']);
	}

	function get_option_value($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false 
		|| xivo_issa('option',$this->_conf) === false
		|| isset($this->_conf['option'][$key]) === false)
			return(false);

		return($this->_conf['option'][$key]);
	}

	function get_element()
	{
		if(isset($this->_conf) === false || xivo_issa('element',$this->_conf) === false)
			return(false);

		return($this->_conf['element']);
	}

	function get_element_where($where)
	{
		if(is_array($where) === false
		|| isset($this->_conf) === false
		|| xivo_issa('element',$this->_conf) === false)
			return(false);

		$arr_keys = array_keys($this->_conf['element']);

		if(($nb = count($arr_keys)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$key = &$arr_keys[$i];

			if(array_key_exists($key,$where) === true
			&& $this->unset_element_value($key) === false)
				$r[$key] = $where[$key];
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_element_value($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| xivo_issa('element',$this->_conf) === false
		|| xivo_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['value']) === false)
			return(false);

		return($this->_conf['element'][$key]['value']);
	}

	function chk_element_value($key,$value)
	{
		if(($values = $this->get_element_value($key)) === false
		|| in_array($value,$values,true) === false)
			return(false);

		return(true);
	}

	function _set_element_default($key,$value)
	{
		$key = (string) $key;
		$value = (string) $value;

		if(isset($this->_conf) === false
		|| xivo_issa('element',$this->_conf) === false
		|| xivo_issa($key,$this->_conf['element']) === false)
			return(false);

		$this->_conf['element'][$key]['default'] = $value;

		return(true);
	}

	function get_element_default($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| xivo_issa('element',$this->_conf) === false
		|| xivo_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['default']) === false)
			return(false);

		return($this->_conf['element'][$key]['default']);
	}

	function isset_element_default($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| xivo_issa('element',$this->_conf) === false
		|| xivo_issa($key,$this->_conf['element']) === false
		|| xivo_ak('setdef',$this->_conf['element'][$key],true) === false)
			return(false);

		return(true);
	}

	function unset_element_default($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === true
		&& xivo_issa('element',$this->_conf) === true
		&& xivo_issa($key,$this->_conf['element']) === true
		&& isset($this->_conf['element'][$key]['setdef']) === true
		&& $this->_conf['element'][$key]['setdef'] === false)
			return(true);

		return(false);
	}

	function isset_element_value($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| xivo_issa('element',$this->_conf) === false
		|| xivo_issa($key,$this->_conf['element']) === false
		|| xivo_ak('set',$this->_conf['element'][$key],true) === false)
			return(false);

		return(true);
	}

	function unset_element_value($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| xivo_issa('element',$this->_conf) === false 
		|| xivo_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['unset']) === false)
			return(false);

		return($this->_conf['element'][$key]['unset']);
	}

	function isnull_element_value($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| xivo_issa('element',$this->_conf) === false
		|| xivo_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['null']) === false)
			return(false);

		return($this->_conf['element'][$key]['null']);
	}

	function _load_filter()
	{
		if(isset($this->_conf,$this->_filter) === false
		|| xivo::load_class('xivo_filter') === false
		|| xivo_issa('filter',$this->_conf) === false)
			return(false);

		$this->_filter = new xivo_filter($this->_conf['filter']);

		return(true);
	}

	function get_filter_error()
	{
		if(isset($this->_filter) === false || $this->_filter === false)
			return(false);

		return($this->_filter->get_error());
	}

	function get_filter_errnb()
	{
		if(isset($this->_filter) === false || $this->_filter === false)
			return(false);

		return($this->_filter->get_errnb());
	}

	function get_filter_result()
	{
		if(isset($this->_filter) === false || $this->_filter === false)
			return(false);

		return($this->_filter->get_result());
	}

	function chk_values($arr,$set=true,$setval=true,$empty=true)
	{
		$set = (bool) $set;
		$setval = (bool) $setval;
		$empty = (bool) $empty;

		if(isset($this->_filter) === false
		|| ($this->_filter === false && $this->_load_filter() === false) === true
		|| $this->_filter->chk($arr,$setval) === false)
			return(false);

		$rs = $this->_filter->get_result();

		if($set === false || ($arr = xivo_get_aks($rs)) === false)
			return($rs);

		$r = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$rs[$key];

			if($this->unset_element_value($key) === true)
				continue;

			if(($sval = $this->_set_value($key,$val)) !== false)
				$val = $sval;

			if($val === '' && $this->isnull_element_value($key) === true)
				$val = null;
			else if($val === null && $this->isnull_element_value($key) === false)
				$val = '';

			if($val === '' && $this->isset_element_default($key) === true
			&& ($dval = $this->get_element_default($key)) !== false)
				$val = $dval;

			$r[$key] = $val;
		}

		if($empty === false && empty($r) === true)
			return(false);
			
		return($r);
	}

	function chk_value($key,$value)
	{
		if(isset($this->_filter) === false 
		|| ($this->_filter === false && $this->_load_filter() === false) === true)
			return(false);

		if($this->_chk_exists_cfg('filter',$key) === false)
			return(null);

		return($this->_filter->chk_value($key,$value));
	}

	function _set_value($key,$val)
	{
		if($this->isset_element_value($key) === false)
			return($val);

		if(($value = $this->get_element_value($key)) === false)
			return(false);

		if(is_scalar($value) === true)
			return($value);

		if(xivo_ak($val,$value) === true)
			return($value[$val]);

		return(false);
	}

	function set_chk_value($key,$value,$valid=false)
	{
		$r = '';
		
		$valid = (bool) $valid;

		if(($val = $this->chk_value($key,$value)) !== false)
		{
			if(($sval = $this->_set_value($key,$val)) !== false)
				$r = $sval;
			else
				$r = $val;
		}
		else if($valid === true)
			return(false);

		if($r === '' && $this->isnull_element_value($key) === true)
			$r = null;
		else if($r === null && $this->isnull_element_value($key) === false)
			$r = '';

		if($r !== ''
		|| ($dval = $this->get_element_default($key)) === false
		|| $this->unset_element_default($key) === true)
			return($r);

		return($dval);
	}
}

?>
