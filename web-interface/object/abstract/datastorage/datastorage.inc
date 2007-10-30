<?php

class xivo_abstract_datastorage
{
	var $_dso	= null;

	function _init(&$dso)
	{
		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;
	}

	function set_param($name,$value)
	{
		$name = (string) $name;

		$this->_param[$name] = $value;	

		return(true);
	}

	function get_param($name)
	{
		$name = (string) $name;

		if(isset($this->_param[$name]) === true)
			return($this->_param[$name]);
		else
			return(null);
	}
}

?>
