<?php

#
# XiVO Web-Interface
# Copyright (C) 2006, 2007, 2008  Proformatique <technique@proformatique.com>
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

require_once(xivo_file::joinpath(XIVO_PATH_LIB,'tpl','module.inc'));

class xivo_tpl_user extends xivo_tpl_module
{
	var $_tpl	= null;

	function xivo_tpl_user(&$tpl)
	{
		$this->_tpl = &$tpl;
		$this->_usr = xivo_gct::get('_USR');
	}

	function get_info($name)
	{
		return($this->_usr->get_info($name));
	}
}

?>
