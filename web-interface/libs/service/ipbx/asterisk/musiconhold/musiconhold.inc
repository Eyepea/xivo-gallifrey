<?php

require_once(XIVO_PATH_LIBS.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'staticconf'.XIVO_SEP_DIR.'staticconf.inc');

class xivo_service_asterisk_musiconhold extends xivo_service_asterisk_staticconf
{
	var $_dso	= null;
	var $_name	= 'musiconhold';
	var $_filename	= 'musiconhold.conf';
	var $_filter	= null;
	var $_mohpath	= '';

	function xivo_service_asterisk_musiconhold(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		if($this->_chk_mohpath() === false)
			trigger_error('Invalid or missing music on hold path in '.$this->_sre->get_type().' init file',E_USER_ERROR);

		$this->_load_config();

		$this->_set_element_default('directory',$this->_mohpath);
	}

	function _chk_mohpath()
	{
		if(xivo_issa('general',$this->_sre->_ini) === false
		|| xivo_issa('path',$this->_sre->_ini['general']) === false
		|| isset($this->_sre->_ini['general']['path']['musiconhold']) === false
		|| ($this->_mohpath = xivo_file::is_d_rwx($this->_sre->_ini['general']['path']['musiconhold'])) === false)
			return(false);
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

	function _chk_dir($dir,$exists=false)
	{
		$r = false;

		if(($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$dir) !== 1)
			return($r);

		if($exists === true)
			$r = xivo_file::is_d_rwx($this->_mohpath.XIVO_SEP_DIR.$dir);
		else
			$r = true;

		return($r);
	}

	function _chk_file($name,$dir,$exists=false)
	{
		$r = false;

		$name = (string) $name;
		$dir = (string) $dir;
		$exists = (bool) $exists;

		if(($extensions = $this->_get_option_file_extension()) === false
		|| ($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$dir) !== 1
		|| ($match_file = $this->_get_option_file_name()) === false
		|| preg_match($match_file,$name) !== 1
		|| ($pos = strrpos($name,'.')) === false)
			return($r);

		$ext = strtolower(substr($name,$pos + 1));

		if(in_array($ext,$extensions) === false)
			return($r);

		if($exists === true)
			$r = xivo_file::is_f_rw($this->_mohpath.XIVO_SEP_DIR.$dir.XIVO_SEP_DIR.$name);
		else
			$r = true;

		return($r);
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
		$r = false;

		if(is_array($arr) === false)
			return($r);

		$category = $arr['category'];
		unset($arr['category']);

		if($this->add_dir($category) === false)
			return($r);

		$arr['directory'] = $this->_mohpath.XIVO_SEP_DIR.$category;

		if(($r = parent::add_category($category,$arr)) !== false)
			return($r);

		$this->delete_dir($category);

		return($r);
	}

	function edit_category($id,$arr)
	{
		$r = false;

		if(is_array($arr) === false)
			return($r);

		$category = $arr['category'];

		if($this->edit_dir($id,$category) === false)
			return($r);

		$arr['directory'] = $this->_mohpath.XIVO_SEP_DIR.$category;

		if(($r = parent::edit_category($id,$arr)) !== false)
			return($r);

		$this->edit_dir($category,$id);

		return($r);
	}

	function delete_category($id)
	{
		if(($list = $this->get_all_where(array('category' => $id))) === false
		|| parent::delete_by_category($id) === false)
			return(false);

		if($this->delete_dir($id) === true)
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
		$r = false;

		$name = (string) $name;
		$dir = (string) $dir;

		if(($file = $this->_chk_file($name,$dir,true)) === false)
			return($r);

		$r = pathinfo($file);
		$r['path'] = $file;
		$r['dirpath'] = $r['dirname'];
		$r['dirname'] = $dir;
		$r['filename'] = $name;
		$r['basename'] = basename($r['filename'],'.'.$r['extension']);

		return($r);
	}

	function add_file($name,$tmpname)
	{
		$r = false;

		$name = (string) $name;
		$tmpname = (string) $tmpname;

		$file = $this->_mohpath.XIVO_SEP_DIR.$name;

		if(xivo_file::is_f($tmpname) === false || xivo_file::is_f($file) !== false
		|| xivo_file::is_d_rwx($this->_mohpath) === false)
			return($r);

		return(move_uploaded_file($tmpname,$file));
	}

	function edit_file($name,$newname)
	{
		$r = false;

		$name = (string) $name;
		$newname = (string) $newname;

		$file = $this->_mohpath.XIVO_SEP_DIR.$name;
		$newfile = $this->_mohpath.XIVO_SEP_DIR.$newname;

		if(xivo_file::is_f($file) === false || xivo_file::is_f($newfile) !== false
		|| xivo_file::is_d_rwx($this->_mohpath) === false)
			return($r);

		return(rename($file,$newfile));
	}

	function delete_file($name)
	{
		$r = false;

		$name = (string) $name;

		$file = $this->_mohpath.XIVO_SEP_DIR.$name;

		if(xivo_file::is_f($file) === false || xivo_file::is_d_rwx($this->_mohpath) === false)
			return($r);

		return(xivo_file::rm($file));
	}

	function get_dir($name)
	{
		$r = false;

		$name = (string) $name;

		if(($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$name) !== 1)
			return($r);

		if(($path = xivo_file::is_d_rwx($this->_mohpath.XIVO_SEP_DIR.$name)) === false)
			return($r);

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
		$r = false;

		$name = (string) $name;

		$dir = $this->_mohpath.XIVO_SEP_DIR.$name;

		if(($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$name) !== 1
		|| xivo_file::is_d_rwx($this->_mohpath) === false
		|| xivo_file::is_d($dir) !== false) 
			return($r);

		$r = mkdir($dir);

		return($r);
	}

	function edit_dir($name,$newname)
	{
		$r = false;

		$name = (string) $name;
		$newname = (string) $newname;

		$dir = $this->_mohpath.XIVO_SEP_DIR.$name;

		if(($match_dir = $this->_get_option_dir_name()) === false
		|| preg_match($match_dir,$newname) !== 1
		|| xivo_file::is_d_rwx($this->_mohpath) === false
		|| xivo_file::is_d($dir) === false)
			return($r);

		if($name === $newname)
			return(true);

		$r = rename($dir,$this->_mohpath.XIVO_SEP_DIR.$newname);

		return($r);
	}

	function delete_dir($name)
	{
		$r = false;

		$name = (string) $name;

		if(($info = $this->get_dir($name)) === false)
			return($r);

		if($info['files'] === false)
			return(rmdir($this->_mohpath.XIVO_SEP_DIR.$name));

		$r = true;

		for($i = 0;$i < $info['nb_files'];$i++)
		{
			if($this->delete_file($name.XIVO_SEP_DIR.$info['files'][$i]) === false)
			{
				$r = false;
				break;
			}
		}

		if($r === true)
			$r = rmdir($this->_mohpath.XIVO_SEP_DIR.$name);

		return($r);
	}

	function get_all_by_category()
	{
		$r = false;

		if(($list = parent::get_name_val_by_category()) === false
		|| ($arr = xivo_get_aks($list)) === false)
			return($r);

		$r = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$list[$key];
			$val['category'] = $key;

			if($val['mode'] === 'custom' && $val['directory'] === '')
			{
				$r[] = $val;
				continue;
			}

			if($this->_chk_dir($key,true) === false)
				continue;

			$r[] = $val;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_list_files($dir='')
	{
		$dir = (string) $dir;

		if(($match_file = $this->_get_option_file_name()) === false)
			return($r);

		if($dir !== '')
		{
			if(($dirs = $this->get_list_dirs()) === false || in_array($dir,$dirs) === false)
				return(false);

			$dir = $this->_mohpath.XIVO_SEP_DIR.$dir;
		}
		else
			$dir = $this->_mohpath;

		$r = xivo_file::read_d($dir,'file','rw',$match_file);

		if($r !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_list_dirs()
	{
		if(($match_dir = $this->_get_option_dir_name('match_dir')) === false)
			return(false);

		$r = xivo_file::read_d($this->_mohpath,'dir','rwx',$match_dir);

		if($r !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
