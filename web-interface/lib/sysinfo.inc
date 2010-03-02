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

define('ARGIN_SEPARATOR','&');
define('ARGOUT_SEPARATOR','&');
define('DWHO_PHP_VER_MIN','4.4');
define('DWHO_PHP_VER_MAX','5.3.2');

class dwho_sysinfo
{
	var $_sysinfo	= null;
	var $_net	= null;

	function dwho_sysinfo()
	{
		$os = strtolower(PHP_OS);

		if(dwho::load_class('dwho::sysinfo::'.$os) === false)
			trigger_error('Failed to load SysInfo class for '.PHP_OS,E_USER_ERROR);

		$name = 'dwho_sysinfo_'.$os;

		$this->_sysinfo = new $name();
	}

	function os_name()
	{
		return(PHP_OS);
	}

	function os_meta()
	{
		return(strtolower(substr(PHP_OS,0,3)));
	}

	function php_version()
	{
		return(PHP_VERSION);
	}

	function argin_separator()
	{
		if(($sep = ini_get('arg_separator.input')) !== false
		&& dwho_has_len($sep) === true
		&& $sep !== '&amp;')
			return($sep);

		return(ARGIN_SEPARATOR);
	}

	function argout_separator()
	{
		if(($sep = ini_get('arg_separator.output')) !== false
		&& dwho_has_len($sep) === true
		&& $sep !== '&amp;')
			return($sep);

		return(ARGOUT_SEPARATOR);
	}

	function directory_separator()
	{
		return(DIRECTORY_SEPARATOR);
	}

	function path_separator()
	{
		return(PATH_SEPARATOR);
	}

	function eol()
	{
		return(PHP_EOL);
	}

	function sapi_name()
	{
		return(PHP_SAPI);
	}

	function sapi_mode()
	{
		switch(PHP_SAPI)
		{
			case 'cli':
				return(PHP_SAPI);
		}

		return('default');
	}

	function uptime()
	{
		if(isset($this->_sysinfo) === true && is_object($this->_sysinfo) === true)
			return($this->_sysinfo->uptime());

		return(false);
	}

	function cpustats()
	{
		if(isset($this->_sysinfo) === true && is_object($this->_sysinfo) === true)
			return($this->_sysinfo->cpustats());

		return(false);
	}

	function memstats($byte=false)
	{
		if(isset($this->_sysinfo) === true && is_object($this->_sysinfo) === true)
			return($this->_sysinfo->memstats($byte));

		return(false);
	}

	function net()
	{
		if(isset($this->_sysinfo) === false
		|| is_object($this->_sysinfo) === false)
			return(false);

		if(isset($this->_net) === false)
			$this->_net = $this->_sysinfo->net();

		if(is_object($this->_net) === true)
			return($this->_net);

		return(false);
	}

	function netinfo()
	{
		if($this->net() !== false)
			return($this->_net->netinfo());

		return(false);
	}

	function netstats()
	{
		if($this->net() !== false)
			return($this->_net->netstats());

		return(false);
	}
}

?>
