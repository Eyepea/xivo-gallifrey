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

xivo_service_asterisk::required(array('staticconf','staticconf.inc'),true);

class xivo_service_asterisk_musiconhold extends xivo_service_asterisk_staticconf
{
	var $_dso		= null;
	var $_name		= 'musiconhold';
	var $_filename		= 'musiconhold.conf';
	var $_filter		= false;
	var $_file		= false;
	var $_mohpath		= '';
	var $_musiconhold	= array();
	var $_fileoption	= array();

	function xivo_service_asterisk_musiconhold(&$sre,&$dso)
	{
		$this->_load_staticconf_config();

		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_dso->set_param('filename',$this->_filename);

		if($this->_chk_mohpath() === false)
			trigger_error('Invalid or missing music on hold path in '.$this->_sre->get_type().' init file',E_USER_ERROR);

		$this->_load_config();

		$this->_set_element_default('directory',$this->_mohpath);

		if(($size = $this->_chk_upload_file()) === false)
			die();

		$this->_conf['option']['file']['size'] = $size;
		$this->_musiconhold['conf'] = $this->_conf;
	}

	function _chk_mohpath()
	{
		if(dwho_issa($this->_name,$this->_sre->_ini) === false
		|| isset($this->_sre->_ini[$this->_name]['path']) === false
		|| ($this->_mohpath = dwho_file::is_d_rwx($this->_sre->_ini[$this->_name]['path'])) === false)
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
			$this->_fileoption['name'] = '/^['.XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN.']'.
						     XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_LENGTH.
						     '\.(?:'.implode('|',array_map('preg_quote',
										   $refextlist)).')$/';
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

	function _chk_dir($dir,$exists=false)
	{
		if(($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$dir) !== 1)
			return(false);
		else if((bool) $exists === false)
			return(true);

		return(dwho_file::is_d_rwx($this->_mohpath.DWHO_SEP_DIR.$dir));
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

		return(dwho_file::is_f(dwho_file::joinpath($this->_mohpath,$dir,$name)));
	}

	function get_category($id,$disable=null)
	{
		$r = array();

		if(($r['dir'] = $this->get_dir($id)) === false || ($r['cat'] = parent::get_category($id)) === false)
			return(false);

		return($r);
	}

	function add_category($arr)
	{
		if(is_array($arr) === false)
			return(false);

		$category = $arr['category'];

		if($this->add_dir($category) === false)
			return(false);

		$arr['directory'] = $this->_mohpath.DWHO_SEP_DIR.$category;

		if(($r = parent::get_category($category)) !== false)
		{
			parent::edit_category($category,$arr);
			return($r);
		}

		unset($arr['category']);

		if(($r = parent::add_category($category,$arr)) !== false)
			return($r);

		$this->delete_dir($category);

		return(false);
	}

	function edit_category($id,$arr)
	{
		if(is_array($arr) === false)
			return(false);

		$category = $arr['category'];

		if($this->edit_dir($id,$category) === false)
			return(false);

		$arr['directory'] = $this->_mohpath.DWHO_SEP_DIR.$category;

		if(strtolower($id) !== strtolower($category) && parent::get_category($category) !== false)
			parent::delete_by_category($category);

		if(($r = parent::edit_category($id,$arr)) !== false)
			return($r);

		$this->edit_dir($category,$id);

		return(false);
	}

	function delete_category($id)
	{
		if(($list = $this->get_all_where(array('category' => $id))) === false
		|| parent::delete_by_category($id) === false)
			return(false);
		else if($this->delete_dir($id) === true)
			return(true);

		$nb = count($list);

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$list[$i];
			parent::add($ref,$ref['id']);
		}

		return(false);
	}

	function get_file($name,$dir)
	{
		$name = (string) $name;
		$dir = (string) $dir;

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

	function get_upload($name)
	{
		if((is_object($this->_file) === false
		   && ($this->_file = new dwho_file()) === false) === true
		|| $this->_get_option_file() === false
		|| $this->_file->get_upload($name,$this->_fileoption) === false)
			return(false);

		return($this->_file->info);
	}

	function add_file($name,$tmpname)
	{
		$name = (string) $name;
		$tmpname = (string) $tmpname;

		$file = $this->_mohpath.DWHO_SEP_DIR.$name;

		if(dwho_file::is_f($tmpname) === false || dwho_file::is_f($file) !== false
		|| dwho_file::is_d_rwx($this->_mohpath) === false
		|| move_uploaded_file($tmpname,$file) === false)
			return(false);

		@chmod($file,0664);

		return(true);
	}

	function edit_file($name,$newname)
	{
		$name = (string) $name;
		$newname = (string) $newname;

		$file = $this->_mohpath.DWHO_SEP_DIR.$name;
		$newfile = $this->_mohpath.DWHO_SEP_DIR.$newname;

		if($file === $newfile)
			return(true);
		else if(dwho_file::is_f($file) === false
		|| dwho_file::is_f($newfile) !== false
		|| dwho_file::is_d_rwx($this->_mohpath) === false
		|| rename($file,$newfile) === false)
			return(false);

		@chmod($newfile,0664);

		return(true);
	}

	function delete_file($name)
	{
		$name = (string) $name;

		$file = $this->_mohpath.DWHO_SEP_DIR.$name;

		if(dwho_file::is_f($file) === false || dwho_file::is_d_rwx($this->_mohpath) === false)
			return(false);

		return(dwho_file::rm($file));
	}

	function get_dir($name)
	{
		$name = strval($name);

		if(($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$name) !== 1
		|| ($path = dwho_file::is_d_rwx($this->_mohpath.DWHO_SEP_DIR.$name)) === false)
			return(false);

		$r = array();
		$r['dirname'] = $name;
		$r['path'] = $path;
		$r['nb_files'] = 0;

		if(($r['files'] = $this->get_list_files($name)) === false)
			return($r);

		$r['nb_files'] = count($r['files']);

		return($r);
	}

	function add_dir($name)
	{
		$dir = $this->_mohpath.DWHO_SEP_DIR.$name;

		if(($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$name) !== 1
		|| dwho_file::is_d_rwx($this->_mohpath) === false)
			return(false);
		else if(dwho_file::is_d($dir) === false)
			$r = mkdir($dir);
		else if(dwho_file::is_d_rwx($dir) !== false)
			$r = true;

		@chmod($dir,02775);

		return($r);
	}

	function edit_dir($name,$newname)
	{
		$name = (string) $name;
		$newname = (string) $newname;

		$dir = $this->_mohpath.DWHO_SEP_DIR.$name;
		$newdir = $this->_mohpath.DWHO_SEP_DIR.$newname;

		if(($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$newname) !== 1
		|| dwho_file::is_d_rwx($this->_mohpath) === false
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
		if(($info = $this->get_dir($name)) === false)
			return(false);

		$dirpath = $this->_mohpath.DWHO_SEP_DIR.$info['dirname'];

		if(is_link($dirpath) === true)
			return(false);
		else if($info['files'] === false)
			return(rmdir($dirpath));

		$r = true;

		for($i = 0;$i < $info['nb_files'];$i++)
		{
			if($this->delete_file($info['dirname'].DWHO_SEP_DIR.$info['files'][$i]) === false)
			{
				$r = false;
				break;
			}
		}

		if($r === true)
			$r = rmdir($dirpath);

		return($r);
	}

	function get_all_by_category()
	{
		if(($list = parent::get_name_val_by_category()) === false
		|| is_array($list) === false
		|| empty($list) === true)
			return(false);

		$r = array();

		foreach($list as $key => $val)
		{
			$val['category'] = $key;

			if(($val['mode'] === 'custom' && dwho_has_len($val,'directory') === false) === true
			|| $this->_chk_dir($key,true) !== false)
				$r[] = $val;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_category($arr=array(),$disable=null)
	{
		if(($list = $this->_prepare_get_all_where($arr,$disable)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_chk_dir($list[$i]['category'],true) !== false)
				$r[$list[$i]['category']] = $list[$i];
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_list_files($dir='')
	{
		$dir = (string) $dir;

		if(($match_file = $this->_get_option_file_name()) === false)
			return(false);
		else if($dir !== '')
		{
			if(($dirs = $this->get_list_dirs()) === false || in_array($dir,$dirs) === false)
				return(false);

			$dir = $this->_mohpath.DWHO_SEP_DIR.$dir;
		}
		else
			$dir = $this->_mohpath;

		$r = dwho_file::read_d($dir,'file',0,$match_file,true);

		if($r !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_list_dirs()
	{
		if(($match_dir = $this->_get_option_dir_name('match_dir')) === false)
			return(false);

		$r = dwho_file::read_d($this->_mohpath,'dir',(FILE_R_OK | FILE_W_OK | FILE_X_OK),$match_dir,true);

		if($r !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_list_where($arr=array(),$disable=null)
	{
		$this->_conf = &$this->_staticconf['conf'];

		$r = parent::get_list_where($arr,$disable);

		$this->_conf = &$this->_musiconhold['conf'];

		return($r);
	}

	function _prepare_get_all_where($arr=array(),$disable=null)
	{
		$this->_conf = &$this->_staticconf['conf'];

		$r = parent::_prepare_get_all_where($arr,$disable);

		$this->_conf = &$this->_musiconhold['conf'];

		return($r);
	}

	function get_all_where($arr=array(),$disable=null)
	{
		$this->_conf = &$this->_staticconf['conf'];

		$r = parent::get_all_where($arr,$disable);

		$this->_conf = &$this->_musiconhold['conf'];

		return($r);
	}
}

?>
