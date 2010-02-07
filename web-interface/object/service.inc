<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

class xivo_service
{
	var $_ini	= array();
	var $_services	= array();
	var $_config	= array();
	var $_enginecfg	= array();

	function xivo_service($ini=null)
	{
		if(($this->_services = dwho_gat::get_file(XIVO_PATH_OBJECTCONF.DWHO_SEP_DIR.'service')) === false)
			trigger_error('Failed to load service Array',E_USER_ERROR);
		else if(is_array($ini) === true && empty($ini) === false)
			$this->_ini = $ini;

		if(dwho_issa('_SRE',$_SESSION) === false)
			$_SESSION['_SRE'] = array();
	}

	function get_engines($service)
	{
		if(dwho_issa($service,$this->_services) === true)
			return($this->_services[$service]);

		return(false);
	}

	function is_valid_engine($service,$engine)
	{
		if(dwho_issa($service,$this->_services) === true
		&& isset($this->_services[$service][$engine]) === true)
			return(true);

		return(false);
	}

	function get_init_file($service)
	{
		if(preg_match('/^[a-zA-Z0-9\-_]+$/',$service) !== 1)
			return(false);

		return(xivo::load_init(XIVO_PATH_CONF.DWHO_SEP_DIR.$service.'.ini'));
	}

	function get_engine_config($service,$engine)
	{
		if($this->is_valid_engine($service,$engine) === false)
			return(false);

		if(isset($this->_enginecfg[$service]) === false)
			$this->_enginecfg[$service] = array();

		if(isset($this->_enginecfg[$service][$engine]) === false)
			$this->_enginecfg[$service][$engine] = $this->_services[$service][$engine]['ini'];

		return($this->_enginecfg[$service][$engine]);
	}

	function set_engine_config($service,$engine,$config)
	{
		if($this->get_engine_config($service,$engine) === false
		|| is_array($config) === false)
			return(false);

		$this->_enginecfg[$service][$engine] = dwho_array_merge_replace_recursive(
							$this->_enginecfg[$service][$engine],
							$config);
		return(true);
	}

	function get_service_config($service,$engine=null)
	{
		$init = $this->get_init_file($service);

		if(dwho_has_len($engine) === false)
		{
			if(dwho_issa('general',$init) === false
			|| isset($init['general']['name']) === false
			|| $this->is_valid_engine($service,$init['general']['name']) === false)
				return(false);

			$engine = $init['general']['name'];
		}

		if(is_array($init) === false)
			return($this->get_engine_config($service,$engine));

		return(dwho_array_merge_replace_recursive(
				$this->get_engine_config($service,$engine),
				$init));
	}

	function &get_module($service,$module,$engine=null)
	{
		if(($class = &$this->get($service,$engine)) === false)
			trigger_error('Service not found or not installed',E_USER_ERROR);

		return(($ref = &$class->get_module($module)));
	}

	function _set($service,&$obj)
	{
		if(dwho_has_len($service) === false || is_object($obj) === false)
			return(false);

		return(dwho_gct::set($obj,XIVO_SRE_CLASS_PRE.$service));
	}

	function &get($service,$engine=null)
	{
		$r = false;

		if($this->_load_service($service,$engine) !== false)
			$r = &dwho_gct::get(XIVO_SRE_CLASS_PRE.$service);

		return(($ref = &$r));
	}

	function &_set_get($service,&$obj)
	{
		$r = false;

		if(dwho_has_len($service) === true)
			$r = &dwho_gct::set_get($obj,XIVO_SRE_CLASS_PRE.$service);

		return(($ref = &$r));
	}

	function is_loaded($service)
	{
		return(dwho_gct::is_valid(XIVO_SRE_CLASS_PRE.$service));
	}

	function _load_service($service,$engine=null)
	{
		if($this->is_loaded($service) === true)
			return(null);
		else if((bool) dwho_ak($service,$this->_ini,true) === false
		|| ($config = $this->get_service_config($service,$engine)) === false)
			return(false);

		if(dwho_has_len($engine) === false)
			$enginename = $config['general']['name'];
		else
			$enginename = $engine;

		$classname = 'xivo_service_'.$enginename;

		if(isset($_SESSION['_SRE'][$service]) === false)
			$_SESSION['_SRE'][$service] = array();

		if(isset($_SESSION['_SRE'][$service][$enginename]) === false)
			$_SESSION['_SRE'][$service][$enginename] = array();

		if(isset($_SESSION['_SRE'][$service][$enginename]['ini']) === false)
			$_SESSION['_SRE'][$service][$enginename]['ini'] = array('general' => array());

		$sess_general = &$_SESSION['_SRE'][$service][$enginename]['ini']['general'];

		if(dwho_has_len($this->_services[$service][$enginename],'label') === true)
			$sess_general['label'] = $this->_services[$service][$enginename]['label'];

		if(dwho_has_len($this->_services[$service][$enginename],'version') === true)
			$sess_general['version'] = $this->_services[$service][$enginename]['version'];

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,'service',$service);

		if(xivo::load_class($classname,$dir,$enginename,false) === false)
			trigger_error('Failed to load '.$service.' '.$enginename.' service',E_USER_ERROR);

		if(isset($config['general']['datastorage']) === true)
		{
			dwho::load_class('dwho_datastorage');

			if(is_string($config['general']['datastorage']) === true)
				$config['general']['datastorage'] = dwho_datastorage::get_param_from_uri($config['general']['datastorage']);
			else if(isset($config['general']['datastorage']['type']) === false)
				$config['general']['datastorage'] = false;

			if($config['general']['datastorage'] === false)
				trigger_error('Invalid datastorage in '.$service.' init file',E_USER_ERROR);
		}

		$instance = new $classname($config);

		if(method_exists($instance,'__instance') === true)
			return($this->_set($service,$instance->__instance()));

		return($this->_set($service,$instance));
	}
}

?>
