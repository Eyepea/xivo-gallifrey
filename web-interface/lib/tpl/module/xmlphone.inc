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

class xivo_tpl_xmlphone extends xivo_tpl_module
{
	var $_vendor		= null;

	function xivo_tpl_xmlphone(&$tpl)
	{
		$this->_tpl = &$tpl;
	}

	function factory($vendor)
	{
		$this->_vendor = null;

		if(($vendor = $this->chk_vendor($vendor)) === false)
			return(false);

		$dir = array('tpl',
			     'module',
			     'xmlphone');

		$classname = 'xivo_tpl_xmlphone_'.$vendor;

		if(xivo::load_class($classname,$dir,$vendor) === false)
			return(false);

		$this->_vendor = $vendor;

		return(new $classname());
	}

	function chk_vendor($vendor)
	{
		if(is_scalar($vendor) === false)
			return(false);

		$vendor = strtolower($vendor);

		switch($vendor)
		{
			case 'aastra':
			case 'snom':
			case 'thomson':
				return($vendor);
		}

		return(false);
	}
/*
	function _load_vendor($vendor)
	{
		if(($vendor = $this->chk_vendor($vendor)) === false)
			return(false);

		$dir = array(XIVO_PATH_LIB,
			     'tpl',
			     'module',
			     'xmlphone',
			     $vendor);

		$classname = 'xivo_tpl_xmlphone_'.$vendor;

		if(xivo::load_class($classname,$dir,$vendor) === false)
			return(false);

		return(new $classname());

		switch($vendor)
		{
			case 'aastra':
				$this->_tag['menu'] = 'AastraIPPhoneTextMenu';
				$this->_tag['directory'] = 'AastraIPPhoneDirectory';
				$this->_tag['input'] = 'AastraIPPhoneInputScreen';
				$this->_argseparator = '&amp;';
				$this->_cdata = true;
				$this->_escape = false;
				$this->_utf8 = false;
				break;
			case 'snom':
				$this->_tag['menu'] = 'SnomIPPhoneMenu';
				$this->_tag['directory'] = 'SnomIPPhoneDirectory';
				$this->_tag['input'] = 'SnomIPPhoneInput';
				$this->_argseparator = '&';
				$this->_cdata = false;
				$this->_escape = false;
				$this->_utf8 = true;
				break;
			case 'thomson':
				$this->_tag['menu'] = 'ThomsonPhoneMenu';
				$this->_tag['directory'] = 'ThomsonPhoneBook';
				$this->_tag['input'] = '';
				$this->_argseparator = '&amp;';
				$this->_cdata = true;
				$this->_escape = false;
				$this->_utf8 = false;
				break;
			default:
				return(false);
		}

		$this->_vendor = $vendor;

		return(true);
	}
*/

	function get_vendor()
	{
		if($this->_vendor === null)
			return(false);

		return($this->_vendor);
	}
}

?>
