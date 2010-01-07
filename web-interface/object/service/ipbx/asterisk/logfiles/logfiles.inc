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

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_logfiles extends xivo_service_asterisk_abstract
{
	var $_name	= 'logfiles';
	var $_filter	= false;
	var $_logpath	= '';

	function xivo_service_asterisk_logfiles(&$sre)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;

		if($this->_chk_logpath() === false)
			trigger_error('Invalid or missing configuration path in '.$this->_sre->get_type().' init file',E_USER_ERROR);

		$this->_load_config();
	}

	function _chk_logpath()
	{
		if(dwho_issa($this->_name,$this->_sre->_ini) === false
		|| isset($this->_sre->_ini[$this->_name]['path']) === false
		|| ($this->_logpath = dwho_file::is_d_r($this->_sre->_ini[$this->_name]['path'])) === false)
			return(false);
	}

	function _chk_file($name)
	{
		$name = basename(strval($name));

		if(preg_match('/^[a-zA-Z0-9_\-\.]+(?:\.tgz|\.\d+)?$/',$name) === 1)
			return(dwho_file::is_f_r($this->_logpath.DWHO_SEP_DIR.$name));

		return(false);
	}

	function get($name)
	{
		if(($file = $this->_chk_file($name)) === false)
			return(false);

		$r = array();
		$r['name'] = $name;
		$r['file'] = $file;

		return($r);
	}

	function get_list()
	{
		$r = dwho_file::read_d($this->_logpath,'file',FILE_R_OK,'/^[a-zA-Z0-9_\-\.]+(?:\.tgz|\.\d+)?$/',false,true);

		if($r !== false && isset($r[0]) === false)
			return(false);

		return($r);
	}
}

?>
