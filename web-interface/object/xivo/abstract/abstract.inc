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

require_once(dwho_file::joinpath(XIVO_PATH_OBJECT,'abstract','libdatastorage.inc'));

class xivo_object_abstract extends xivo_libdatastorage_abstract
{
	var $_conf	= array();
	var $_xobj	= null;

	function xivo_object_abstract(&$xobj)
	{
		if(dwho_is_instance($xobj,'xivo_object') === false)
			trigger_error('Invalid XIVO Object in '.__CLASS__,E_USER_ERROR);

		$this->_xobj = &$xobj;

		$this->_load_config();
	}

	function get_name()
	{
		return($this->_name);
	}

	function _load_config()
	{
		if(empty($this->_conf) === false)
			return(null);

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'xivo',
					   $this->_name,
					   'config');

		if(($this->_conf = dwho_gat::get_file($dir)) === false)
			$this->_conf = array();
	}
}

?>
