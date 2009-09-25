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

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_configfiles extends xivo_service_asterisk_abstract
{
	var $_name	= 'configfiles';
	var $_filter	= false;
	var $_cfgpath	= '';

	function xivo_service_asterisk_configfiles(&$sre)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;

		if($this->_chk_cfgpath() === false)
			trigger_error('Invalid or missing configuration path in '.$this->_sre->get_type().' init file',E_USER_ERROR);

		$this->_load_config();
	}

	function _chk_cfgpath()
	{
		if(dwho_issa($this->_name,$this->_sre->_ini) === false
		|| isset($this->_sre->_ini[$this->_name]['path']) === false
		|| ($this->_cfgpath = dwho_file::is_d_rwx($this->_sre->_ini[$this->_name]['path'])) === false)
			return(false);
	}

	function _chk_file($name)
	{
		$name = basename(strval($name));

		if(preg_match('/^[a-zA-Z0-9_-]+\.conf$/',$name) === 1)
			return(dwho_file::is_f_rw($this->_cfgpath.DWHO_SEP_DIR.$name));

		return(false);
	}

	function get($name)
	{
		if(($file = $this->_chk_file($name)) === false
		|| ($content = file_get_contents($file)) === false)
			return(false);

		$r = array();
		$r['name'] = $name;
		$r['file'] = $file;
		$r['content'] = &$content;

		return($r);
	}

	function edit($name,$contents)
	{
		if(($file = $this->_chk_file($name)) !== false)
			return(dwho_file::put_contents($file,dwho_eol($contents)));

		return(false);
	}

	function get_list()
	{
		$r = dwho_file::read_d($this->_cfgpath,'file',(FILE_R_OK | FILE_W_OK),'/^[a-zA-Z0-9_-]+\.conf$/',true);

		if($r !== false && isset($r[0]) === false)
			return(false);

		return($r);
	}
}

?>
