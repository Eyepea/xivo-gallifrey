<?php

class xivo_application_abstract
{
	var $_errnb	= 0;
	var $_error	= array();
	var $_result	= array();
	var $_return	= array();
	var $_info	= array();

	function _reset()
	{
		$this->_errnb = 0;
		$this->_error = array();
		$this->_result = array();
		$this->_return = array();
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
