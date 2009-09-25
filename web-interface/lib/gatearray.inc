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

class dwho_gatearray
{
	var $_array = array();

	function &getinstance()
	{
		static $instance = null;

		if($instance === null)
			$instance = new dwho_gatearray();

		return(($ref = &$instance));
	}

	function set($name,&$array)
	{
		$name = (string) $name;

		if(is_array($array) === false || $name === '')
			return(false);
		else if(isset($this->_array[$name]) === false)
			$this->_array[$name] = &$array;

		return(true);
	}

	function load($file,$path='',$name='')
	{
		$file = (string) $file;
		$name = (string) $name;

		if(($pos = strrpos($file,'.')) !== false)
		{
			$file = substr($file,0,$pos);

			if(preg_match('/^[a-z0-9\-_]+$/i',$file) !== 1)
				return(false);
		}

		if($name === '')
			$name = $file;

		if($path === '')
			$path = DWHO_PATH_ROOT.DIRECTORY_SEPARATOR.'gatearray'.DIRECTORY_SEPARATOR.$file.'.inc';
		else if(is_array($path) === true)
			$path = implode(DIRECTORY_SEPARATOR,$path).DIRECTORY_SEPARATOR.$file.'.inc';
		else
			$path = rtrim($path,DIRECTORY_SEPARATOR).DIRECTORY_SEPARATOR.$file.'.inc';

		if(dwho_file::is_includable($path) === false)
			return(false);
		else if(isset($this->_array[$name]) === false)
		{
			require_once($path);

			if(isset($array) === false || is_array($array) === false)
				return(false);

			$this->_array[$name] = &$array;
		}

		return($name);
	}

	function get_file($path,$once=false)
	{
		if(is_array($path) === true)
			$path = implode(DIRECTORY_SEPARATOR,$path);

		$path = (string) $path.'.inc';

		if(dwho_file::is_includable($path) === false)
			return(false);
		else if((bool) $once === false)
			require($path);
		else
			require_once($path);

		if(isset($array) === true && is_array($array) === true)
			$r = &$array;
		else
			$r = false;

		return($r);
	}

	function &get($name='')
	{
		$name = (string) $name;

		if($name === '')
			$r = &$this->_array;
		else if(dwho_issa($name,$this->_array) === true)
			$r = &$this->_array[$name];
		else
			$r = false;

		return(($ref = &$r));
	}

	function &load_get($file,$path='',$name='')
	{
		$r = false;

		if(($name = $this->load($file,$path,$name)) !== false)
			$r = &$this->get($name);

		return(($ref = &$r));
	}

	function &set_get($name,&$array)
	{
		$r = false;

		if($this->set($name,$array) === true)
			$r = &$this->get($name);

		return(($ref = &$r));
	}

	function is_valid($name)
	{
		$name = (string) $name;

		if(dwho_issa($name,$this->_array) === true)
			return(true);

		return(false);
	}
}

class dwho_gat
{
	function set($name,&$array)
	{
		$aga = &dwho_gatearray::getinstance();
		return($aga->set($name,$array));
	}

	function load($file,$path='',$name='')
	{
		$aga = &dwho_gatearray::getinstance();
		return($aga->load($file,$path,$name));
	}

	function &get($name='')
	{
		$aga = &dwho_gatearray::getinstance();
		return(($ref = &$aga->get($name)));
	}

	function get_file($path,$once=false)
	{
		return(dwho_gatearray::get_file($path,$once));
	}

	function &load_get($file,$path='',$name='')
	{
		$aga = &dwho_gatearray::getinstance();
		return(($ref = &$aga->load_get($file,$path,$name)));
	}

	function &set_get($name,&$array)
	{
		$aga = &dwho_gatearray::getinstance();
		return(($ref = &$aga->set_get($name,$array)));
	}

	function is_valid($name)
	{
		$aga = &dwho_gatearray::getinstance();
		return($aga->is_valid($name));
	}
}

?>
