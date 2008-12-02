<?php

if(defined('UPLOAD_ERR_NO_TMP_DIR') === false)
	define('UPLOAD_ERR_NO_TMP_DIR',6);

if(defined('UPLOAD_ERR_CANT_WRITE') === false)
	define('UPLOAD_ERR_CANT_WRITE',7);

if(defined('FILE_USE_INCLUDE_PATH') === false)
	define('FILE_USE_INCLUDE_PATH',1);

if(defined('FILE_APPEND') === false)
	define('FILE_APPEND',8);

class xivo_file
{
	var $_finfo	= null;
	var $info	= array(
				'dir'		=> '',
				'ext'		=> '',
				'size'		=> 0,
				'name'		=> '',
				'path'		=> '',
				'mtime'		=> '',
				'pathnoext'	=> '',
				'mime'		=> false,
				'charset'	=> false,
				'upload'	=> false);

	function xivo_file()
	{
		$this->_finfo = $this->_load_fileinfo();
	}

	function _load_fileinfo()
	{
		if(blop::load_extension('fileinfo') === false
		|| xivo::load_class('xivo_file_fileinfo','file','fileinfo') === false)
			return(false);

		return(new xivo_file_fileinfo());
	}

	function get_info($file)
	{
		$file = (string) $file;

		if(($file = xivo_file::is_f($file)) === false)
			return(false);

		$pinfo = pathinfo($file);

		$info = array();
		$info['path'] = $file;
		$info['dir'] = $pinfo['dirname'];
		$info['name'] = $pinfo['basename'];
		$info['ext'] = $pinfo['extension'];

		if(($extlen = strlen($pinfo['extension'])) > 0)
			$info['filename'] = substr($pinfo['basename'],0,-($extlen+1));
		else
			$info['filename'] = $info['name'];

		$info['pathnoext'] = $pinfo['dirname'].XIVO_SEP_DIR.$info['filename'];
		$info['mime'] = xivo_file::get_mime($info['path']);
		$info['charset'] = xivo_file::get_charset($info['path']);
		$info['mtime'] = filemtime($info['path']);
		$info['size'] = filesize($info['path']);

		if(isset($this) === true && xivo_instanceof($this,__CLASS__) === true)
			$this->info = $info;

		return($info);
	}

	function download($file)
	{
		if($this->is_f_r($file) === false || $this->get_info($file) === false)
			return(false);

		header('Pragma: no-cache');
		header('Expires: 0');
		header('Last-Modified: '.date('D, d M Y H:i:s',$this->info['mtime']).' '.strftime('%Z'));
		header('Cache-Control: must-revalidate');
		header('Content-Type: '.$this->info['mime']);
		header('Content-Length: '.$this->info['size']);
		header('Content-Disposition: attachment; filename='.$this->info['name']);
		print file_get_contents($this->info['path']);
	}

	function get_finfo_info($file='')
	{
		$file = $file === '' && $this->info['path'] !== '' ? $this->info['path'] : $file;

		if(is_object($this->_finfo) === false || $this->_finfo->get_info($file) === false)
			return(false);

		return($this->_finfo->info);
	}

	function get_mime($file='')
	{
		if(isset($this) === true && xivo_instanceof($this,__CLASS__) === true)
			$static = false;
		else
			$static = true;

		if($file === '' && $static === false && $this->info['path'] !== '')
			$file = $this->info['path'];

		if($static === false && is_object($this->_finfo) === true)
			$r = $this->_finfo->get_mime($file);
		else if(($file = xivo_file::is_f($file)) !== false && blop::load_extension('mime_magic') !== false)
			$r = mime_content_type($file);
		else
			$r = false;

		return($r);
	}

	function get_charset($file='')
	{
		if(isset($this) === false
		|| xivo_instanceof($this,__CLASS__) === false
		|| is_object($this->_finfo) === false)
			return(false);

		if($file === '' && $this->info['path'] !== '')
			$file = $this->info['path'];

		return($this->_finfo->get_charset($file));
	}

	function get_time($file)
	{
		$r = false;

		if(($file = xivo_file::is_f($file)) !== false)
			$r = filemtime($file);

		return($r);
	}

	function get_upload($name,$chk_param=array())
	{
		$name = (string) $name;
		$chk_param = (array) $chk_param;

		if($this->chk_upload_err($name) === false
		|| isset($_FILES[$name]['tmp_name'],
			 $_FILES[$name]['name'],
			 $_FILES[$name]['type'],
			 $_FILES[$name]['size']) === false)
			return(false);

		if(is_uploaded_file($_FILES[$name]['tmp_name']) === false)
		{
			$this->rm($_FILES[$name]['tmp_name']);
			return(false);
		}

		$this->info['upload'] = $_FILES[$name];

		$this->info['mime'] = $_FILES[$name]['type'];
		$this->info['tmp_name'] = $_FILES[$name]['tmp_name'];
		$this->info['name'] = $this->info['upload']['name'];
		$this->info['size'] = $this->info['upload']['size'];

		if(($this->info['mime'] = $this->get_mime($this->info['tmp_name'])) === false)
			$this->info['mime'] = $this->info['upload']['mime'];

		$r = false;

		if(empty($chk_param) === false && ($r = $this->_chk_upload($chk_param)) === false)
			$this->rm($this->info['tmp_name']);
		else
			$r = true;

		return($r);
	}

	function _chk_upload($param)
	{
		if(is_array($param) === false)
			return(false);
		else if(isset($param['size']) === true)
		{
			$param['size'] = xivo_uint($param['size']);

			if($this->info['size'] > $param['size'])
				return(false);
		}

		if(isset($param['mime']) === true)
		{
			$param['mime'] = (array) $param['mime'];

			if(isset($param['mime'][0]) === false
			|| in_array($this->info['mime'],$param['mime']) === false)
				return(false);
		}

		if(isset($param['name']) === true)
		{
			$param['name'] = (string) $param['name'];

			if(preg_match($param['name'],$this->info['name']) !== 1)
				return(false);
		}

		if(isset($param['extension']) === true)
		{
			$param['extension'] = (array) $param['extension'];

			if(($pos = strrpos($this->info['name'],'.')) === false)
				return(false);

			$ext = strtolower(substr($this->info['name'],$pos + 1));

			if(isset($param['extension'][0]) === true)
			{
				if(in_array($ext,$param['extension']) === false)
					return(false);
			}

			if(xivo_issa($ext,$param['extension']) === true
			&& in_array($this->info['mime'],$param['extension'][$ext]) === false)
				return(false);
		}

		return(true);
	}

	function chk_upload_err($name)
	{
		$name = (string) $name;

		if(xivo_issa($name,$_FILES) === false || isset($_FILES[$name]['error']) === false)
			return(false);
		else if(($upload = xivo_get_memory('upload_max_filesize')) === false)
			trigger_error('Invalid upload_max_filesize php.ini directive',E_USER_ERROR);

		if(($post = xivo_get_memory('post_max_size')) === false)
			trigger_error('Invalid post_max_size php.ini directive',E_USER_ERROR);

		if(($mlimit = xivo_get_memory('memory_limit')) === false)
			trigger_error('Invalid memory_limit php.ini directive',E_USER_ERROR);

		if($post < $upload)
			trigger_error('post_max_size php.ini directive shoud be less than upload_max_filesize php.ini directive',E_USER_ERROR);

		if($mlimit < $post)
			trigger_error('memory_limit php.ini directive shoud be less than post_max_size php.ini directive',E_USER_ERROR);

		$r = false;

		switch($_FILES[$name]['error'])
		{
			case UPLOAD_ERR_OK:
				$r = true;
				break;
			case UPLOAD_ERR_INI_SIZE:
				//trigger_error('Uploaded file exceeds the upload_max_filesize directive',E_USER_WARNING);
				break;
			case UPLOAD_ERR_FORM_SIZE:
				//trigger_error('Uploaded file exceeds the MAX_FILE_SIZE directive',E_USER_WARNING);
				break;
			case UPLOAD_ERR_PARTIAL:
				//trigger_error('Uploaded file was only partially uploaded',E_USER_WARNING);
				break;
			case UPLOAD_ERR_NO_FILE:
				//trigger_error('No file was uploaded',E_USER_WARNING);
				break;
			case UPLOAD_ERR_NO_TMP_DIR:
				trigger_error('Missing a temporary folder',E_USER_WARNING);
				break;
			case UPLOAD_ERR_CANT_WRITE:
				trigger_error('Failed to write file to disk',E_USER_WARNING);
				break;
			default:
				trigger_error('Unknown File Error',E_USER_WARNING);
		}

		return($r);
	}

	function load_file($file,$dir='',$once=false,$verif=true)
	{
		$path = '';

		if(preg_match('/^[a-z0-9\-_.]+$/i',$file) !== 1)
			return(false);
		else if(is_array($dir) === true)
		{
			if(isset($dir[0]) === false)
				return(false);

			$dir = implode($dir,XIVO_SEP_DIR);
		}

		$dir = (string) $dir;

		if($dir === '')
		{
			$path = $file;
			$verif = false;
		}
		else if(is_readable($dir) === true && is_dir($dir) === true)
			$path = rtrim($dir,XIVO_SEP_DIR).XIVO_SEP_DIR.$file;
		else
			return(false);

		if((bool) $verif === true
		&& (is_file($path) === false || is_readable($path) === false) === true)
			return(false);
		else if($once === false)
			return((bool) require($path));

		return((bool) require_once($path));
	}

	function is_includable($file,$verif=true)
	{
		if((bool) $verif === true
		&& (is_file($file) === false || is_readable($file) === false) === true)
			return(false);
		else if(($r = is_resource(($f = fopen($file,'r',true)))) === true)
			fclose($f);

		return(true);
	}

	function required($arr,$once=false,$includable=false)
	{
		$arr = (array) $arr;

		if(isset($arr[0]) === false)
			return(false);

		$path = implode($arr,XIVO_SEP_DIR);

		if(($path = xivo_file::is_f_r($path)) === false
		|| ((bool) $includable === true
		   && xivo_file::is_includable($path) === false) === true)
			return(false);
		else if((bool) $once === false)
			return((bool) require($path));
		else
			return((bool) require_once($path));
	}

	function is_r($file)
	{
		return(is_readable($file));
	}

	function is_w($file)
	{
		return(is_writeable($file));
	}

	function is_x($file)
	{
		return(is_executable($file));
	}

	function is_rw($file)
	{
		return((is_readable($file) === true && is_writeable($file) === true));
	}

	function is_rx($file)
	{
		return((is_readable($file) === true && is_executable($file) === true));
	}

	function is_wx($file)
	{
		return((is_writeable($file) === true && is_executable($file) === true));
	}

	function is_rwx($file)
	{
		return((is_readable($file) === true && is_writeable($file) === true && is_executable($file) === true));
	}

	function joinpath()
	{
		$arr = func_get_args();

		return(xivo_implode(XIVO_SEP_DIR,$arr));
	}

	function abspath($str)
	{
		$str = (string) $str;

		if(isset($str{0}) === false)
			return(false);
		else if(strpos($str,XIVO_SEP_DIR) !== 0)
			$str = getcwd().XIVO_SEP_DIR.$str;

		$lstr = preg_split('@'.preg_quote(XIVO_SEP_DIR).'+@',$str,-1,PREG_SPLIT_NO_EMPTY);

		if(($nb = count($lstr)) === 0)
			return($str);

		$r = array();

		$nb = count($lstr);

		for($i = 0;$i < $nb;$i++)
		{
			if(($c = $lstr[$i]) === '.')
				continue;
			else if($c !== '..')
				$r[] = $c;
			else if(isset($r[0]) === true)
				array_pop($r);
		}

		return(XIVO_SEP_DIR.implode(XIVO_SEP_DIR,$r));
	}

	function is_f($file,$mode=1)
	{
		static $pathcache = array();

		if(is_file($file) === false)
			return(false);
		else if(($mode = intval($mode)) === 0)
			return($file);
		else if(isset($pathcache[$file]) === false)
		{
			if($mode !== 2)
				$pathcache[$file] = xivo_file::abspath($file);
			else
				$pathcache[$file] = realpath($file);
		}

		return($pathcache[$file]);
	}

	function is_f_r($file,$mode=1)
	{
		if(is_readable($file) === true)
			return(xivo_file::is_f($file,$mode));

		return(false);
	}

	function is_f_w($file,$mode=1)
	{
		if(is_writeable($file) === true)
			return(xivo_file::is_f($file,$mode));

		return(false);
	}

	function is_f_x($file,$mode=1)
	{
		if(is_executable($file) === true)
			return(xivo_file::is_f($file,$mode));

		return(false);
	}

	function is_f_rw($file,$mode=1)
	{
		if(xivo_file::is_rw($file) === true)
			return(xivo_file::is_f($file,$mode));

		return(false);
	}

	function is_f_rx($file,$mode=1)
	{
		if(xivo_file::is_rx($file) === true)
			return(xivo_file::is_f($file,$mode));

		return(false);
	}

	function is_f_wx($file,$mode=1)
	{
		if(xivo_file::is_wx($file) === true)
			return(xivo_file::is_f($file,$mode));

		return(false);
	}

	function is_f_rwx($file,$mode=1)
	{
		if(xivo_file::is_rwx($file) === true)
			return(xivo_file::is_f($file,$mode));

		return(false);
	}

	function is_d($dir,$dirname=false,$mode=1)
	{
		static $pathcache = array();

		if((bool) $dirname === true)
			$dir = dirname($dir);

		if(is_dir($dir) === false)
			return(false);
		else if(($mode = intval($mode)) === 0)
				return($dir);
		else if(isset($pathcache[$dir]) === false)
		{
			if($mode !== 2)
				$pathcache[$dir] = xivo_file::abspath($dir);
			else
				$pathcache[$dir] = realpath($dir);
		}

		return($pathcache[$dir]);
	}

	function is_d_r($dir,$dirname=false,$mode=1)
	{
		if((bool) $dirname === true)
			$dir = dirname($dir);

		if(is_readable($dir) === true)
			return(xivo_file::is_d($dir,false,$mode));

		return(false);
	}

	function is_d_w($dir,$dirname=false,$mode=1)
	{
		if((bool) $dirname === true)
			$dir = dirname($dir);

		if(is_writeable($dir) === true)
			return(xivo_file::is_d($dir,false,$mode));

		return(false);
	}

	function is_d_x($dir,$dirname=false,$mode=1)
	{
		if((bool) $dirname === true)
			$dir = dirname($dir);

		if(is_executable($dir) === true)
			return(xivo_file::is_d($dir,false,$mode));

		return(false);
	}

	function is_d_rw($dir,$dirname=false,$mode=1)
	{
		if((bool) $dirname === true)
			$dir = dirname($dir);

		if(xivo_file::is_rw($dir) === true)
			return(xivo_file::is_d($dir,false,$mode));

		return(false);
	}

	function is_d_rx($dir,$dirname=false,$mode=1)
	{
		if((bool) $dirname === true)
			$dir = dirname($dir);

		if(xivo_file::is_rx($dir) === true)
			return(xivo_file::is_d($dir,false,$mode));

		return(false);
	}

	function is_d_wx($dir,$dirname=false,$mode=1)
	{
		if((bool) $dirname === true)
			$dir = dirname($dir);

		if(xivo_file::is_wx($dir) === true)
			return(xivo_file::is_d($dir,false,$mode));

		return(false);
	}

	function is_d_rwx($dir,$dirname=false,$mode=1)
	{
		if((bool) $dirname === true)
			$dir = dirname($dir);

		if(xivo_file::is_rwx($dir) === true)
			return(xivo_file::is_d($dir,false,$mode));

		return(false);
	}

	function put_contents($filename,$data,$flags=0)
	{
		$filename = (string) $filename;
		$flags = xivo_uint($flags);

		if(is_array($data) === true || is_object($data) === true)
			$data = xivo_implode('',$data);

		if(is_scalar($data) === false)
			return(false);

		$dir = dirname($filename);

		if(xivo_file::is_d_rwx($dir) === false)
			return(false);
		else if(($flags & FILE_APPEND) !== 0)
			$mode = 'a';
		else if(($filename = xivo_file::is_f_rw($filename)) !== false)
			$mode = 'wb';
		else
			return(false);

		$use_inc_path = ($flags & FILE_USE_INCLUDE_PATH) !== 0;
		$use_lock = ($flags & LOCK_EX) !== 0;

		if(($fp = fopen($filename,$mode,$use_inc_path)) === false
		|| ($use_lock === true && flock($fp,LOCK_EX) === false) === true)
			return(false);

		$r = false;
		$bytes = 0;

		if(($bytes = fwrite($fp,$data)) !== false)
			$r = $bytes;

		if($use_lock === true)
			flock($fp,LOCK_UN);

		fclose($fp);

		return($r);
	}

	function read_d($dir,$type='',$mode='',$match='',$dirwmode=false)
	{
		$dir = (string) $dir;
		$type = (string) $type;
		$mode = (string) $mode;
		$match = (string) $match;

		if(($dir = xivo_file::is_d_rx($dir)) === false
		|| ($open = opendir($dir)) === false
		|| ((bool) $dirwmode === true && xivo_file::is_w($dir) === false) === true)
			return(false);

		if($type !== 'file' && $type !== 'dir')
			$type = '';

		$nmode = '';

		if(strpos($mode,'r') !== false)
			$nmode .= 'r';

		if(strpos($mode,'w') !== false)
			$nmode .= 'w';

		if(strpos($mode,'x') !== false)
			$nmode .= 'x';

		$mode = $nmode;

		$r = array();

		while(($name = readdir($open)) !== false)
		{
			if($name === '.' || $name === '..')
				continue;
			else if($match !== '' && preg_match($match,$name) !== 1)
				continue;

			$file = $dir.XIVO_SEP_DIR.$name;

			switch($mode)
			{
				case 'r':
					if(xivo_file::is_r($file) === false)
						continue 2;
					break;
				case 'w':
					if(xivo_file::is_w($file) === false)
						continue 2;
					break;
				case 'x':
					if(xivo_file::is_x($file) === false)
						continue 2;
					break;
				case 'rw':
					if(xivo_file::is_rw($file) === false)
						continue 2;
					break;
				case 'rx':
					if(xivo_file::is_rx($file) === false)
						continue 2;
					break;
				case 'wx':
					if(xivo_file::is_wx($file) === false)
						continue 2;
					break;
				case 'rwx':
					if(xivo_file::is_rwx($file) === false)
						continue 2;
					break;
			}

			switch($type)
			{
				case 'file':
					if(xivo_file::is_f($file) === false)
						continue 2;
					break;
				case 'dir':
					if(xivo_file::is_d($file) === false)
						continue 2;
					break;
			}

			$r[] = $name;
		}

		closedir($open);

		return($r);
	}

	function rm($file)
	{
		if(xivo_file::is_d_w($file,true) !== false && unlink($file) !== false)
			return(true);

		return(false);
	}

	function rmdir($dir)
	{
		if(($dir = xivo_file::is_d_rwx($dir)) === false
		|| xivo_file::is_d_rw(dirname($dir)) === false
		|| ($list = xivo_file::read_d($dir)) === false
		|| is_link($dir) === true)
			return(false);
		else if(isset($list[0]) === false)
			return(rmdir($dir));

		$nb = count($list);

		for($i = 0;$i < $nb;$i++)
		{
			$path = $dir.XIVO_SEP_DIR.$list[$i];

			if(xivo_file::is_f($path) !== false)
			{
				unlink($path);
				continue;
			}

			if(xivo_file::is_d($path) !== false)
				xivo_file::rmdir($path);
		}

		return(rmdir($dir));
	}
}

?>
