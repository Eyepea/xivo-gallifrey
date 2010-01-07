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

require_once(dwho_file::joinpath(DWHO_PATH_ROOT,'tpl','module.inc'));

class dwho_tpl_json extends dwho_tpl_module
{
	var $_tpl	= null;

	function dwho_tpl_json(&$tpl)
	{
		$this->_tpl = &$tpl;

		if(dwho::load_class('dwho_json') === false)
			trigger_error('Failed to load json class',E_USER_ERROR);
	}

	function get_header()
	{
		return(dwho_json::get_header());
	}

	function encode($data)
	{
		return(dwho_json::encode($data));
	}

	function decode($data,$assoc=false)
	{
		return(dwho_json::decode($data,$assoc));
	}

	function display()
	{
		return($this->_tpl->display('/json'));
	}
}

?>
