<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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
	var $_conf	= array();
	var $_uri	= null;

	function xivo_service($ini)
	{
		if(($this->_conf = xivo_gat::get_file(XIVO_PATH_OBJECTCONF.XIVO_SEP_DIR.'service')) === false)
			trigger_error('Failed to load service Array',E_USER_ERROR);
		else if(is_array($ini) === false || empty($ini) === true)
			trigger_error('Invalid parameter in '.__CLASS__,E_USER_ERROR);

		$this->_ini = $ini;

		if(xivo_issa('_SRE',$_SESSION) === false)
			$_SESSION['_SRE'] = array();
	}

	function _load_service($name)
	{
		if($this->is_valid($name) === true)
			return(null);
		else if((bool) xivo_ak($name,$this->_ini,true) === false)
			return(false);
		else if(xivo_issa($name,$this->_conf) === false
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

		$dir = xivo_file::joinpath(XIVO_PATH_OBJECT,'service',$name);

		if(xivo::load_class($cname,$dir,$sname,false) === false)
			trigger_error('Failed to load '.$name.' '.$sname.' service',E_USER_ERROR);

		if(isset($init['general']['datastorage']) === true)
		{
			if(is_string($init['general']['datastorage']) === true)
				$init['general']['datastorage'] = xivo_datastorage::set_param_from_uri($init['general']['datastorage']);
			else if(isset($init['general']['datastorage']['type']) === false)
				$init['general']['datastorage'] = false;

			if($init['general']['datastorage'] === false)
				trigger_error('Invalid datastorage in '.$name.' init file',E_USER_ERROR);
		}

		$instance = new $cname($init);

		if(method_exists($instance,'__instance') === true)
			return($this->_set($name,$instance->__instance()));

		return($this->_set($name,$instance));
	}

	function &get_module($service,$module)
	{
		$service = (string) $service;
		$module = (string) $module;

		if(($class = &$this->get($service)) === false)
			trigger_error('Service not found or installed',E_USER_ERROR);

		return(($ref = &$class->get_module($module)));
	}

	function _load_init_file($name)
	{
		if(preg_match('/^[a-z0-9\-_]+$/i',$name) !== 1)
			return(false);

		return(xivo::load_init(XIVO_PATH_CONF.XIVO_SEP_DIR.$name.'.ini'));
	}

	function _set($name,&$obj)
	{
		if(xivo_has_len($name) === false || is_object($obj) === false)
			return(false);

		return(xivo_gct::set($obj,XIVO_SRE_CLASS_PRE.$name));
	}

	function &get($name)
	{
		$r = false;

		if($this->_load_service($name) !== false)
			$r = &xivo_gct::get(XIVO_SRE_CLASS_PRE.$name);

		return(($ref = &$r));
	}

	function &_set_get($name,&$obj)
	{
		$r = false;

		if(xivo_has_len($name) === true)
			$r = &xivo_gct::set_get($obj,XIVO_SRE_CLASS_PRE.$name);

		return(($ref = &$r));
	}

	function is_valid($name)
	{
		return(xivo_gct::is_valid(XIVO_SRE_CLASS_PRE.$name));
	}
}

?>
