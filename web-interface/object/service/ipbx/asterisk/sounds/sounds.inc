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

dwho::load_class('dwho_http');

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_sounds extends xivo_service_asterisk_abstract
{
	var $_name		= 'sounds';
	var $_filter		= false;
	var $_sndpath		= '';
	var $_fileoption	= array();

	function xivo_service_asterisk_sounds(&$sre)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;

		if($this->_chk_sndpath() === false)
			trigger_error('Invalid or missing sounds path in '.$this->_sre->get_type().' init file',E_USER_ERROR);

		$this->_load_config();

		if(($size = $this->_verify_can_upload_file()) === false)
			die();

		$this->_conf['option']['file']['size'] = $size;
	}

	function _chk_sndpath()
	{
		if(dwho_issa($this->_name,$this->_sre->_ini) === false
		|| isset($this->_sre->_ini[$this->_name]['path']) === false
		|| ($this->_sndpath = dwho_file::is_d_rwx($this->_sre->_ini[$this->_name]['path'])) === false)
			return(false);

		return(true);
	}

	function _get_config_file_maxsize()
	{
		$r = false;

		if(($option_file = $this->_chk_exists_cfg('option','file',true)) !== false
		&& isset($option_file['size']) === true)
			$r = dwho_get_memory($import_file['size'],false);

		return($r);
	}

	function _get_option_defined_dir()
	{
		if(($option = $this->get_option_value('defdir')) === false
		|| is_array($option) === false)
			return(false);

		return($option);
	}

	function _get_option_file()
	{
		if(empty($this->_fileoption) === false)
			return($this->_fileoption);
		if(($option = $this->get_option_value('file')) === false)
			return(false);

		if(dwho_has_len($option,'name') === true)
			$this->_fileoption['name'] = $option['name'];

		if(dwho_issa('mime',$option) === true)
			$this->_fileoption['mime'] = array_keys($option['mime']);

		if(dwho_issa('extension',$option) === false)
			return($this->_fileoption);

		$formats = $this->_sre->get_formats();

		if(isset($this->_fileoption['mime']) === false)
			$this->_fileoption['mime'] = array();

		$this->_fileoption['extension'] = array();
		$this->_fileoption['extensionlist'] = array();

		$refmime = &$this->_fileoption['mime'];
		$refext = &$this->_fileoption['extension'];
		$refextlist = &$this->_fileoption['extensionlist'];

		foreach($option['extension'] as $key => $val)
		{
			if($val === true && dwho_issa($key,$formats) === true)
			{
				$refformat = &$formats[$key];

				if(($setext = isset($refformat['extension'])) === true)
				{
					$extension = array_values((array) $refformat['extension']);
					$setext = isset($extension[0]) === true;
				}

				if(($setmime = isset($refformat['mime'])) === true)
				{
					$mime = array_values((array) $refformat['mime']);
					$setmime = isset($mime[0]) === true;
				}

				if($setext === true)
				{
					if($setmime === true)
					{
						$nb = count($extension);

						for($i = 0;$i < $nb;$i++)
							$refext[$extension[$i]] = $mime;
					}
					else
						$refext = array_merge($extension,$refext);

					$refextlist = array_merge($extension,$refextlist);
				}
				else if($setmime === true)
					$refmime = array_merge($mime,$refmime);
			}
			else if(is_array($val) === true)
			{
				$mime = array_values((array) $val);

				$refextlist[] = $key;
				$refext[$key] = $mime;
				$refmime = array_merge($mime,$refmime);
			}
			else if($val !== false)
			{
				$refextlist[] = $key;
				$refext[] = $key;
			}
		}

		if(empty($this->_fileoption['mime']) === true)
			unset($this->_fileoption['mime']);

		if(empty($this->_fileoption['extension']) === true)
			unset($this->_fileoption['extension']);

		if(empty($refextlist) === false)
		{
			$this->_fileoption['extensionregexp'] = '(?:'.implode('|',array_map('preg_quote',
											    $refextlist)).')';

			$this->_fileoption['name'] = '/^['.XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN.']'.
						     XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_LENGTH.
						     '\.'.$this->_fileoption['extensionregexp'].'$/';
		}
		else
			unset($this->_fileoption['extensionlist']);

		return($this->_fileoption);
	}

	function _get_option_dir_name()
	{
		if(($option = $this->get_option_value('dir')) === false
		|| isset($option['name']) === false)
			return(false);

		return($option['name']);
	}

	function _get_option_file_name()
	{
		if($this->_get_option_file() === false
		|| isset($this->_fileoption['name']) === false)
			return(false);

		return($this->_fileoption['name']);
	}

	function _chk_file($name,$dir,$exists=false)
	{
		$name = strval($name);
		$dir = strval($dir);

		if(($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$dir) !== 1
		|| ($match_file = $this->_get_option_file_name()) === false
		|| preg_match($match_file,$name) !== 1)
			return(false);
		else if((bool) $exists === false)
			return(true);

		return(dwho_file::is_f(dwho_file::joinpath($this->_sndpath,$dir,$name)));
	}

	function get($name,$dir)
	{
		$name = strval($name);
		$dir = strval($dir);

		if(($file = $this->_chk_file($name,$dir,true)) === false)
			return(false);

		$r = pathinfo($file);
		$r['path'] = $file;
		$r['dirpath'] = $r['dirname'];
		$r['dirname'] = $dir;
		$r['filename'] = $name;
		$r['basename'] = basename($r['filename'],'.'.$r['extension']);

		return($r);
	}

	function uploaded_fileinfo($name)
	{
		if(($http_response = dwho_http::factory('response')) === false
		|| $this->_get_option_file() === false
		|| ($fileinfo = $http_response->upload_file($name,$this->_fileoption)) === false)
			return(false);

		return($fileinfo);
	}

	function add($name,$tmpname)
	{
		$name = (string) $name;
		$tmpname = (string) $tmpname;

		$file = $this->_sndpath.DWHO_SEP_DIR.$name;

		if(dwho_file::is_f($tmpname) === false || dwho_file::is_f($file) !== false
		|| dwho_file::is_d_rwx($this->_sndpath) === false
		|| move_uploaded_file($tmpname,$file) === false)
			return(false);

		@chmod($file,0664);

		return(true);
	}

	function edit($name,$newname)
	{
		$name = (string) $name;
		$newname = (string) $newname;

		$file = $this->_sndpath.DWHO_SEP_DIR.$name;
		$newfile = $this->_sndpath.DWHO_SEP_DIR.$newname;

		if($file === $newfile)
			return(true);
		else if(dwho_file::is_f($file) === false
		|| dwho_file::is_f($newfile) !== false
		|| dwho_file::is_d_rwx($this->_sndpath) === false
		|| rename($file,$newfile) === false)
			return(false);

		@chmod($newfile,0664);

		return(true);
	}

	function delete($name)
	{
		$name = (string) $name;

		$file = $this->_sndpath.DWHO_SEP_DIR.$name;

		if(dwho_file::is_f($file) === false || dwho_file::is_d_rwx($this->_sndpath) === false)
			return(false);

		return(dwho_file::rm($file));
	}

	function get_dir_path($name)
	{
		$name = strval($name);

		if(($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$name) !== 1
		|| ($path = dwho_file::is_d_rwx($this->_sndpath.DWHO_SEP_DIR.$name)) === false)
			return(false);

		return($path);
	}

	function get_dir($name,$getinfo=false,$search=false)
	{
		$name = strval($name);

		if(($defined_dir = $this->_get_option_defined_dir()) === false
		|| ($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$name) !== 1
		|| ($path = dwho_file::is_d_rwx($this->_sndpath.DWHO_SEP_DIR.$name)) === false)
			return(false);

		$r = array();
		$r['dirname'] = $name;
		$r['path'] = $path;
		$r['nb_files'] = 0;
		$r['defineddir'] = isset($defined_dir[$name]);

		if(($r['files'] = $this->get_list($name,$getinfo,$search)) === false)
			return($r);

		$r['nb_files'] = count($r['files']);

		return($r);
	}

	function add_dir($name)
	{
		$name = (string) $name;

		$dir = $this->_sndpath.DWHO_SEP_DIR.$name;

		if(($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$name) !== 1
		|| dwho_file::is_d_rwx($this->_sndpath) === false
		|| dwho_file::is_d($dir) !== false)
			return(false);

		return(mkdir($dir));
	}

	function edit_dir($name,$newname)
	{
		$name = (string) $name;
		$newname = (string) $newname;

		$dir = $this->_sndpath.DWHO_SEP_DIR.$name;
		$newdir = $this->_sndpath.DWHO_SEP_DIR.$newname;

		if(($defined_dir = $this->_get_option_defined_dir()) === false
		|| isset($defined_dir[$name]) === true
		|| ($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$newname) !== 1
		|| dwho_file::is_d_rwx($this->_sndpath) === false
		|| dwho_file::is_d($dir) === false)
			return(false);
		else if($name === $newname)
			return(true);
		else if(dwho_file::is_d($newdir) !== false)
			return(false);

		return(rename($dir,$newdir));
	}

	function delete_dir($name)
	{
		if(($info = $this->get_dir($name)) === false
		|| ($defined_dir = $this->_get_option_defined_dir()) === false
		|| isset($defined_dir[$info['dirname']]) === true)
			return(false);

		$dirpath = $this->_sndpath.DWHO_SEP_DIR.$info['dirname'];

		if(is_link($dirpath) === true)
			return(false);
		else if($info['files'] === false)
			return(rmdir($dirpath));

		$r = true;

		for($i = 0;$i < $info['nb_files'];$i++)
		{
			if($this->delete($info['dirname'].DWHO_SEP_DIR.$info['files'][$i]) === false)
			{
				$r = false;
				break;
			}
		}

		if($r === true)
			$r = rmdir($dirpath);

		return($r);
	}

	function get_list($dir='',$getinfo=false,$search=false)
	{
		$dir = (string) $dir;

		if(($match_file = $this->_get_option_file_name()) === false)
			return(false);

		if(dwho_has_len($search) === true
		&& isset($this->_fileoption['extensionregexp']) === true
		&& preg_match('/^['.XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN.']+$/',
			      $search) === 1)
			$match_file = '/^(?:.*'.preg_quote($search).'.*)'.
				      XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_LENGTH.
				      '\.'.$this->_fileoption['extensionregexp'].'$/';

		if($dir !== '')
		{
			if(($dirs = $this->get_list_dirs()) === false || in_array($dir,$dirs) === false)
				return(false);

			$dir = $this->_sndpath.DWHO_SEP_DIR.$dir;
		}
		else
			$dir = $this->_sndpath;


		$r = dwho_file::read_d($dir,'file',0,$match_file,true);

		if($r !== false && isset($r[0]) === false)
			$r = false;

		if($r === false || (bool) $getinfo === false)
			return($r);

		$arr = array();

		$nb = count($r);

		for($i = 0;$i < $nb;$i++)
		{
			$file = $dir.DWHO_SEP_DIR.$r[$i];

			if(($info = dwho_file::get_info($file)) === false)
				continue;
			else if($getinfo === true)
				$arr[$file] = $info;
			else if(isset($info[$getinfo]) === true)
				$arr[$info[$getinfo]] = $r[$i];
			else
				$arr[$file] = $r[$i];
		}

		if(empty($arr) === true)
			return(false);

		return($arr);
	}

	function get_list_dirs()
	{
		if(($match_dir = $this->_get_option_dir_name('match_dir')) === false)
			return(false);

		$r = dwho_file::read_d($this->_sndpath,'dir',(FILE_R_OK | FILE_W_OK | FILE_X_OK),$match_dir,true);

		if($r !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_list_dirs_files()
	{
		if(($list = $this->get_list_dirs()) === false || ($nb = count($list)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$name = &$list[$i];

			if(($info = $this->get_dir($name)) !== false)
				$r[] = $info;
		}

		return($r);
	}
}

?>
