<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'application'.XIVO_SEP_DIR.'abstract.inc');

class xivo_application_service_abstract extends xivo_application_abstract
{
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
		else if(($class = $this->_load_abstract_class($name)) === false)
		{
			trigger_error('Abstract Application '.$name.' does not exist in '.$this->_service_name,E_USER_ERROR);
			return(false);
		}

		$this->_abstract[$name] = new $class($this->_service);

		return(true);
	}
}

?>
