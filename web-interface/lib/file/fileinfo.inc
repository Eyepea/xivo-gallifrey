<?php

class xivo_file_fileinfo
{
	var $_finfo	= null;
	var $info	= array(
				'path'		=> '',
				'mime'		=> false,
				'charset'	=> false);

	function open($define=FILEINFO_MIME)
	{
		$this->close();
		$this->_finfo = finfo_open($define);
	}

	function get_info($file)
	{
		if(($file = xivo_file::is_f($file)) === false)
			return(false);
		else if(is_resource($this->_finfo) === false)
			$this->open(FILEINFO_MIME);

		$info = finfo_file($this->_finfo,$file);

		$this->info['path'] = $file;

		$arr = explode(';',$info);

		if(isset($arr[1]) === true)
			$this->info['charset'] = trim($arr[1]);

		$this->info['mime'] = trim($arr[0]);

		return($this->info);
	}

	function get_charset($file)
	{
		if($this->get_info($file) === false)
			return(false);

		return($this->info['charset']);
	}

	function get_mime($file)
	{
		if($this->get_info($file) === false)
			return(false);

		return($this->info['mime']);
	}

	function close()
	{
		if(is_resource($this->_finfo) === true)
			finfo_close($this->_finfo);
	}
}

?>
