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

require_once(dwho_file::joinpath(DWHO_PATH_ROOT,'tpl','module','xmlphone','abstract','abstract.inc'));

class dwho_tpl_xmlphone_cisco extends dwho_tpl_xmlphone_abstract
{
	var $_vendor		= 'cisco';
	var $_model		    = null;
	var $_argseparator	= '&';
	var $_cdata		    = false;
	var $_escape		= false;
	var $_utf8		    = true;
	var $_noaccents		= true;
	var $_tags		= array('directory'	=> 'CiscoIPPhoneDirectory',
					'menu'		=> 'CiscoIPPhoneMenu',
					'input'		=> 'CiscoIPPhoneInput');


	function dwho_tpl_xmlphone_cisco()
	{
		$this->_model = $this->get_model();
	}

	function get_model()
	{
		if(isset($_SERVER['HTTP_USER_AGENT']) === false
		|| preg_match('/Allegro-/i',
			      $_SERVER['HTTP_USER_AGENT']) !== 1)
			return(null);

		return('cisco7960');
	}
}

?>
