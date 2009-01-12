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

class xivo_tpl_xmlphone extends xivo_tpl_module
{
	var $_vendor		= null;
	var $_tag		= array();
	var $_argseparator	= '&amp;';
	var $_cdata		= false;
	var $_escape		= true;
	var $_utf8		= true;

	function xivo_tpl_xmlphone(&$tpl,$param=array())
	{
		$this->_tpl = &$tpl;
		$this->load_init($param);
	}

	function load_init($param)
	{
		if(is_array($param) === false
		|| isset($param['vendor']) === false
		|| $this->_load_vendor($param['vendor']) === false)
		{
			$this->_vendor = null;
			return(false);
		}

		return(true);
	}

	function chk_vendor($vendor)
	{
		$vendor = strtolower($vendor);

		switch($vendor)
		{
			case 'thomson':
			case 'snom':
			case 'aastra':
				return($vendor);
		}

		return(false);
	}

	function _load_vendor($vendor)
	{
		if(($vendor = $this->chk_vendor($vendor)) === false)
			return(false);

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
			case 'thomson':
				$this->_tag['menu'] = 'ThomsonPhoneMenu';
				$this->_tag['directory'] = 'ThomsonPhoneBook';
				$this->_tag['input'] = '';
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
			default:
				return(false);
		}

		$this->_vendor = $vendor;

		return(true);
	}

	function get_vendor()
	{
		if($this->_vendor === null)
			return(false);

		return($this->_vendor);
	}

	function get_header_contenttype()
	{
		$charset = (bool) $this->_utf8 === true ? 'utf-8' : 'iso-8859-15';

		return('Content-Type: text/xml; charset='.$charset);
	}

	function get_argseparator()
	{
		return($this->_argseparator);
	}

	function get_tag($name)
	{
		if($this->_vendor === null || isset($this->_tag[$name]) === false)
			return(false);

		return($this->_tag[$name]);
	}

	function is_utf8()
	{
		return((bool) $this->_utf8);
	}

	function escape($str,$force=true)
	{
		if(is_scalar($str) === false)
			return('');
		else if((bool) $this->_cdata === true)
			$str = '<![CDATA['.$str.']]>';
		else if((bool) $this->_escape === true || (bool) $force === true)
			$str = xivo_htmlsc($str,ENT_NOQUOTES);

		if((bool) $this->_utf8 === false)
			$str = utf8_decode($str);

		return($str);
	}
}

?>
