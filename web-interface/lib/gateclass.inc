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

class dwho_gateclass
{
	var $_class = array();

	function &getinstance()
	{
		static $instance = null;

		if($instance === null)
			$instance = new dwho_gateclass();

		return(($ref = &$instance));
	}

	function set(&$obj,$name='')
	{
		$name = (string) $name;

		if(is_object($obj) === false)
			return(false);
		else if($name === '')
			$name = get_class($obj);

		if($name !== '' && preg_match('/^[a-z0-9\-_]+$/i',$name) !== 1)
			return(false);
		else if(isset($this->_class[$name]) === false)
			$this->_class[$name] = &$obj;

		return(true);
	}

	function &get($name)
	{
		$r = false;

		$name = (string) $name;

		if(isset($this->_class[$name]) === true)
			$r = &$this->_class[$name];

		return(($ref = &$r));
	}

	function &set_get(&$obj,$name='')
	{
		$r = false;

		$name = (string) $name;

		if($this->set($obj,$name) === true)
			$r = &$this->get(($name === '' ? get_class($obj) : $name));

		return(($ref = &$r));
	}

	function is_valid($name)
	{
		return((isset($this->_class[$name]) === true && is_object($this->_class[$name]) === true));
	}
}

class dwho_gct
{
	function set(&$obj,$name='')
	{
		$agc = &dwho_gateclass::getinstance();
		return($agc->set($obj,$name));
	}

	function &get($name)
	{
		$agc = &dwho_gateclass::getinstance();
		return(($ref = &$agc->get($name)));
	}

	function &set_get(&$obj,$name='')
	{
		$agc = &dwho_gateclass::getinstance();
		return(($ref = &$agc->set_get($obj,$name)));
	}

	function is_valid($name)
	{
		$agc = &dwho_gateclass::getinstance();
		return($agc->is_valid($name));
	}
}

?>
