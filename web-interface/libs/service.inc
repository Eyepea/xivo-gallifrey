<?php

class xivo_service
{
	var $_ini	= array();
	var $_conf	= array();

	function xivo_service($ini)
	{
		if(($this->_conf = xivo_gat::load_get('service',XIVO_PATH_APPCONF)) === false)
			trigger_error('Failed to load service Array',E_USER_ERROR);	

		if(($arr = xivo_get_aks($ini)) === false)
			trigger_error('Invalid parameter in '.__CLASS__,E_USER_ERROR);

		$this->_ini = $ini;

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = $arr['keys'][$i];
			$v = (bool) $ini[$k];

			$this->_load_service($k);
		}
	}

	function _load_service($name)
	{
		$r = false;

		if($this->is_valid($name) === true)
			return(null);

		if(xivo_issa($name,$this->_conf) === false
		|| ($init = $this->_load_init_file($name)) === false
		|| xivo_issa('general',$init) === false)
			return($r);

		if(isset($init['general']['name'],$this->_conf[$name][$init['general']['name']]) === false)
			return($r);

		$sname = $init['general']['name'];
		$cname = 'xivo_service_'.$sname;
		$name_up = strtoupper($name);

		$label = 'XIVO_SRE_'.$name_up.'_LABEL';
		$version = 'XIVO_SRE_'.$name_up.'_VERSION';

		if(isset($init['general']['label']) === true && is_string($init['general']['label']) === true)
			define($label,$init['general']['label']);
		else
			define($label,'Undefined');

		if(isset($init['general']['version']) === true && is_string($init['general']['version']) === true)
			define($version,$init['general']['version']);
		else
			define($version,'Undefined');

		if(xivo::load_class($cname,'service::'.$name,$sname) === false)
			trigger_error('Failed to load '.$name.' '.$sname.' service',E_USER_ERROR);

		$this->_set($name,new $cname($init));
	}

	function &call_module($service,$module,$func,$args=null)
	{
		$service = (string) $service;
		$module = (string) $module;
		$func = (string) $func;
	
		if(($class = &$this->get($service)) === false)
			trigger_error('Service not found, not loaded or installed',E_USER_ERROR);

		return($class->call($module,$func,$args));
	}

	function &get_module($service,$module)
	{
		$service = (string) $service;
		$module = (string) $module;
	
		if(($class = &$this->get($service)) === false)
			trigger_error('Service not found, not loaded or installed',E_USER_ERROR);	

		return($class->get_module($module));
	}

	function _load_init_file($name)
	{
		$name = (string) $name;

		if(preg_match('/^[a-z0-9\-_]+$/i',$name) !== 1)
			return(false);

		return(xivo::load_init(XIVO_PATH_CONF.XIVO_SEP_DIR.$name.'.ini'));
	}

	function _set($name,&$obj)
	{
		$r = false;

		if($name === '' || is_object($obj) === false)
			return($r);

		return(xivo_gct::set($obj,XIVO_SRE_CLASS_PRE.$name));
	}

	function &get($name)
	{
		return(xivo_gct::get(XIVO_SRE_CLASS_PRE.$name));
	}

	function &_set_get($name,&$obj)
	{
		$r = false;

		if($name === '')
			return($r);

		return(xivo_gct::set_get($obj,XIVO_SRE_CLASS_PRE.$name));
	}

	function is_valid($name)
	{
		return(xivo_gct::is_valid(XIVO_SRE_CLASS_PRE.$name));
	}
}

?>
