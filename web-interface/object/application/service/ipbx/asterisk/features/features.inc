<?php

class xivo_application_service_asterisk_features
{
	var $_service	= null;
	var $_module	= array(
				'general'		=> false,
				'featuremap'		=> false,
				'applicationmap'	=> false);

	function xivo_application_service_asterisk_features(&$service)
	{
		if(is_object($service) === false || get_class($service) !== 'xivo_service_asterisk')
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_service = &$service;
	}

	function _load_module_class($name)
	{
		$name = (string) $name;

		if($this->is_valid($name) === false)
			return(false);

		$file = $name.'.inc';
		$dir = dirname(__FILE__).XIVO_SEP_DIR.$name;
		$class = __CLASS__.'_'.$name;

		if(xivo_file::load_file($file,$dir,true) !== false && class_exists($class) === true)
			return($class);

		return(false);
	}

	function _load_module($name)
	{
		$name = (string) $name;

		if($this->is_valid($name) === false)
			return(false);

		if(isset($this->_module[$name]) === true && is_object($this->_module[$name]) === true)
			return(true);
		
		if(($class = $this->_load_module_class($name)) === false)
		{
			trigger_error('Application\'s module '.$name.' does not exist',E_USER_ERROR);
			return(false);
		}

		$this->_module[$name] = new $class($this->_service);

		return(true);
	}

	function &get_module($name)
	{
		$r = false;

		$name = (string) $name;

		if($this->_load_module($name) === true)
			$r = &$this->_module[$name];

		return(($ref = &$r));
	}

	function is_valid($name)
	{
		$name = (string) $name;

		return(isset($this->_module[$name]));
	}
}

?>
