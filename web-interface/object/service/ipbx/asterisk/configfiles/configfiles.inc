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

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

dwho::load_class('dwho_http');

class xivo_service_asterisk_configfiles extends xivo_service_asterisk_abstract
{
	var $_name	= 'configfiles';
	var $_filter	= false;
	var $_cfgpath	= '';
	var $_error	= array();
	var $_conf	= array();	

	function xivo_service_asterisk_configfiles(&$sre)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;

		if($this->_chk_cfgpath() === false)
			trigger_error('Invalid or missing configuration path in '.$this->_sre->get_type().' init file',E_USER_ERROR);

		$this->_load_config();

		if(($size = $this->_verify_can_upload_file()) === false)
			die();

		$this->_conf['import']['file']['size'] = $size;
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

	function _checkfile($shortname)
	{
		$shortname = trim($shortname);
		if(strlen($shortname) == 0)
		{ $this->_error['filename'] = 'empty'; return false; }
			
		if(strpos($shortname, '.conf') !== strlen($shortname) - 5)
			$shortname .= '.conf';

		if(preg_match('/^[a-zA-Z0-9_-]+\.conf$/',$shortname) !== 1)
		{ $this->_error['filename'] = 'regexp'; return false; }
		
		$filename = $this->_cfgpath.DWHO_SEP_DIR.$shortname;
		if(dwho_file::is_f($filename) || dwho_file::is_d($filename))
		{ $this->_error['filename'] = 'alreadyexists'; return false; }

		return $filename;
	}

	function add($shortname, $contents)
	{
		$this->_error = array();
		if(($filename = $this->_checkfile($shortname)) === false)
			return false;

		if(!touch($filename))
		{ $this->_error['filename'] = 'creationerror'; return false; }

		if(dwho_file::put_contents($filename,dwho_eol($contents)) === false)
		{ return false; }

		@chmod($filename, 0664);
		@chown($filename, 'asterisk');

		return true;
	}

	function _get_config_file_maxsize()
	{
		if(($import_file = $this->_chk_exists_cfg('import','file',true)) !== false
		&& isset($import_file['size']) === true)
			return(dwho_get_memory($import_file['size'],false));

		return(false);
	}

	function get_config_import_file()
	{
		return($this->_chk_exists_cfg('import','file',true));
	}

	function import()
	{
		$this->_error = array();

		if(($http_response = dwho_http::factory('response')) === false
		|| ($fileinfo = $http_response->upload_file('import',
							    $this->_conf['import']['file'])) === false)
		{ $this->_error['import'] = 'empty'; return(false); }
		
		$shortname = $fileinfo['name'];
		if(($pos = strpos($shortname, '.')) !== null)
			$shortname = substr($shortname, 0, $pos);
		$shortname .= ".conf";

		if(($filename = $this->_checkfile($shortname)) === false)
			return false;
		if(!move_uploaded_file($fileinfo['tmp_name'], $filename))
		{ $this->_error['import'] = 'creationerror'; return false; }

		return true;
	}

	function delete($filename)
	{
		$this->_error = array();
		$filename = trim($filename);

		if(preg_match('/^[a-zA-Z0-9_-]+\.conf$/',$filename) !== 1)
		{ $this->_error['filename'] = 'regexp'; return false; }

		$filename = $this->_cfgpath.DWHO_SEP_DIR.$filename;
		if(!dwho_file::is_f($filename))
		{ $this->_error['filename'] = 'notexists'; return false; }

		return(dwho_file::rm($filename));
	}

	function get_list()
	{
		$r = dwho_file::read_d($this->_cfgpath,'file',(FILE_R_OK | FILE_W_OK),'/^[a-zA-Z0-9_-]+\.conf$/',true);

		if($r !== false && isset($r[0]) === false)
			return(false);

		return($r);
	}

	function get_error()
	{ return($this->_error); }
}

?>
