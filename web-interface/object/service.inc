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

	function xivo_service($ini)
	{
		if(($this->_services = dwho_gat::get_file(XIVO_PATH_OBJECTCONF.DWHO_SEP_DIR.'service')) === false)
			trigger_error('Failed to load service Array',E_USER_ERROR);
		else if(is_array($ini) === false || empty($ini) === true)
			trigger_error('Invalid parameter in '.__CLASS__,E_USER_ERROR);

		$this->_ini = $ini;

		if(dwho_issa('_SRE',$_SESSION) === false)
			$_SESSION['_SRE'] = array();
	}

	function _load_service($service)
	{
		if($this->is_loaded($service) === true)
			return(null);
		else if((bool) dwho_ak($service,$this->_ini,true) === false)
			return(false);
		else if(($config = $this->get_config($service)) === false
		|| dwho_issa('general',$config) === false
		|| isset($config['general']['name'],
			 $this->_services[$service][$config['general']['name']]) === false)
			return(false);

		$enginename	= $config['general']['name'];
		$classname	= 'xivo_service_'.$enginename;

		if(isset($_SESSION['_SRE'][$service]) === false)
			$_SESSION['_SRE'][$service] = array();

		if(isset($_SESSION['_SRE'][$service][$enginename]) === false)
			$_SESSION['_SRE'][$service][$enginename] = array();

		if(isset($_SESSION['_SRE'][$service][$enginename]['ini']) === false)
			$_SESSION['_SRE'][$service][$enginename]['ini'] = array('general' => array());

		$sess_general = &$_SESSION['_SRE'][$service][$enginename]['ini']['general'];

		if(dwho_has_len($this->_services[$service][$enginename],'label') === true)
			$sess_general['label'] = $this->_services[$service][$enginename]['label'];

		if(dwho_has_len($config['general'],'label') === true)
			$sess_general['label'] = $config['general']['label'];

		if(dwho_has_len($this->_services[$service][$enginename],'version') === true)
			$sess_general['version'] = $this->_services[$service][$enginename]['version'];

		if(dwho_has_len($config['general'],'version') === true)
			$sess_general['version'] = $config['general']['version'];

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

	function get_engines($service)
	{
		if(dwho_issa($service,$this->_services) === true)
			return($this->_services[$service]);

		return(false);
	}

	function &get_module($service,$module)
	{
		$service = (string) $service;
		$module = (string) $module;

		if(($class = &$this->get($service)) === false)
			trigger_error('Service not found or installed',E_USER_ERROR);

		return(($ref = &$class->get_module($module)));
	}

	function get_init_file($name)
	{
		if(preg_match('/^[a-zA-Z0-9\-_]+$/',$name) !== 1)
			return(false);

		return(xivo::load_init(XIVO_PATH_CONF.DWHO_SEP_DIR.$name.'.ini'));
	}

	function set_config($service,$config)
	{
		if(dwho_issa($service,$this->_services) === false
		|| is_array($config) === false)
			return(false);

		$this->_config[$service] = $config;
		
		return(true);
	}

	function get_config($service)
	{
		if(dwho_issa($service,$this->_services) === false)
			return(false);
		else if(dwho_issa($service,$this->_config) === false
		&& ($config = $this->get_init_file($service)) !== false)
			$this->_config[$service] = $config;

		if(dwho_issa($service,$this->_config) === false)
			return(false);

		return($this->_config[$service]);
	}

	function _set($name,&$obj)
	{
		if(dwho_has_len($name) === false || is_object($obj) === false)
			return(false);

		return(dwho_gct::set($obj,XIVO_SRE_CLASS_PRE.$name));
	}

	function &get($name)
	{
		$r = false;

		if($this->_load_service($name) !== false)
			$r = &dwho_gct::get(XIVO_SRE_CLASS_PRE.$name);

		return(($ref = &$r));
	}

	function &_set_get($name,&$obj)
	{
		$r = false;

		if(dwho_has_len($name) === true)
			$r = &dwho_gct::set_get($obj,XIVO_SRE_CLASS_PRE.$name);

		return(($ref = &$r));
	}

	function is_loaded($name)
	{
		return(dwho_gct::is_valid(XIVO_SRE_CLASS_PRE.$name));
	}
}

?>
