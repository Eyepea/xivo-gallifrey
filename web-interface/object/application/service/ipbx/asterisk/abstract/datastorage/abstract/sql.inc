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

require_once(dwho_file::joinpath(XIVO_PATH_OBJECT,'application','abstract','datastorage','sql.inc'));

class xivo_application_service_asterisk_abstract_sql extends dwho_datastorage_helper_abstract
{
	var $_cnt	= 0;
	var $_dsoconf	= array();
	var $_table	= array();

	function xivo_application_service_asterisk_abstract_sql(&$dso,&$dsoconf)
	{
		parent::_init(&$dso);

		if(is_array($dsoconf) === false)
			trigger_error('Invalid Datastorage configuration array',E_USER_ERROR);

		$this->_dsoconf = &$dsoconf;

		$this->_set_table();
	}

	function _set_table()
	{
		if(is_array($this->_table) === false
		|| empty($this->_table) === true)
			return(false);

		reset($this->_table);

		while(list($key) = each($this->_table))
		{
			if($this->_get_table($key) === false)
				trigger_error('Invalid or missing Datastorage table '.$key,E_USER_ERROR);
		}

		return(true);
	}

	function get_cnt()
	{
		return(dwho_ulongint($this->_cnt));
	}

	function _get_table($module)
	{
		if(isset($this->_table[$module]) === true
		&& dwho_has_len($this->_table[$module]) === true)
			return($this->_table[$module]);
		else if(($table = $this->get_modconf_key($module,'table')) === false)
			return(false);

		$this->_table[$module] = $table;

		return($table);
	}

	function get_modconf_key($module,$key)
	{
		if(($modconf = $this->get_modconf($module)) === false
		|| isset($modconf[$key]) === false)
			return(false);

		return($modconf[$key]);
	}

	function get_modconf($module)
	{
		if(is_array($this->_dsoconf) === true
		&& isset($this->_dsoconf[$module]) === true
		&& is_array($this->_dsoconf[$module]) === true)
			return($this->_dsoconf[$module]);

		return(false);
	}
}

?>
