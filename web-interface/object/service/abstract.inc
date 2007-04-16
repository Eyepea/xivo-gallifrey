<?php

class xivo_service_abstract
{
	var $_conf	= array();
	var $_module	= array();

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
		$r = false;

		$type = (string) $type;
		$name = (string) $name;

		if(xivo_issa($type,$this->_conf) === true && xivo_ak($name,$this->_conf[$type]) === true)
			$r = true;

		return($r);
	}

	function _load_class($module)
	{
		$r = false;

		$module = (string) $module;

		$file = $module.'.inc';

		$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.$this->_type.XIVO_SEP_DIR.$this->_name.XIVO_SEP_DIR.$module;
		$class = 'xivo_service_'.$this->_name.'_'.$module;

		if(xivo_file::load_file($file,$dir,true) === false)
			return($r);

		if(class_exists($class) === true)
			$r = $class;

		return($r);
	}

	function _load_config()
	{
		if(empty($this->_conf) === true)
		{
			$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.$this->_type.XIVO_SEP_DIR.$this->_name;
	
			if(($this->_conf = xivo_gat::load_get('config',$dir,$this->_name)) === false)
				trigger_error('Failed to load '.$this->_name.' configuration',E_USER_ERROR);
		}
	}

	function &call_module($name,$func,$args=null)
	{
		$name = (string) $name;
		$func = (string) $func;

		if($this->_load_module($name) === false)
			return(false);

		if($args === null)
			$r = call_user_func(array(&$this->_module[$name],$func));
		else
		{
			$args = (array) $args;
			$r = call_user_func_array(array(&$this->_module[$name],$func),$args);
		}

		return(($ref = &$r));
	}

	function &get_module($name)
	{
		$r = false;

		$name = (string) $name;

		if($this->_load_module($name) === true)
			$r = &$this->_module[$name];

		return(($ref = &$r));
	}

	function chk_module_enable($name)
	{
		if($this->_chk_exists_cfg('module',$name) === false)
			return(false);

		$module = &$this->_conf['module'][$name];

		if(isset($module['section'],$this->_conf['section'][$module['section']]) === true)
		{
			$ref = &$this->_conf['section'][$module['section']];
			$section = $module['section'];

			if(isset($ref['disabled']) === false || $ref['disabled'] === false)
				return(true);

			if(xivo_issa('section',$this->_ini['general']) === false)
				return(true);

			if((bool) xivo_ak($section,$this->_ini['general']['section'],true) === false)
				return(false);
		}
		else
		{
			if(isset($module['disabled']) === false || $module['disabled'] === false)
				return(true);

			if(xivo_issa($name,$this->_ini) === false || isset($this->_ini[$name]['disable']) === false)
				return(true);

			return((bool) $this->_ini[$name]['disable']);
		}

		return(true);
	}

	function _load_module($name)
	{
		$r = false;

		$name = (string) $name;

		if($this->_chk_exists_cfg('module',$name) === false)
			trigger_error('Invalid module in '.$name.' '.$this->_name,E_USER_ERROR);

		$ref = &$this->_conf['module'][$name];

		if(isset($this->_module[$name]) === true)
			return(true);
		
		if($this->chk_module_enable($name) === false)
			return(false);

		if(($class = $this->_load_class($name)) !== false)
		{
			if(isset($ref['datastorage']) === true && (bool) $ref['datastorage'] === true)
			{
				if(is_string($ref['datastorage']) === true && $this->chk_module_enable($ref['datastorage']) === true)
					$this->_module[$name] = new $class($this,$this->_load_dso($ref['datastorage']));
				else
					$this->_module[$name] = new $class($this,$this->_load_dso($name));
			}
			else
				$this->_module[$name] = new $class($this);
		}
		else
		{
			trigger_error('Module '.$name.' does not exist in '.$this->_name,E_USER_ERROR);
			return($r);
		}

		return(true);
	}

	function _load_dso($name)
	{
		$r = false;

		$name = (string) $name;

		if(xivo_issa($name,$this->_ini) === false || xivo_issa('datastorage',$this->_ini[$name]) === false)
			trigger_error('Invalid or missing datastorage '.$name.' in '.$this->_type.' ini file',E_USER_ERROR);

		$datastorage = &$this->_ini[$name]['datastorage'];

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

				$r = new $class($dso,$datastorage);
				break;
			default:
				$class = 'xivo_service_'.$this->_name.'_'.$name.'_'.$type;

				$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.'service'.
				       XIVO_SEP_DIR.$this->_type.XIVO_SEP_DIR.$this->_name.
				       XIVO_SEP_DIR.$name.XIVO_SEP_DIR.'datastorage';

				if(xivo::load_class($class,$dir,$type,false) === false)
					trigger_error('Failed to load datastorage '.$type.' for '.$name,E_USER_ERROR);

				if(xivo_datastorage::is_valid($type) === true)
					$r = new $class(xivo_datastorage::factory($type,$datastorage),$datastorage);
				else
					trigger_error('Invalid datastorage '.$type.' for '.$name,E_USER_ERROR);
		}

		return($r);
	}
}

?>
