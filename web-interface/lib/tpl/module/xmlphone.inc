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

class dwho_tpl_xmlphone extends dwho_tpl_module
{
	var $_vendor	= null;

	function dwho_tpl_xmlphone(&$tpl)
	{
		$this->_tpl = &$tpl;
	}

	function normalize_vendor_name($vendor)
	{
		if(is_scalar($vendor) === false
		|| preg_match('/^[a-zA-Z0-9_-]+$/',$vendor) === 0)
			return(false);

		return(strtolower($vendor));
	}

	function factory($vendor)
	{
		$this->_vendor = null;

		if(($vendor = $this->normalize_vendor_name($vendor)) === false)
			return(false);

		$dir = array('tpl',
			     'module',
			     'xmlphone');

		$classname = 'dwho_tpl_xmlphone_'.$vendor;

		if(dwho::load_class($classname,$dir,$vendor) === false)
			return(false);

		$this->_vendor = $vendor;

		return(new $classname());
	}

	function get_vendor()
	{
		if($this->_vendor === null)
			return(false);

		return($this->_vendor);
	}
}

?>
