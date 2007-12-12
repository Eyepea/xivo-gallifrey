<?php

class xivo_service_abstract
{
	var $_conf		= array();
	var $_module		= array();
	var $_application	= array();
	var $_appcustom		= array();
	var $_dsogeneral	= null;
	var $_dsocustom		= array();
	var $_dsoconf		= array();

	function get_name()
	{
		return($this->_name);
	}

	function get_type()
	{
		return($this->_type);
	}

	function _chk_exists_cfg($type,$name,$ret=false)
	{
		$r = false;

		$type = (string) $type;
		$name = (string) $name;
		$ret = (bool) $ret;

		if(isset($this->_conf) === true
		&& xivo_issa($type,$this->_conf) === true
		&& isset($this->_conf[$type][$name]) === true)
			$r = $ret === true ? $this->_conf[$type][$name] : true;

		return($r);
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
	
		$dir = array();
		$dir[] = XIVO_PATH_OBJECT;
		$dir[] = 'service';
		$dir[] = $this->_type;
		$dir[] = $this->_name;
		$dir[] = 'config';

		if(($this->_conf = xivo_gat::load_get('config',$dir,$this->_name)) === false)
			trigger_error('Failed to load '.$this->_name.' configuration',E_USER_ERROR);
	}

	function _load_dso_config($type)
	{
		if(isset($this->_dsoconf[$type]) === true)
			return(true);

		$dir = array();
		$dir[] = XIVO_PATH_OBJECT;
		$dir[] = 'service';
		$dir[] = $this->_type;
		$dir[] = $this->_name;
		$dir[] = 'config';
		$dir[] = 'datastorage';
		$dir[] = $type;

		if(($this->_dsoconf[$type] = xivo_gat::get_file($dir)) === false)
		{
			trigger_error('Failed to load '.$type.' datastorage configuration for '.$this->_name,E_USER_ERROR);
			return(false);
		}

		return(true);
	}

	function get_dso_config_by_name($type,$name)
	{
		if($this->_load_dso_config($type) === true
		&& isset($this->_dsoconf[$type][$name]) === true
		&& is_array($this->_dsoconf[$type][$name]) === true)
			return($this->_dsoconf[$type][$name]);

		return(false);
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
		else if($this->chk_module_enable($name) === false)
			return(false);
		else if(($class = $this->_load_module_class($name)) === false)
		{
			trigger_error('Module '.$name.' does not exist in '.$this->_name,E_USER_ERROR);
			return(false);
		}
		else if(isset($ref['datastorage']) === false || (bool) $ref['datastorage'] === false)
			$this->_module[$name] = new $class($this,$param);
		else
		{
			$custom = (bool) xivo_ak('custom',$ref['datastorage'],true);
			$this->_module[$name] = new $class($this,$this->_load_dso($name,$custom),$param);
		}

		return(true);
	}

	function _load_application($name,$param=null)
	{
		$name = (string) $name;

		if(isset($this->_application[$name]) === true)
		{
			if(is_array($param) === true)
				$this->_application[$name]->set_params($param);
			return(true);
		}
		else if(($class = $this->_load_application_class($name)) === false)
		{
			trigger_error('Application '.$name.' does not exist in '.$this->_name,E_USER_ERROR);
			return(false);
		}
		else if(($dsoclass = $this->_load_dso_application($name)) !== false)
			$this->_application[$name] = new $class($this,$dsoclass,$param);
		else
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
		else if($this->chk_appcustom_enable($type,$name) === false)
			return(false);
		else if(($class = $this->_load_appcustom_class($type,$name)) === false)
		{
			trigger_error('Custom Application '.$name.' does not exist in '.$this->_name,E_USER_ERROR);
			return(false);
		}

		$this->_appcustom[$type][$name] = new $class($this);

		return(true);
	}

	function _load_dso_application($name)
	{
		if(($conf = $this->_chk_exists_cfg('application',$name,true)) === true
		&& isset($conf['datastorage']) === true
		&& $this->_load_module($conf['datastorage']) !== false
		&& isset($this->_dsocustom[$conf['datastorage']]) === true)
		{
			if(is_object($this->_dsocustom[$conf['datastorage']]) === true)
				$dso = &$this->_dsocustom[$conf['datastorage']];
			else
			{
				trigger_error('Invalid custom Datastorage in '.$this->_name.' for '.$name,E_USER_ERROR);
				return(false);
			}
		}
		else if(is_object($this->_dsogeneral) === true)
			$dso = &$this->_dsogeneral;
		else if($this->_load_dso() === true && is_object($this->_dsogeneral) === true)
			$dso = &$this->_dsogeneral;
		else
		{
			trigger_error('Missing general Datastorage in '.$this->_name.' for '.$name,E_USER_ERROR);
			return(false);
		}

		$type = $dso->get_type();

		$class = 'xivo_application_service_'.$this->_name.'_'.$name.'_'.$type;

		$dir = array();
		$dir[] = XIVO_PATH_OBJECT;
		$dir[] = 'application';
		$dir[] = 'service';
		$dir[] = $this->_type;
		$dir[] = $this->_name;
		$dir[] = $name;
		$dir[] = 'datastorage';

		if($this->_load_dso_config($type) === false || xivo::load_class($class,$dir,$type,false) === false)
			return(false);

		return(new $class($dso,&$this->_dsoconf[$type]));
	}

	function _load_dso($name='',$custom=false)
	{
		$name = (string) $name;
		$custom = (bool) $custom;
		$ini = $gendatastorage = $datastorage = $datastoragemod = false;

		if($name !== '' && xivo_issa($name,$this->_ini) === true)
			$ini = $this->_ini[$name];

		if(isset($this->_ini['general']['datastorage']) === true)
			$datastorage = $gendatastorage = $this->_ini['general']['datastorage'];

		if($name === '')
		{
			if($datastorage === false || isset($datastorage['type']) === false)
				return(false);
			else if(xivo_datastorage::is_valid($datastorage['type']) === false)
			{
				trigger_error('Invalid datastorage '.$datastorage['type'],E_USER_ERROR);
				return(false);
			}

			$this->_load_dso_config($datastorage['type']);

			if(is_object($this->_dsogeneral) === false)
				$this->_dsogeneral = xivo_datastorage::factory($datastorage['type'],$datastorage);

			return(true);
		}

		if($custom === false && $gendatastorage === false)
			trigger_error('Missing general datastorage in '.$this->_type.' ini file',E_USER_ERROR);

		if($ini === false && $gendatastorage === false)
			trigger_error('Invalid or missing '.$name.' datastorage in '.$this->_type.' ini file',E_USER_ERROR);

		if($ini !== false && isset($ini['datastorage']) === true)
		{
			if(is_string($ini['datastorage']) === true)
				$datastoragemod = xivo_datastorage::set_param_from_uri($ini['datastorage']);
			else
				$datastoragemod = $ini['datastorage'];

			if($custom === true)
				$datastorage = $datastoragemod;
		}

		if($datastorage === false)
			trigger_error('Invalid or missing '.$name.' datastorage in '.$this->_type.' ini file',E_USER_ERROR);
		else if(isset($datastorage['type']) === false)
			trigger_error('Missing datastorage type in '.$this->_type.' ini file for '.$name,E_USER_ERROR);
		else if(xivo_datastorage::is_valid($datastorage['type']) === false)
			trigger_error('Invalid datastorage '.$datastorage['type'].' for '.$name,E_USER_ERROR);

		$type = $datastorage['type'];

		$class = 'xivo_service_'.$this->_name.'_'.$name.'_'.$type;

		$dir = array();
		$dir[] = XIVO_PATH_OBJECT;
		$dir[] = 'service';
		$dir[] = $this->_type;
		$dir[] = $this->_name;
		$dir[] = $name;
		$dir[] = 'datastorage';

		if(($param = $this->get_dso_config_by_name($type,$name)) === false)
			$param = array();

		if(is_array($datastoragemod) === true)
			$param = array_merge($datastoragemod,$param);

		if(xivo::load_class($class,$dir,$type,false) === false)
		{
			trigger_error('Failed to load datastorage '.$type.' for '.$name,E_USER_ERROR);
			return(false);
		}
		else if($custom === false)
		{
			if(is_object($this->_dsogeneral) === false)
				$this->_dsogeneral = xivo_datastorage::factory($type,$datastorage);

			return(new $class($this->_dsogeneral,$param));
		}
		else
		{
			if(isset($this->_dsocustom[$name]) === false
			|| is_object($this->_dsocustom[$name]) === false)
				$this->_dsocustom[$name] = xivo_datastorage::factory($type,$datastorage);

			return(new $class($this->_dsocustom[$name],$param));
		}

		return(false);
	}
}

?>
