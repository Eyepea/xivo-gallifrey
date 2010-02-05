<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

class xivo_object
{
	var $_conf		= array();
	var $_module		= array();
	var $_application	= array();
	var $_identity		= array();
	var $_dso		= null;
	var $_dsoconf		= array();

	function xivo_object()
	{
		$this->_load_config();
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

	function required($arr,$once=false,$includable=false)
	{
		$arr = (array) $arr;

		array_unshift($arr,dirname(__FILE__).DWHO_SEP_DIR.'xivo');

		return(dwho_file::required($arr,$once,$includable));
	}

	function _load_identity_class($module)
	{
		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'xivo',
					   $module);

		$class = 'xivo_object_'.$module.'_identity';

		if(dwho_file::load_file('identity.inc',$dir,true) === false
		|| class_exists($class) === false)
			return(false);

		return($class);
	}

	function _load_identity($module)
	{
		if($this->chk_module_enable($module) === false)
		{
			trigger_error('Invalid module '.$module.' in XIVO Object to load identity class',E_USER_ERROR);
			return(false);
		}
		else if(isset($this->_identity[$module]) === true)
			return(true);
		else if(($class = $this->_load_identity_class($module)) === false)
		{
			trigger_error($module.' identity class does not exist',E_USER_ERROR);
			return(false);
		}
		else
			$this->_identity[$module] = new $class($this);

		return(true);
	}

	function &get_identity($module)
	{
		$r = false;

		if($this->_load_identity($module) === true)
			$r = &$this->_identity[$module];

		return(($ref = &$r));
	}

	function identity($module,$arr,$full=true)
	{
		if(($identity = &$this->get_identity($module)) === false)
			return(false);

		return($identity->mk_identity($arr,$full));
	}

	function load_module_class($module)
	{
		if($this->chk_module_enable($module) === false)
		{
			trigger_error('Invalid module '.$module.' in XIVO Object',E_USER_ERROR);
			return(false);
		}

		return($this->_load_module_class($module));
	}

	function _load_module_class($module)
	{
		$module = (string) $module;

		$file = $module.'.inc';

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'xivo',
					   $module);

		$class = 'xivo_object_'.$module;

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
					   'xivo',
					   $application);

		$class = 'xivo_application_xivo_object_'.$application;

		if(dwho_file::load_file($file,$dir,true) !== false && class_exists($class) === true)
			return($class);

		return(false);
	}

	function _load_config()
	{
		if(empty($this->_conf) === false)
			return(null);

		$file = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'xivo',
					   'config',
					   'config');

		if(($this->_conf = dwho_gat::get_file($file)) === false)
			trigger_error('Failed to load XIVO Object configuration',E_USER_ERROR);
	}

	function _load_dso_config($type)
	{
		if(isset($this->_dsoconf[$type]) === true)
			return(true);

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'xivo',
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

	function chk_module_enable($name)
	{
		return($this->_chk_exists_cfg('module',$name));
	}

	function _load_module($name,$param=null)
	{
		$name = (string) $name;

		if($this->chk_module_enable($name) === false)
		{
			trigger_error('Invalid module '.$name.' in XIVO Object',E_USER_ERROR);
			return(false);
		}

		$ref = &$this->_conf['module'][$name];

		if(isset($this->_module[$name]) === true)
			return(true);
		else if(($class = $this->_load_module_class($name)) === false)
		{
			trigger_error('Module '.$name.' does not exist in XIVO Object',E_USER_ERROR);
			return(false);
		}
		else if(isset($ref['datastorage']) === false || (bool) $ref['datastorage'] === false)
			$this->_module[$name] = new $class($this,$param);
		else
			$this->_module[$name] = new $class($this,$this->_load_dso($name),$param);

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
			trigger_error('Application '.$name.' does not exist in XIVO Object',E_USER_ERROR);
			return(false);
		}
		else if(($dsoclass = $this->_load_dso_application($name)) !== false)
			$this->_application[$name] = new $class($this,$dsoclass,$param,$components);
		else
			$this->_application[$name] = new $class($this,$param,$components);

		return(true);
	}

	function _load_dso_application($name)
	{
		if(is_object($this->_dso) === true)
			$dso = &$this->_dso;
		else if($this->_load_dso() === true && is_object($this->_dso) === true)
			$dso = &$this->_dso;
		else
			return(false);

		$dsotype = $dso->get_type();

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'application',
					   'xivo',
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

	function _load_dso_application_class($name,$dir,$dsotype,&$dso,$meta=false,$param=false)
	{
		if($meta === false)
			$metatype = $dsotype;
		else
			$metatype = $meta;

		$class = 'xivo_application_xivo_object_'.$name.'_'.$metatype;

		if($param === false && $this->_load_dso_config($metatype) === true)
			$param = &$this->_dsoconf[$metatype];

		if($param === false || xivo::load_class($class,$dir,$metatype,false) === false)
			return($param);

		return(new $class($dso,&$param));
	}

	function _load_dso($name='')
	{
		$name = (string) $name;

		if(is_object($this->_dso) === false)
			$this->_dso = &dwho_gct::get('DWHO_DSO');

		if(is_object($this->_dso) === false)
			return(false);

		$type = $this->_dso->get_type();

		if($name === '')
		{
			$this->_load_dso_config($type);
			return(true);
		}

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'xivo',
					   $name,
					   'datastorage');

		$dso = $this->_load_dso_class($name,$type,$dir);

		if(is_object($dso) === true)
			return($dso);

		if(($meta = dwho_datastorage::get_meta($type)) === false)
		{
			trigger_error('Failed to load datastorage '.$type.' for '.$name,E_USER_ERROR);
			return(false);
		}

		$dso = $this->_load_dso_class($name,$type,$dir,$meta);

		if(is_object($dso) === true)
			return($dso);

		trigger_error('Failed to load datastorage '.$type.' for '.$name,E_USER_ERROR);

		return(false);
	}

	function _load_dso_class($name,$type,$dir,$meta=false)
	{
		if($meta === false)
			$metatype = $type;
		else
			$metatype = $meta;

		$class = 'xivo_object_'.$name.'_'.$metatype;

		if(($param = $this->get_dso_config_by_name($metatype,$name)) === false)
			$param = array();

		if(xivo::load_class($class,$dir,$metatype,false) === false)
			return(false);
		else if(is_object($this->_dso) === false)
			$this->_dso = &dwho_gct::get('DWHO_DSO');

		return(new $class($this->_dso,$param));
	}
}

?>
