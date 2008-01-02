<?php

class xivo_service
{
	var $_ini	= array();
	var $_conf	= array();
	var $_uri	= null;

	function xivo_service($ini)
	{
		if(($this->_conf = xivo_gat::load_get('service',XIVO_PATH_OBJECTCONF)) === false)
			trigger_error('Failed to load service Array',E_USER_ERROR);	
		else if(($arr = xivo_get_aks($ini)) === false)
			trigger_error('Invalid parameter in '.__CLASS__,E_USER_ERROR);

		$this->_ini = $ini;

		if(xivo_issa('_SRE',$_SESSION) === false)
			$_SESSION['_SRE'] = array();

		for($i = 0;$i < $arr['cnt'];$i++)
			$this->_load_service($arr['keys'][$i]);
	}

	function _load_service($name)
	{
		if($this->is_valid($name) === true)
			return(null);

		if(xivo_issa($name,$this->_conf) === false
		|| ($init = $this->_load_init_file($name)) === false
		|| xivo_issa('general',$init) === false
		|| isset($init['general']['name'],$this->_conf[$name][$init['general']['name']]) === false)
			return(false);

		$sname = $init['general']['name'];
		$cname = 'xivo_service_'.$sname;
		$name_up = strtoupper($name);

		$label = 'XIVO_SRE_'.$name_up.'_LABEL';
		$version = 'XIVO_SRE_'.$name_up.'_VERSION';

		if(isset($_SESSION['_SRE'][$name]) === false)
			$_SESSION['_SRE'][$name] = array();

		if(isset($_SESSION['_SRE'][$name][$sname]) === false)
			$_SESSION['_SRE'][$name][$sname] = array();

		if(isset($_SESSION['_SRE'][$name][$sname]['ini']) === false)
			$_SESSION['_SRE'][$name][$sname]['ini'] = array('general' => array());

		if(isset($_SESSION['_SRE'][$name][$sname]['infos']) === false)
			$_SESSION['_SRE'][$name][$sname]['infos'] = array();

		$sess_ini = &$_SESSION['_SRE'][$name][$sname]['ini'];

		if(isset($init['general']['label']) === true && is_string($init['general']['label']) === true)
			$sess_ini['general']['label'] = $init['general']['label'];

		if(isset($init['general']['version']) === true && is_string($init['general']['version']) === true)
			$sess_ini['general']['version'] = $init['general']['version'];

		$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.$name;

		if(xivo::load_class($cname,$dir,$sname,false) === false)
			trigger_error('Failed to load '.$name.' '.$sname.' service',E_USER_ERROR);

		if(isset($init['general']['datastorage']) === true)
		{
			if(is_string($init['general']['datastorage']) === true)
				$init['general']['datastorage'] = xivo_datastorage::set_param_from_uri($init['general']['datastorage']);
			else if(isset($init['general']['datastorage']['type']) === false)
				$init['general']['datastorage'] = false;

			if($init['general']['datastorage'] === false)
				trigger_error('Invalid datastorage in '.$name.' ini file',E_USER_ERROR);
		}

		$this->_set($name,new $cname($init));
	}

	function &call_module($service,$module,$func,$args=null)
	{
		$service = (string) $service;
		$module = (string) $module;
		$func = (string) $func;
	
		if(($class = &$this->get($service)) === false)
			trigger_error('Service not found, not loaded or installed',E_USER_ERROR);

		return(($ref = &$class->call($module,$func,$args)));
	}

	function &get_module($service,$module)
	{
		$service = (string) $service;
		$module = (string) $module;
	
		if(($class = &$this->get($service)) === false)
			trigger_error('Service not found, not loaded or installed',E_USER_ERROR);	

		return(($ref = &$class->get_module($module)));
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
		return(($ref = &xivo_gct::get(XIVO_SRE_CLASS_PRE.$name)));
	}

	function &_set_get($name,&$obj)
	{
		$r = false;

		if($name !== '')
			$r = &xivo_gct::set_get($obj,XIVO_SRE_CLASS_PRE.$name);

		return(($ref = &$r));
	}

	function is_valid($name)
	{
		return(xivo_gct::is_valid(XIVO_SRE_CLASS_PRE.$name));
	}
}

?>
