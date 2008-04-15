<?php

class xivo_application_helper
{
	var $_errnb	= 0;
	var $_error	= array();
	var $_info	= array();
	var $_origin	= array();
	var $_result	= array();
	var $_return	= array();

	function load_init($param=null)
	{
		$this->_reset_info();
		$this->_reset_params();

		if(is_array($param) === true)
			$this->set_params($param);

		return(null);
	}

	function _reset_params()
	{
		if(isset($this->_param) === false)
			return(false);

		$vars = get_class_vars(get_class($this));

		if(isset($vars['param']) === true)
			$this->_param = $vars['param'];

		return(true);
	}

	function _reset_info()
	{
		$this->_info = array();
		$this->_origin = array();
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

	function get_origin()
	{
		return($this->_origin);
	}

	function get_origin_var($var)
	{
		$var = (string) $var;

		if(is_array($this->_origin) === false
		|| isset($this->_origin[$var]) === false)
			return(null);

		return($this->_origin[$var]);
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

	function _set_result($value)
	{
		$this->_result = $value;
	}

	function _set_result_var($var,$value)
	{
		$var = (string) $var;

		$this->_result[$var] = $value;
	}

	function get_result()
	{
		if(empty($this->_result) === true)
			return(null);

		return($this->_result);
	}

	function get_result_var($var)
	{
		$var = (string) $var;

		if(is_array($this->_result) === false
		|| isset($this->_result[$var]) === false)
			return(null);

		return($this->_result[$var]);
	}

	function _set_error($value=false)
	{
		$this->_error = $value;
		$this->_errnb++;
	}

	function get_error()
	{
		return($this->_error);
	}

	function get_errnb()
	{
		return($this->_errnb);
	}

	function set_params($params)
	{
		if(is_array($params) === false
		|| empty($params) === true
		|| isset($this->_param) === false
		|| is_array($this->_param) === false)
			return(false);

		foreach($params as $key => $value)
		{
			if(isset($this->_param[$key]) === true)
				$this->_param[$key] = $value;
		}

		return(true);
	}

	function get_params()
	{
		if(isset($this->_param) === false)
			return(false);

		return($this->_param);
	}

	function set_param($name,$value)
	{
		if(isset($this->_param) === false
		|| is_array($this->_param) === false
		|| isset($this->_param[$name]) === false)
			return(false);

		$this->_param[$name] = $value;

		return(true);
	}

	function get_param($name)
	{
		if(isset($this->_param) === false
		|| is_array($this->_param) === false
		|| isset($this->_param[$name]) === false)
			return(false);

		return($this->_param[$name]);
	}
}

?>
