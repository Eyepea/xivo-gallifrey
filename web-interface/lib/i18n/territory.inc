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

class dwho_i18n_territory
{
	var $_attrib	= array();
	var $_conf	= array();

	function dwho_i18n_territory()
	{
		$file = dwho_file::joinpath(DWHO_PATH_ROOT,'i18n','territories','territories');

		if(($this->_conf = dwho_gat::get_file($file)) === false)
			trigger_error('Failed to load territories listing',E_USER_ERROR);
	}

	function set_attrib($name,$value)
	{
		$name = (string) $name;

		$this->_attrib[$name] = $value;
	}

	function get_attrib($name)
	{
		$name = (string) $name;

		if(isset($this->_attrib[$name]) === true)
			return($this->_attrib[$name]);
		else
			return(null);
	}

	function is_valid($territo)
	{
		$territo = strtoupper($territo);

		if(dwho_issa($territo,$this->_conf) === false)
			return(false);

		return($territo);
	}

	function get_info($territo)
	{
		if(($territo = $this->is_valid($territo)) === false)
			return(false);

		$r = $this->_conf[$territo];
		$r['territo'] = $territo;

		return($r);
	}

	function get_timezone($territo)
	{
		if(($territo = $this->is_valid($territo)) === false
		|| dwho_issa('tz',$this->_conf[$territo]) === false
		|| count($this->_conf[$territo]['tz']) !== 1)
			return(false);

		reset($this->_conf[$territo]['tz']);

		return(key($this->_conf[$territo]['tz']));
	}

	function get_timezone_list($territo)
	{
		if(($territo = $this->is_valid($territo)) === false
		|| dwho_issa('tz',$this->_conf[$territo]) === false)
			return(false);

		return($this->_conf[$territo]['tz']);
	}

	function exists_default($territo)
	{
		if(($territo = $this->is_valid($territo)) === false)
			return(false);

		return(dwho_ak('default',$this->_conf[$territo],true));
	}

	function get_list()
	{
		return($this->_conf);
	}
}

?>
