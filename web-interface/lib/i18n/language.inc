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

class dwho_i18n_language
{
	var $_attrib	= array();
	var $_conf	= array();

	function dwho_i18n_language()
	{
		$file = dwho_file::joinpath(DWHO_PATH_ROOT,'i18n','languages','languages');

		if(($this->_conf = dwho_gat::get_file($file)) === false)
			trigger_error('Failed to load languages listing',E_USER_ERROR);
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

	function is_valid($lang)
	{
		$lang = strtolower($lang);

		if(dwho_issa($lang,$this->_conf) === false)
			return(false);

		return($lang);
	}

	function get_info($lang)
	{
		if(($lang = $this->is_valid($lang)) === false)
			return(false);

		$r = $this->_conf[$lang];
		$r['lang'] = $lang;

		return($r);
	}

	function get_locale_list($lang)
	{
		if(($lang = $this->is_valid($lang)) === false)
			return(false);

		return($this->_conf[$lang]['locale']);
	}

	function exists_default($lang)
	{
		if(($lang = $this->is_valid($lang)) === false)
			return(false);

		return(dwho_ak('default',$this->_conf[$lang],true));
	}

	function get_list()
	{
		return($this->_conf);
	}
}

?>
