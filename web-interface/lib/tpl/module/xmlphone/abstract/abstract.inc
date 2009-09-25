<?php

#
# XiVO Web-Interface
# Copyright (C) 2009  Proformatique <technique@proformatique.com>
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

class dwho_tpl_xmlphone_abstract
{
	var $_argseparator	= '&amp;';
	var $_cdata		= false;
	var $_escape		= true;
	var $_utf8		= true;

	function has_cdata()
	{
		return($this->_cdata);
	}

	function has_escape()
	{
		return($this->_escape);
	}

	function has_utf8()
	{
		return($this->_utf8);
	}

	function arg_separator()
	{
		return($this->_argseparator);
	}

	function get_vendor()
	{
		return($this->_vendor);
	}

	function get_header_contenttype()
	{
		$charset = $this->has_utf8() === true ? 'utf-8' : 'iso-8859-15';

		return('Content-Type: text/xml; charset='.$charset);
	}

	function escape($str,$force=true)
	{
		if(is_scalar($str) === false)
			return('');
		else if($this->has_cdata() === true)
			$str = '<![CDATA['.$str.']]>';
		else if($this->has_escape() === true || (bool) $force === true)
			$str = dwho_htmlsc($str,ENT_NOQUOTES);

		if($this->has_utf8() === false)
			$str = utf8_decode($str);

		return($str);
	}
}

?>
