<?php

class xivo_service_abstract
{
	var $_conf		= array();
	var $_module		= array();
	var $_application	= array();
	var $_appcustom		= array();

	function get_name()
	{
		return($this->_name);
	}

	function get_type()
	{
		return($this->_type);
	}

	function _chk_exists_cfg($type,$name)
	{
		$type = (string) $type;
		$name = (string) $name;

		if(xivo_issa($type,$this->_conf) === true && xivo_ak($name,$this->_conf[$type]) === true)
			return(true);

		return(false);
	}

	function _load_module_class($module)
	{
		$module = (string) $module;

		$file = $module.'.inc';

		$dir = array(XIVO_PATH_OBJECT,'service',$this->_type,$this->_name,$module);

		$class = 'xivo_service_'.$this->_name.'_'.$module;

		if(xivo_file::load_file($file,$dir,true) !== false && class_exists($class) === true)
			return($class);

		return(false);
	}

	function _load_application_class($application)
	{
		$application = (string) $application;

		$file = $application.'.inc';

		$dir = array(XIVO_PATH_OBJECT,'application','service',$this->_type,$this->_name,$application);

		$class = 'xivo_application_service_'.$this->_name.'_'.$application;

		if(xivo_file::load_file($file,$dir,true) !== false && class_exists($class) === true)
			return($class);

		return(false);
	}

	function _load_appcustom_class($type,$name)
	{
		$type = (string) $type;
		$name = (string) $name;

		$file = $name.'.inc';

		$dir = array(XIVO_PATH_OBJECT,'application','service',$this->_type,$this->_name,$type,$name);

		$class = 'xivo_application_service_'.$this->_name.'_'.$type.'_'.$name;

		if(xivo_file::load_file($file,$dir,true) !== false && class_exists($class) === true)
			return($class);

		return(false);
	}

	function _load_config()
	{
		if(empty($this->_conf) === false)
			return(null);
	
		$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.
		       'service'.XIVO_SEP_DIR.
		       $this->_type.XIVO_SEP_DIR.
		       $this->_name;
	
		if(($this->_conf = xivo_gat::load_get('config',$dir,$this->_name)) === false)
			trigger_error('Failed to load '.$this->_name.' configuration',E_USER_ERROR);
	}

	function &call_module($name,$func,$args=null)
	{
		$name = (string) $name;
		$func = (string) $func;

		if($this->_load_module($name) === false)
			$r = false;
		else if($args === null)
			$r = call_user_func(array(&$this->_module[$name],$func));
		else
		{
			$args = (array) $args;
			$r = call_user_func_array(array(&$this->_module[$name],$func),$args);
		}

		return(($ref = &$r));
	}

	function &call_application($name,$func,$args=null)
	{
		$name = (string) $name;
		$func = (string) $func;

		if($this->_load_application($name) === false)
			$r = false;
		else if($args === null)
			$r = call_user_func(array(&$this->_application[$name],$func));
		else
		{
			$args = (array) $args;
			$r = call_user_func_array(array(&$this->_application[$name],$func),$args);
		}

		return(($ref = &$r));
	}

	function &call_appcustom($type,$name,$func,$args=null)
	{
		$type = (string) $type;
		$name = (string) $name;
		$func = (string) $func;

		if($this->_load_appcustom($type,$name) === false)
			$r = false;
		else if($args === null)
			$r = call_user_func(array(&$this->_appcustom[$type][$name],$func));
		else
		{
			$args = (array) $args;
			$r = call_user_func_array(array(&$this->_appcustom[$type][$name],$func),$args);
		}

		return(($ref = &$r));
	}

	function &get_module($name,$param=null)
	{
		$r = false;

		$name = (string) $name;

		if($this->_load_module($name,$param) === true)
			$r = &$this->_module[$name];

		return(($ref = &$r));
	}

	function &get_application($name,$param=null)
	{
		$r = false;

		$name = (string) $name;

		if($this->_load_application($name,$param) === true)
			$r = &$this->_application[$name];

		return(($ref = &$r));
	}

	function &get_appcustom($type,$name)
	{
		$r = false;

		$type = (string) $type;
		$name = (string) $name;

		if($this->_load_appcustom($type,$name) === true)
			$r = &$this->_appcustom[$type][$name];

		return(($ref = &$r));
	}

	function chk_module_enable($name)
	{
		return($this->_chk_exists_cfg('module',$name));
	}

	function chk_appcustom_enable($type,$name)
	{
		if($this->_chk_exists_cfg('appcustom',$type) === true
		&& isset($this->_conf['appcustom'][$type][$name]) === true)
			return(true);

		return(false);
	}

	function _load_module($name,$param=null)
	{
		$name = (string) $name;

		if($this->_chk_exists_cfg('module',$name) === false)
		{
			trigger_error('Invalid module in '.$name.' '.$this->_name,E_USER_ERROR);
			return(false);
		}

		$ref = &$this->_conf['module'][$name];

		if(isset($this->_module[$name]) === true)
			return(true);
		
		if($this->chk_module_enable($name) === false)
			return(false);

		if(($class = $this->_load_module_class($name)) === false)
		{
			trigger_error('Module '.$name.' does not exist in '.$this->_name,E_USER_ERROR);
			return(false);
		}

		if(isset($ref['datastorage']) === false || (bool) $ref['datastorage'] === false)
		{
			$this->_module[$name] = new $class($this,$param);
			return(true);
		}

		if(is_string($ref['datastorage']) === true && $this->chk_module_enable($ref['datastorage']) === true)
			$this->_module[$name] = new $class($this,$this->_load_dso($ref['datastorage']),$param);
		else
			$this->_module[$name] = new $class($this,$this->_load_dso($name),$param);

		return(true);
	}

	function _load_application($name,$param=null)
	{
		$name = (string) $name;

		if(isset($this->_application[$name]) === true)
			return(true);
		
		if(($class = $this->_load_application_class($name)) === false)
		{
			trigger_error('Application '.$name.' does not exist in '.$this->_name,E_USER_ERROR);
			return(false);
		}

		$this->_application[$name] = new $class($this,$param);

		return(true);
	}

	function _load_appcustom($type,$name)
	{
		$type = (string) $type;
		$name = (string) $name;

		if(xivo_issa($this->_appcustom,$type) === true
		&& isset($this->_appcustom[$type][$name]) === true)
			return(true);
		
		if($this->chk_appcustom_enable($type,$name) === false)
			return(false);

		if(($class = $this->_load_appcustom_class($type,$name)) === false)
		{
			trigger_error('Custom Application '.$name.' does not exist in '.$this->_name,E_USER_ERROR);
			return(false);
		}

		$this->_appcustom[$type][$name] = new $class($this);

		return(true);
	}

	function _load_dso($name)
	{
		$r = false;

		$name = (string) $name;
		$ini = $datastorage = false;

		if(xivo_issa($name,$this->_ini) === true)
			$ini = $this->_ini[$name];

		if(isset($this->_ini['general']['datastorage']) === true)
			$datastorage = $this->_ini['general']['datastorage'];

		if($ini === false && $datastorage === false)
			trigger_error('Invalid or missing datastorage '.$name.' in '.$this->_type.' ini file',E_USER_ERROR);

		if($ini !== false && isset($ini['datastorage']) === true)
		{
			if(is_string($ini['datastorage']) === true)
				$datastorage = xivo_datastorage::set_param_from_uri($ini['datastorage']);
			else
				$datastorage = $ini['datastorage'];
		}

		if($datastorage === false)
			trigger_error('Invalid or missing datastorage '.$name.' in '.$this->_type.' ini file',E_USER_ERROR);

		if(isset($datastorage['type']) === false)
			trigger_error('Missing datastorage type '.$name.' in '.$this->_type.' ini file',E_USER_ERROR);

		$type = $datastorage['type'];

		switch($type)
		{
			case 'internal':
				$dso = &xivo_gct::get('XIVO_DSO');
				$type = $dso->get_type();
				$class = 'xivo_service_'.$this->_name.'_'.$name.'_'.$type;

				$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.'service'.
				       XIVO_SEP_DIR.$this->_type.XIVO_SEP_DIR.$this->_name.
				       XIVO_SEP_DIR.$name.XIVO_SEP_DIR.'datastorage';

				if(xivo::load_class($class,$dir,$type,false) === false)
					trigger_error('Failed to load datastorage '.$type.' for '.$name,E_USER_ERROR);
				else
					$r = new $class($dso,$datastorage);
				break;
			default:
				$class = 'xivo_service_'.$this->_name.'_'.$name.'_'.$type;

				$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.'service'.
				       XIVO_SEP_DIR.$this->_type.XIVO_SEP_DIR.$this->_name.
				       XIVO_SEP_DIR.$name.XIVO_SEP_DIR.'datastorage';

				if(xivo::load_class($class,$dir,$type,false) === false)
				{
					trigger_error('Failed to load datastorage '.$type.' for '.$name,E_USER_ERROR);
					return($r);
				}

				if(xivo_datastorage::is_valid($type) === true)
					$r = new $class(xivo_datastorage::factory($type,$datastorage),$datastorage);
				else
					trigger_error('Invalid datastorage '.$type.' for '.$name,E_USER_ERROR);
		}

		return($r);
	}
}

?>
