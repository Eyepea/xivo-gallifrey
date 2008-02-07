<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_sounds extends xivo_service_asterisk_abstract
{
	var $_name	= 'sounds';
	var $_filter	= false;
	var $_sndpath	= '';

	function xivo_service_asterisk_sounds(&$sre)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;

		if($this->_chk_sndpath() === false)
			trigger_error('Invalid or missing sounds path in '.$this->_sre->get_type().' ini file',E_USER_ERROR);

		$this->_load_config();

		if(($size = $this->_chk_upload_file()) === false)
			die();

		$this->_conf['option']['file']['size'] = $size;
	}

	function _chk_sndpath()
	{
		if(xivo_issa($this->_name,$this->_sre->_ini) === false
		|| isset($this->_sre->_ini[$this->_name]['path']) === false
		|| ($this->_sndpath = xivo_file::is_d_rwx($this->_sre->_ini[$this->_name]['path'])) === false)
			return(false);
	}

	function _get_config_file_maxsize()
	{
		$r = false;

		if(($option_file = $this->_chk_exists_cfg('option','file',true)) !== false
		&& isset($option_file['size']) === true)
			$r = xivo_get_memory($import_file['size'],false);

		return($r);
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
		if(($option = $this->get_option_value('file')) === false
		|| isset($option['name']) === false)
			return(false);

		return($option['name']);
	}

	function _get_option_file_extension()
	{
		if(($option = $this->get_option_value('file')) === false
		|| isset($option['extension']) === false)
			return(false);

		$r = (array) $option['extension'];

		if(isset($r[0]) === false)
			$r = array_keys($r);

		if(isset($r[0]) === false)
			return(false);

		return($r);
	}

	function _chk_file($name,$dir,$exists=false)
	{
		$name = strval($name);
		$dir = strval($dir);

		if(($extensions = $this->_get_option_file_extension()) === false
		|| ($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$dir) !== 1
		|| ($match_file = $this->_get_option_file_name()) === false
		|| preg_match($match_file,$name) !== 1
		|| ($pos = strrpos($name,'.')) === false)
			return(false);

		$ext = strtolower(substr($name,$pos + 1));

		if(in_array($ext,$extensions) === false)
			return(false);
		else if((bool) $exists === false)
			return(true);

		return(xivo_file::is_f($this->_sndpath.XIVO_SEP_DIR.$dir.XIVO_SEP_DIR.$name));
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

	function add($name,$tmpname)
	{
		$name = (string) $name;
		$tmpname = (string) $tmpname;

		$file = $this->_sndpath.XIVO_SEP_DIR.$name;

		if(xivo_file::is_f($tmpname) === false || xivo_file::is_f($file) !== false
		|| xivo_file::is_d_rwx($this->_sndpath) === false
		|| move_uploaded_file($tmpname,$file) === false)
			return(false);

		chmod($file,0664);

		return(true);
	}

	function edit($name,$newname)
	{
		$name = (string) $name;
		$newname = (string) $newname;

		$file = $this->_sndpath.XIVO_SEP_DIR.$name;
		$newfile = $this->_sndpath.XIVO_SEP_DIR.$newname;

		if($file === $newfile)
			return(true);
		else if(xivo_file::is_f($file) === false
		|| xivo_file::is_f($newfile) !== false
		|| xivo_file::is_d_rwx($this->_sndpath) === false
		|| rename($file,$newfile) === false)
			return(false);

		chmod($newfile,0664);

		return(true);
	}

	function delete($name)
	{
		$name = (string) $name;

		$file = $this->_sndpath.XIVO_SEP_DIR.$name;

		if(xivo_file::is_f($file) === false || xivo_file::is_d_rwx($this->_sndpath) === false)
			return(false);

		return(xivo_file::rm($file));
	}

	function get_dir($name,$getpath=false,$getfull=false)
	{
		$name = strval($name);

		if(($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$name) !== 1
		|| ($path = xivo_file::is_d_rwx($this->_sndpath.XIVO_SEP_DIR.$name)) === false)
			return(false);

		$r = array();
		$r['dirname'] = $name;
		$r['path'] = $path;
		$r['nb_files'] = 0;

		if(($r['files'] = $this->get_list($name,$getpath,$getfull)) === false)
			return($r);	

		$r['nb_files'] = count($r['files']);

		return($r);
	}

	function add_dir($name)
	{
		$name = (string) $name;

		$dir = $this->_sndpath.XIVO_SEP_DIR.$name;

		if(($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$name) !== 1
		|| xivo_file::is_d_rwx($this->_sndpath) === false
		|| xivo_file::is_d($dir) !== false) 
			return(false);

		return(mkdir($dir));
	}

	function edit_dir($name,$newname)
	{
		$name = (string) $name;
		$newname = (string) $newname;

		$dir = $this->_sndpath.XIVO_SEP_DIR.$name;
		$newdir = $this->_sndpath.XIVO_SEP_DIR.$newname;

		if(($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$newname) !== 1
		|| xivo_file::is_d_rwx($this->_sndpath) === false
		|| xivo_file::is_d($dir) === false)
			return(false);
		else if($name === $newname)
			return(true);
		else if(xivo_file::is_d($newdir) !== false)
			return(false);

		return(rename($dir,$newdir));
	}

	function delete_dir($name)
	{
		$name = (string) $name;

		if(($info = $this->get_dir($name)) === false)
			return(false);
		else if($info['files'] === false)
			return(rmdir($this->_sndpath.XIVO_SEP_DIR.$name));

		$r = true;

		for($i = 0;$i < $info['nb_files'];$i++)
		{
			if($this->delete($name.XIVO_SEP_DIR.$info['files'][$i]) === false)
			{
				$r = false;
				break;
			}
		}

		if($r === true)
			$r = rmdir($this->_sndpath.XIVO_SEP_DIR.$name);

		return($r);
	}

	function get_list($dir='',$getpath=false,$getfull=false)
	{
		$dir = (string) $dir;
		$getfull = (bool) $getfull;

		if(($match_file = $this->_get_option_file_name()) === false)
			return(false);
		else if($dir !== '')
		{
			if(($dirs = $this->get_list_dirs()) === false || in_array($dir,$dirs) === false)
				return(false);

			$dir = $this->_sndpath.XIVO_SEP_DIR.$dir;
		}
		else
			$dir = $this->_sndpath;

		$r = xivo_file::read_d($dir,'file',null,$match_file,true);

		if($r !== false && isset($r[0]) === false)
			$r = false;

		if($r === false || (bool) $getpath === false)
			return($r);

		$arr = array();

		$nb = count($r);

		for($i = 0;$i < $nb;$i++)
		{
			$file = $dir.XIVO_SEP_DIR.$r[$i];

			if($getfull === true)
				$info = xivo_file::get_info($file);
			else
				$info = $r[$i];

			if(($pos = strrpos($file,'.')) !== false)
				$file = substr($file,0,$pos);

			$arr[$file] = $info;
		}

		return($arr);
	}

	function get_list_dirs()
	{
		if(($match_dir = $this->_get_option_dir_name('match_dir')) === false)
			return(false);

		$r = xivo_file::read_d($this->_sndpath,'dir','rwx',$match_dir,true);

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
