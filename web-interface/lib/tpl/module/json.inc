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

require_once(xivo_file::joinpath(XIVO_PATH_LIB,'tpl','module.inc'));

class xivo_tpl_json extends xivo_tpl_module
{
	var $_tpl	= null;

	function xivo_tpl_json(&$tpl)
	{
		$this->_tpl = &$tpl;

		if(xivo::load_class('xivo_json') === false)
			trigger_error('Failed to load json class',E_USER_ERROR);
	}

	function get_header()
	{
		return(xivo_json::get_header());
	}

	function encode($data)
	{
		return(xivo_json::encode($data));
	}

	function decode($data,$assoc=false)
	{
		return(xivo_json::decode($data,$assoc));
	}

	function display()
	{
		return($this->_tpl->display('/json'));
	}
}

?>
