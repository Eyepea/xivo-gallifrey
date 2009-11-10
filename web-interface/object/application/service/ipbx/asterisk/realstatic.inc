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

class xivo_application_service_asterisk_realstatic
{
	var $_service	= null;

	function xivo_application_service_asterisk_realstatic(&$service)
	{
		if(dwho_is_instance($service,'xivo_service_asterisk') === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_service = &$service;
	}

	function _load_module_class($name)
	{
		$name = (string) $name;

		if($this->is_valid($name) === false)
			return(false);

		$file = $name.'.inc';

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'application',
					   'service',
					   'ipbx',
					   'asterisk',
					   'realstatic',
					   $this->_name,
					   $name);

		$class = 'xivo_application_service_asterisk_realstatic_'.$this->_name.'_'.$name;

		if(dwho_file::load_file($file,$dir,true) !== false && class_exists($class) === true)
			return($class);

		return(false);
	}

	function _load_module($name)
	{
		$name = (string) $name;

		if($this->is_valid($name) === false)
			return(false);
		else if(isset($this->_module[$name]) === true && is_object($this->_module[$name]) === true)
			return(true);

		if(($class = $this->_load_module_class($name)) === false)
		{
			trigger_error('RealStatic Application\'s module '.$name.' does not exist',E_USER_ERROR);
			return(false);
		}

		$this->_module[$name] = new $class($this->_service);

		return(true);
	}

	function &get_module($name)
	{
		$r = false;

		$name = (string) $name;

		if($this->_load_module($name) === true)
			$r = &$this->_module[$name];

		return(($ref = &$r));
	}

	function is_valid($name)
	{
		$name = (string) $name;

		return(isset($this->_module[$name]));
	}
}

?>
