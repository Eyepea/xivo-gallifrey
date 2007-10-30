<?php

if(defined('UPLOAD_ERR_NO_TMP_DIR') === false)
	define('UPLOAD_ERR_NO_TMP_DIR',6);

if(defined('UPLOAD_ERR_CANT_WRITE') === false)
	define('UPLOAD_ERR_CANT_WRITE',7);

if(defined('FILE_USE_INCLUDE_PATH') === false)
	define('FILE_USE_INCLUDE_PATH',1);

if(defined('LOCK_EX') === false)
	define('LOCK_EX',2);

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
				'mime'		=> false,
				'charset'	=> false,
				'upload'	=> false,
				'open'		=> 0);

	function xivo_file()
	{
		if(defined('XIVO_EXT_INI_LD') === false)
			return(null);

		$this->_finfo = $this->_load_fileinfo();
	}

	function _load_fileinfo()
	{
		if((XIVO_EXT_INI_LD & XIVO_EXT__FILEINFO) === 0
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
		$info['mime'] = xivo_file::get_mime($info['path']);
		$info['charset'] = xivo_file::get_charset($info['path']);
		$info['mtime'] = filemtime($info['path']);
		$info['size'] = filesize($info['path']);

		if(isset($this) === true && get_class($this) === __CLASS__)
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
		if(isset($this) === true && get_class($this) === __CLASS__)
			$static = false;
		else
			$static = true;

		if($file === '' && $static === false && $this->info['path'] !== '')
			$file = $this->info['path'];

		if($static === false && is_object($this->_finfo) === true)
			$r = $this->_finfo->get_mime($file);
		else if(($file = xivo_file::is_f($file)) !== false && (XIVO_EXT_INI_LD & XIVO_EXT__MIME_MAGIC) !== 0)
			$r = mime_content_type($file);
		else
			$r = false;

		return($r);
	}

	function get_charset($file='')
	{
		if(isset($this) === false
		|| get_class($this) !== __CLASS__
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
		|| isset($_FILES[$name]['tmp_name'],$_FILES[$name]['name'],
			$_FILES[$name]['type'],$_FILES[$name]['size']) === false)
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

		if(isset($param['size']) === true)
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

			if(is_array($param['extension'][$ext]) === true)
			{
				$regexp = false;

				if(isset($param['extension'][$ext]['regexp']) === true)
				{
					$regexp = $param['extension'][$ext]['regexp'];
					unset($param['extension'][$ext]['regexp']);
				}

				if(in_array($this->info['mime'],$param['extension'][$ext]) === false && $regexp === false)
					return(false);

				if($regexp !== false && preg_match($regexp,$this->info['mime']) === 0)
					return(false);
			}
		}

		return(true);
	}

	function chk_upload_err($name)
	{
		$name = (string) $name;

		if(xivo_issa($name,$_FILES) === false || isset($_FILES[$name]['error']) === false)
			return(false);

		if(($upload = xivo_get_memory('upload_max_filesize')) === false)
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
		$file = (string) $file;
		$once = (bool) $once;
		$verif = (bool) $verif;
		$path = '';
		
		if(preg_match('/^[a-z0-9\-_.]+$/i',$file) !== 1)
			return(false);

		if(is_array($dir) === true)
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
		else if(($dir = xivo_file::is_d_r($dir)) !== false)
			$path = $dir.XIVO_SEP_DIR.$file;
		else
			return(false);

		if(xivo_file::is_i($path,$verif) === false)
			return(false);

		if($once === true)
			return((bool) require_once($path));
		else
			return((bool) require($path));
	}

	function is_i($file,$verif=true)
	{
		$verif = (bool) $verif;

		if($verif === true && ($file = xivo_file::is_f_r($file)) === false)
			return(false);

		if(($r = is_resource(($f = fopen($file,'r',true)))) === true)
			fclose($f);

		return($r);
	}

	function required($arr,$once=false,$includable=false)
	{
		$arr = (array) $arr;
		$once = (bool) $once;
		$includable = (bool) $includable;

		if(isset($arr[0]) === false)
			return(false);

		$path = implode($arr,XIVO_SEP_DIR);

		if(($includable === true && xivo_file::is_i($path) === false) === true
		|| ($path = xivo_file::is_f_r($path)) === false)
			return(false);

		if($once === true)
			return((bool) require_once($path));
		else
			return((bool) require($path));
	}

	function is_r($file)
	{
		$file = (string) $file;

		return(is_readable($file));
	}

	function is_w($file)
	{
		$file = (string) $file;

		return(is_writeable($file));
	}

	function is_x($file)
	{
		$file = (string) $file;

		return(is_executable($file));
	}

	function is_rw($file)
	{
		$file = (string) $file;

		return((is_readable($file) === true && is_writeable($file) === true ? true : false));
	}

	function is_rx($file)
	{
		$file = (string) $file;

		return((is_readable($file) === true && is_executable($file) === true ? true : false));
	}

	function is_wx($file)
	{
		$file = (string) $file;

		return((is_writeable($file) === true && is_executable($file) === true ? true : false));
	}

	function is_rwx($file)
	{
		$file = (string) $file;

		return((is_readable($file) === true && is_writeable($file) === true && is_executable($file) === true ? true : false));
	}

	function is_f($file)
	{
		$r = false;

		$file = (string) $file;

		if(is_file($file) === true && ($file = realpath($file)) !== false)
			$r = $file;

		return($r);
	}

	function is_f_r($file)
	{
		$r = false;

		if(is_readable($file) === true && ($file = xivo_file::is_f($file)) !== false)
			$r = $file;

		return($r);	
	}

	function is_f_w($file)
	{
		$r = false;

		if(is_writeable($file) === true && ($file = xivo_file::is_f($file)) !== false)
			$r = $file;

		return($r);	
	}

	function is_f_x($file)
	{
		$r = false;

		if(is_executable($file) === true && ($file = xivo_file::is_f($file)) !== false)
			$r = $file;

		return($r);	
	}

	function is_f_rw($file)
	{
		$r = false;

		if(xivo_file::is_rw($file) === true && ($file = xivo_file::is_f($file)) !== false)
			$r = $file;

		return($r);	
	}

	function is_f_rx($file)
	{
		$r = false;

		if(xivo_file::is_rx($file) === true && ($file = xivo_file::is_f($file)) !== false)
			$r = $file;

		return($r);	
	}

	function is_f_wx($file)
	{
		$r = false;

		if(xivo_file::is_wx($file) === true && ($file = xivo_file::is_f($file)) !== false)
			$r = $file;

		return($r);	
	}

	function is_f_rwx($file)
	{
		$r = false;

		if(xivo_file::is_rwx($file) === true && ($file = xivo_file::is_f($file)) !== false)
			$r = $file;

		return($r);	
	}

	function is_d($dir,$t=false)
	{
		$r = false;

		$dir = (string) $dir;

		if((bool) $t === true)
			$dir = dirname($dir);

		if(is_dir($dir) === true && ($dir = realpath($dir)) !== false)
			$r = $dir;

		return($r);
	}

	function is_d_r($dir,$t=false)
	{
		$r = false;

		$dir = (string) $dir;

		if((bool) $t === true)
			$dir = dirname($dir);

		if(is_readable($dir) === true && ($dir = xivo_file::is_d($dir)) !== false)
			$r = $dir;

		return($r);
	}

	function is_d_w($dir,$t=false)
	{
		$r = false;

		$dir = (string) $dir;

		if((bool) $t === true)
			$dir = dirname($dir);

		if(is_writeable($dir) === true && ($dir = xivo_file::is_d($dir)) !== false)
			$r = $dir;

		return($r);
	}

	function is_d_x($dir,$t=false)
	{
		$r = false;
 
		$dir = (string) $dir;

		if((bool) $t === true)
			$dir = dirname($dir);

		if(is_executable($dir) === true && ($dir = xivo_file::is_d($dir)) !== false)
			$r = $dir;

		return($r);
	}

	function is_d_rw($dir,$t=false)
	{
		$r = false;

		if((bool) $t === true)
			$dir = dirname($dir);

		if(xivo_file::is_rw($dir) === true && ($dir = xivo_file::is_d($dir)) !== false)
			$r = $dir;

		return($r);
	}

	function is_d_rx($dir,$t=false)
	{
		$r = false;

		if((bool) $t === true)
			$dir = dirname($dir);

		if(xivo_file::is_rx($dir) === true && ($dir = xivo_file::is_d($dir)) !== false)
			$r = $dir;

		return($r);
	}

	function is_d_wx($dir,$t=false)
	{
		$r = false;

		if((bool) $t === true)
			$dir = dirname($dir);

		if(xivo_file::is_wx($dir) === true && ($dir = xivo_file::is_d($dir)) !== false)
			$r = $dir;

		return($r);
	}

	function is_d_rwx($dir,$t=false)
	{
		$r = false;

		if((bool) $t === true)
			$dir = dirname($dir);

		if(xivo_file::is_rwx($dir) === true && ($dir = xivo_file::is_d($dir)) !== false)
			$r = $dir;

		return($r);
	}

	function put_contents($filename,$data,$flags=0)
	{
		$filename = (string) $filename;
		$flags = xivo_uint($flags);

		if(is_array($data) === true || is_object($data) === true)
			$data = implode('',(array) $data);

		if(is_scalar($data) === false)
			return(false);

		$dir = dirname($filename);
		
		if(xivo_file::is_d_rwx($dir) === false)
			return(false);

		if(($flags & FILE_APPEND) !== 0)
			$mode = 'a';
		else if(($filename = xivo_file::is_f_rw($filename)) !== false)
			$mode = 'wb';
		else
			return(false);

		$use_inc_path = ($flags & FILE_USE_INCLUDE_PATH) !== 0 ? true : false;

		if(($fp = fopen($filename,$mode,$use_inc_path)) === false)
			return(false);

		if(($flags & LOCK_EX) !== 0 && flock($fp,LOCK_EX) === false)
			return(false);

		$use_lock = ($flags & LOCK_EX) !== 0 ? true : false;

		$r = false;
		$bytes = 0;

		if(($bytes = fwrite($fp,$data)) !== false)
			$r = $bytes;

		fclose($fp);

		return($r);		
	}

	function read_d($dir,$type='',$mode='',$match='',$dirwmode=false)
	{
		$dir = (string) $dir;
		$type = (string) $type;
		$mode = (string) $mode;
		$match = (string) $match;
		$dirwmode = (bool) $dirwmode;

		if(($dir = xivo_file::is_d_rx($dir)) === false
		|| ($open = opendir($dir)) === false
		|| ($dirwmode === true && xivo_file::is_w($dir) === false) === true)
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

			if($match !== '' && preg_match($match,$name) !== 1)
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
		$r = false;

		if(xivo_file::is_d_w($file,true) !== false && unlink($file) !== false)
			$r = true;

		return($r);
	}

	function rmdir($dir)
	{
		if(($dir = xivo_file::is_d_rwx($dir)) === false
		|| xivo_file::is_d_rw(dirname($dir)) === false
		|| ($list = xivo_file::read_d($dir)) === false)
			return(false);

		if(isset($list[0]) === false)
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
