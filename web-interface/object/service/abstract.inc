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

		if(isset($this->_conf) === true
		&& dwho_issa($type,$this->_conf) === true
		&& isset($this->_conf[$type][$name]) === true)
			$r = (bool) $ret === false ? true : $this->_conf[$type][$name];

		return($r);
	}

	function load_module_class($module)
	{
		if($this->chk_module_enable($module) === false)
		{
			trigger_error('Invalid module '.$module.' in '.$this->_name,E_USER_ERROR);
			return(false);
		}

		return($this->_load_module_class($module));
	}

	function _load_module_class($module)
	{
		$module = (string) $module;

		$file = $module.'.inc';

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'service',
					   $this->_type,
					   $this->_name,
					   $module);

		$class = 'xivo_service_'.$this->_name.'_'.$module;

		if(dwho_file::load_file($file,$dir,true) !== false && class_exists($class) === true)
			return($class);

		return(false);
	}

	function _load_application_class($application)
	{
		$application = (string) $application;

		$file = $application.'.inc';

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'application',
					   'service',
					   $this->_type,
					   $this->_name,
					   $application);

		$class = 'xivo_application_service_'.$this->_name.'_'.$application;

		if(dwho_file::load_file($file,$dir,true) !== false && class_exists($class) === true)
			return($class);

		return(false);
	}

	function _load_appcustom_class($type,$name)
	{
		$type = (string) $type;
		$name = (string) $name;

		$file = $name.'.inc';

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'application',
					   'service',
					   $this->_type,
					   $this->_name,
					   $type,
					   $name);

		$class = 'xivo_application_service_'.$this->_name.'_'.$type.'_'.$name;

		if(dwho_file::load_file($file,$dir,true) !== false && class_exists($class) === true)
			return($class);

		return(false);
	}

	function _load_config()
	{
		if(empty($this->_conf) === false)
			return(null);

		$file = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'service',
					   $this->_type,
					   $this->_name,
					   'config',
					   'config');

		if(($this->_conf = dwho_gat::get_file($file)) === false)
			trigger_error('Failed to load '.$this->_name.' configuration',E_USER_ERROR);
	}

	function _load_dso_config($type)
	{
		if(isset($this->_dsoconf[$type]) === true)
			return(true);

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'service',
					   $this->_type,
					   $this->_name,
					   'config',
					   'datastorage',
					   $type);

		if(($this->_dsoconf[$type] = dwho_gat::get_file($dir)) === false)
			return(false);

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

	function &get_module($name,$param=null)
	{
		$r = false;

		$name = (string) $name;

		if($this->_load_module($name,$param) === true)
			$r = &$this->_module[$name];

		return(($ref = &$r));
	}

	function &get_application($name,$param=null,$components=true)
	{
		$r = false;

		$name = (string) $name;

		if($this->_load_application($name,$param,$components) === true)
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

		if($this->chk_module_enable($name) === false)
		{
			trigger_error('Invalid module '.$name.' in '.$this->_name,E_USER_ERROR);
			return(false);
		}

		$ref = &$this->_conf['module'][$name];

		if(isset($this->_module[$name]) === true)
			return(true);
		else if(($class = $this->_load_module_class($name)) === false)
		{
			trigger_error('Module '.$name.' does not exist in '.$this->_name,E_USER_ERROR);
			return(false);
		}
		else if(isset($ref['datastorage']) === false || (bool) $ref['datastorage'] === false)
			$this->_module[$name] = new $class($this,$param);
		else
		{
			$custom = (bool) dwho_ak('custom',$ref['datastorage'],true);
			$this->_module[$name] = new $class($this,$this->_load_dso($name,$custom),$param);
		}

		return(true);
	}

	function _load_application($name,$param=null,$components=true)
	{
		$name = (string) $name;

		if(isset($this->_application[$name]) === true)
		{
			if(is_array($param) === true)
				$this->_application[$name]->load_init($param);

			if((bool) $components === true)
				$this->_application[$name]->load_components();
			return(true);
		}
		else if(($class = $this->_load_application_class($name)) === false)
		{
			trigger_error('Application '.$name.' does not exist in '.$this->_name,E_USER_ERROR);
			return(false);
		}
		else if(($dsoclass = $this->_load_dso_application($name)) !== false)
			$this->_application[$name] = new $class($this,$dsoclass,$param,$components);
		else
			$this->_application[$name] = new $class($this,$param,$components);

		return(true);
	}

	function _load_appcustom($type,$name)
	{
		$type = (string) $type;
		$name = (string) $name;

		if(dwho_issa($this->_appcustom,$type) === true
		&& isset($this->_appcustom[$type][$name]) === true)
			return(true);
		else if($this->chk_appcustom_enable($type,$name) === false)
			return(false);
		else if(($class = $this->_load_appcustom_class($type,$name)) === false)
		{
			trigger_error('Custom Application '.$name.' does not exist in '.$this->_name,E_USER_ERROR);
			return(false);
		}
		else if(($dsoclass = $this->_load_dso_appcustom($type,$name)) !== false)
			$this->_appcustom[$type][$name] = new $class($this,$dsoclass);
		else
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

		$dsotype = $dso->get_type();

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'application',
					   'service',
					   $this->_type,
					   $this->_name,
					   $name,
					   'datastorage');

		$appdso = $this->_load_dso_application_class($name,$dir,$dsotype,$dso);

		if(is_object($appdso) === true)
			return($appdso);
		else if(($meta = dwho_datastorage::get_meta($dsotype)) === false)
			return(false);

		$appdso = $this->_load_dso_application_class($name,$dir,$dsotype,$dso,$meta,$appdso);

		if(is_object($appdso) === true)
			return($appdso);

		return(false);
	}

	function _load_dso_appcustom($type,$name)
	{
		$type = (string) $type;
		$name = (string) $name;

		if(is_object($this->_dsogeneral) === true)
			$dso = &$this->_dsogeneral;
		else if($this->_load_dso() === true && is_object($this->_dsogeneral) === true)
			$dso = &$this->_dsogeneral;
		else
		{
			trigger_error('Missing general Datastorage in '.$this->_name.' for '.$type.'::'.$name,E_USER_ERROR);
			return(false);
		}

		$dsotype = $dso->get_type();

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'application',
					   'service',
					   $this->_type,
					   $this->_name,
					   $type,
					   $name,
					   'datastorage');

		$appdso = $this->_load_dso_application_class($name,$dir,$dsotype,$dso,false,false,$type);

		if(is_object($appdso) === true)
			return($appdso);
		else if(($meta = dwho_datastorage::get_meta($dsotype)) === false)
			return(false);

		$appdso = $this->_load_dso_application_class($name,$dir,$dsotype,$dso,$meta,$appdso,$type);

		if(is_object($appdso) === true)
			return($appdso);

		return(false);
	}

	function _load_dso_application_class($name,$dir,$dsotype,&$dso,$meta=false,$param=false,$custom=false)
	{
		if($meta === false)
			$metatype = $dsotype;
		else
			$metatype = $meta;

		if($custom === false)
			$class = 'xivo_application_service_'.$this->_name.'_'.$name.'_'.$metatype;
		else
			$class = 'xivo_application_service_'.$this->_name.'_'.$custom.'_'.$name.'_'.$metatype;

		if($param === false && $this->_load_dso_config($metatype) === true)
			$param = &$this->_dsoconf[$metatype];

		if($param === false || xivo::load_class($class,$dir,$metatype,false) === false)
			return($param);

		return(new $class($dso,&$param));
	}

	function _load_dso($name='',$custom=false)
	{
		$name = (string) $name;
		$custom = (bool) $custom;
		$ini = $gendatastorage = $datastorage = $datastoragemod = false;

		if($name !== '' && dwho_issa($name,$this->_ini) === true)
			$ini = $this->_ini[$name];

		if(isset($this->_ini['general']['datastorage']) === true)
			$datastorage = $gendatastorage = $this->_ini['general']['datastorage'];

		if($name === '')
		{
			if($datastorage === false || isset($datastorage['type']) === false)
				return(false);
			else if(dwho_datastorage::is_valid($datastorage['type']) === false)
			{
				trigger_error('Invalid datastorage '.$datastorage['type'],E_USER_ERROR);
				return(false);
			}

			$this->_load_dso_config($datastorage['type']);

			if(is_object($this->_dsogeneral) === false)
				$this->_dsogeneral = dwho_datastorage::factory($datastorage['type'],$datastorage);

			return(true);
		}

		if($custom === false && $gendatastorage === false)
			trigger_error('Missing general datastorage in '.$this->_type.' init file',E_USER_ERROR);

		if($ini === false && $gendatastorage === false)
			trigger_error('Invalid or missing '.$name.' datastorage in '.$this->_type.' init file',E_USER_ERROR);

		if($ini !== false && isset($ini['datastorage']) === true)
		{
			if(is_string($ini['datastorage']) === true)
				$datastoragemod = dwho_datastorage::get_param_from_uri($ini['datastorage']);
			else
				$datastoragemod = $ini['datastorage'];

			if($custom === true)
				$datastorage = $datastoragemod;
		}

		if($datastorage === false)
			trigger_error('Invalid or missing '.$name.' datastorage in '.$this->_type.' init file',E_USER_ERROR);
		else if(isset($datastorage['type']) === false)
			trigger_error('Missing datastorage type in '.$this->_type.' init file for '.$name,E_USER_ERROR);
		else if(dwho_datastorage::is_valid($datastorage['type']) === false)
			trigger_error('Invalid datastorage '.$datastorage['type'].' for '.$name,E_USER_ERROR);

		$type = $datastorage['type'];

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'service',
					   $this->_type,
					   $this->_name,
					   $name,
					   'datastorage');

		$dso = $this->_load_dso_class($name,$type,$dir,$custom,$datastorage,$datastoragemod);

		if(is_object($dso) === true)
			return($dso);

		if(($meta = dwho_datastorage::get_meta($type)) === false)
		{
			trigger_error('Failed to load datastorage '.$type.' for '.$name,E_USER_ERROR);
			return(false);
		}

		$dso = $this->_load_dso_class($name,$type,$dir,$custom,$datastorage,$datastorage,$meta,$dso);

		if(is_object($dso) === true)
			return($dso);

		trigger_error('Failed to load datastorage '.$type.' for '.$name,E_USER_ERROR);

		return(false);
	}

	function _load_dso_class($name,$type,$dir,$custom,$datastorage,$datastoragemod,$meta=false,$param=false)
	{
		if($meta === false)
			$metatype = $type;
		else
			$metatype = $meta;

		$class = 'xivo_service_'.$this->_name.'_'.$name.'_'.$metatype;

		if(empty($param) === true
		&& ($param = $this->get_dso_config_by_name($metatype,$name)) === false)
			$param = array();

		if(is_array($datastoragemod) === true)
			$paramod = array_merge($datastoragemod,$param);
		else
			$paramod = $param;

		if(xivo::load_class($class,$dir,$metatype,false) === false)
			return($param);
		else if($custom === false)
		{
			if(is_object($this->_dsogeneral) === false)
				$this->_dsogeneral = dwho_datastorage::factory($type,$datastorage);

			return(new $class($this->_dsogeneral,$paramod));
		}
		else
		{
			if(isset($this->_dsocustom[$name]) === false
			|| is_object($this->_dsocustom[$name]) === false)
				$this->_dsocustom[$name] = dwho_datastorage::factory($type,$datastorage);

			return(new $class($this->_dsocustom[$name],$paramod));
		}

		return($param);
	}
}

?>
