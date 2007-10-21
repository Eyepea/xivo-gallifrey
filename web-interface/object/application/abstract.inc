<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'libconfig.inc');

class xivo_application_abstract extends xivo_libconfig_abstract
{
	var $_errnb	= 0;
	var $_error	= array();
	var $_result	= array();
	var $_return	= array();
	var $_info	= array();

	function _load_abstract_class($abstract)
	{
		$abstract = (string) $abstract;

		$file = $abstract.'.inc';

		$dir = array(XIVO_PATH_OBJECT,'application','service',$this->_service_type,$this->_service_name,'abstract');

		$class = 'xivo_application_service_'.$this->_service_name.'_'.$abstract.'_abstract';

		if(xivo_file::load_file($file,$dir,true) !== false && class_exists($class) === true)
			return($class);

		return(false);
	}

	function _load_abstract($name)
	{
		$name = (string) $name;

		if(isset($this->_abstract[$name]) === true)
			return(true);
		
		if(($class = $this->_load_abstract_class($name)) === false)
		{
			trigger_error('Abstract Application '.$name.' does not exist in '.$this->_service_name,E_USER_ERROR);
			return(false);
		}

		$this->_abstract[$name] = new $class($this->_service);

		return(true);
	}

	function &get_abstract($name)
	{
		$r = false;

		$name = (string) $name;

		if($this->_load_abstract($name) === true)
			$r = &$this->_abstract[$name];

		return(($ref = &$r));
	}

	function _reset()
	{
		$this->_errnb = 0;
		$this->_error = array();
		$this->_result = array();
		$this->_return = array();
	}

	function get_info()
	{
		return($this->_info);
	}

	function get_info_var($var)
	{
		$var = (string) $var;

		if(is_array($this->_info) === false
		|| isset($this->_info[$var]) === false)
			return(null);

		return($this->_info[$var]);
	}

	function _set_result($name,$value)
	{
		$name = (string) $name;
		$this->_result[$name] = $value;
	}

	function _set_result_var($name,$var,$value)
	{
		$name = (string) $name;
		$var = (string) $var;

		$this->_result[$name][$var] = $value;
	}

	function get_result($name='')
	{
		$name = (string) $name;

		if(empty($this->_result) === true)
			return(null);

		if($name === '')
			return($this->_result);

		if(isset($this->_result[$name]) === true)
			return($this->_result[$name]);

		return(null);
	}

	function get_result_var($name,$var)
	{
		$name = (string) $name;
		$var = (string) $var;

		if(xivo_issa($name,$this->_result) === false
		|| isset($this->_result[$name][$var]) === false)
			return(null);

		return($this->_result[$name][$var]);
	}

	function get_return()
	{
		return($this->_return);
	}

	function get_return_var($var)
	{
		$var = (string) $var;

		if(is_array($this->_return) === false
		|| isset($this->_return[$var]) === false)
			return(null);

		return($this->_return[$var]);
	}

	function _set_error($name,$value=false)
	{
		$name = (string) $name;
		$this->_error[$name] = $value;
		$this->_errnb++;
	}

	function get_error($name='')
	{
		$name = (string) $name;

		if($name === '')
			return($this->_error);

		if(isset($this->_error[$name]) === true)
			return($this->_error[$name]);

		return(null);
	}

	function get_errnb()
	{
		return($this->_errnb);
	}
}

?>
