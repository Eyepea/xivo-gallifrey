<?php

class xivo_service_asterisk_abstract
{
	var $_conf	= array();
	var $_sre	= null;

	function get_name()
	{
		return($this->_name);
	}

	function _chk_exists_cfg($type,$name)
	{
		$r = false;

		$type = (string) $type;
		$name = (string) $name;

		if(xivo_issa($type,$this->_conf) === true && xivo_ak($name,$this->_conf[$type]) === true)
			$r = true;

		return($r);
	}

	function get_option()
	{
		if(xivo_issa('option',$this->_conf) === false)
			return(false);

		return($this->_conf['option']);
	}

	function get_option_value($key)
	{
		$key = (string) $key;

		if(xivo_issa('option',$this->_conf) === false || isset($this->_conf['option'][$key]) === false)
			return(false);

		return($this->_conf['option'][$key]);
	}

	function get_element()
	{
		if(xivo_issa('element',$this->_conf) === false)
			return(false);

		return($this->_conf['element']);
	}

	function get_element_value($key)
	{
		$key = (string) $key;

		if(xivo_issa('element',$this->_conf) === false || xivo_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['value']) === false)
			return(false);

		return($this->_conf['element'][$key]['value']);
	}

	function _set_element_default($key,$value)
	{
		$key = (string) $key;
		$value = (string) $value;

		if(xivo_issa('element',$this->_conf) === false || xivo_issa($key,$this->_conf['element']) === false)
			return(false);

		$this->_conf['element'][$key]['default'] = $value;

		return(true);
	}

	function get_element_default($key)
	{
		$key = (string) $key;

		if(xivo_issa('element',$this->_conf) === false || xivo_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['default']) === false)
			return(false);

		return($this->_conf['element'][$key]['default']);
	}

	function get_element_copy($key)
	{
		$key = (string) $key;

		if(xivo_issa('element',$this->_conf) === false || xivo_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['copy']) === false)
			return(false);

		return($this->_conf['element'][$key]['copy']);
	}

	function isset_element_value($key)
	{
		$key = (string) $key;

		if(xivo_issa('element',$this->_conf) === false || xivo_issa($key,$this->_conf['element']) === false
		|| xivo_ak('set',$this->_conf['element'][$key],true) === false)
			return(false);

		return(true);
	}

	function unset_element_value($key)
	{
		$key = (string) $key;

		if(xivo_issa('element',$this->_conf) === false || xivo_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['unset']) === false)
			return(false);

		return($this->_conf['element'][$key]['unset']);
	}

	function disable_element_value($key)
	{
		$key = (string) $key;

		if(xivo_issa('element',$this->_conf) === false || xivo_issa($key,$this->_conf['element']) === false
		|| xivo_ak('disable',$this->_conf['element'][$key],true) === false)
			return(false);

		return(true);
	}

	function _load_config()
	{
		if(empty($this->_conf) === false)
			return(null);

		$dir = dirname(__FILE__).XIVO_SEP_DIR.$this->_name.XIVO_SEP_DIR.'config';

		if(($this->_conf = xivo_gat::get_file($dir)) === false)
			$this->_conf = array();
	}

	function _load_filter()
	{
		if(xivo::load_class('xivo_filter') === false || xivo_issa('filter',$this->_conf) === false)
			return(false);

		$this->_filter = new xivo_filter($this->_conf['filter']);

		return(true);
	}

	function get_list($disable=null)
	{
		if(($r = $this->_dso->get_list($disable)) !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all($disable=null)
	{
		$r = false;

		if(($list = $this->get_list()) === false)
			return($r);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $this->get($list[$i],$disable)) === false)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
	
	function get_filter_err()
	{
		if($this->_filter === null)
			return(false);

		return($this->_filter->get_err());
	}

	function get_filter_result()
	{
		if($this->_filter === null)
			return(false);

		return($this->_filter->get_result());
	}

	function chk_values($arr,$set=false,$default=false)
	{
		$set = (bool) $set;
		$default = (bool) $default;

		if($this->_filter === null && $this->_load_filter() === false)
			return(false);

		if($this->_filter->chk($arr) === false)
			return(false);

		$rs = $this->_filter->get_result();

		if($set === false || ($arr = xivo_get_aks($rs)) === false)
			return($rs);

		$r = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$rs[$key];

			if(($sval = $this->_set_value($key,$val)) !== false)
				$val = $sval;
			else if($default === true && ($dval = $this->get_element_default($key)) !== false)
				$val = $dval;
			
			if(($ckey = $this->get_element_copy($key)) !== false && $val !== '')
				$rs[$ckey] = $r[$ckey] = $val;

			if($this->unset_element_value($key) === true)
				continue;

			$r[$key] = $val;
		}

		return($r);
	}

	function chk_value($key,$value)
	{
		if($this->_filter === null && $this->_load_filter() === false)
			return(false);

		if($this->_chk_exists_cfg('filter',$key) === false)
			return(null);

		return($this->_filter->chk_value($key,$value));
	}

	function _set_value($key,$val)
	{
		if($this->isset_element_value($key) === false)
			return($val);

		if(($values = $this->get_element_value($key)) !== false && xivo_ak($val,$values) === true)
			return($values[$val]);

		return(false);
	}

	function set_chk_value($key,$value,$default=null)
	{
		$r = '';

		if(($val = $this->chk_value($key,$value)) !== false)
		{
			if(($sval = $this->_set_value($key,$val)) !== false)
				$val = $sval;

			return($val);
		}

		if($default !== null || ($default = $this->get_element_default($key)) !== false)
			return($default);

		return($r);
	}
}

?>
