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

class dwho_i18n_locale
{
	var $_attrib	= array();
	var $_conf	= array();

	function dwho_i18n_locale()
	{
		$file = dwho_file::joinpath(DWHO_PATH_ROOT,'i18n','locales','locales');

		if(($this->_conf = dwho_gat::get_file($file)) === false)
			trigger_error('Failed to load locales listing',E_USER_ERROR);
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

		return(null);
	}

	function is_valid($locale)
	{
		if(isset($this->_conf[$locale]) === false)
			$locale = preg_replace('/([a-z]{2,3})[_\- ]+([a-z]{2,3})/ie',
					       'strtolower(\'\\1\').\'_\'.strtoupper(\'\\2\')',
					       $locale);

		if(dwho_issa($locale,$this->_conf) === false)
			return(false);

		return($locale);
	}

	function get_info($locale)
	{
		if(($locale = $this->is_valid($locale)) === false)
			return(false);

		$exp = explode('_',$locale,2);

		$r = $this->_conf[$locale];
		$r['locale'] = $locale;
		$r['lang'] = $exp[0];
		$r['territo'] = $exp[1];

		return($r);
	}

	function get_territory($locale)
	{
		if(($locale = $this->is_valid($locale)) === false)
			return(false);

		$territo = explode('_',$locale,2);

		return($territo[1]);
	}

	function get_language($locale)
	{
		if(($locale = $this->is_valid($locale)) === false)
			return(false);

		$lang = explode('_',$locale,2);

		return($lang[0]);
	}

	function get_list()
	{
		return($this->_conf);
	}
}

?>
