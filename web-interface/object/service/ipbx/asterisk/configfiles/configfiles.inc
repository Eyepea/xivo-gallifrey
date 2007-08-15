<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_configfiles extends xivo_service_asterisk_abstract
{
	var $_name	= 'configfiles';
	var $_filter	= null;
	var $_cfgpath	= '';

	function xivo_service_asterisk_configfiles(&$sre)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;

		if($this->_chk_cfgpath() === false)
			trigger_error('Invalid or missing configuration path in '.$this->_sre->get_type().' ini file',E_USER_ERROR);

		$this->_load_config();
	}

	function _chk_cfgpath()
	{
		if(xivo_issa($this->_name,$this->_sre->_ini) === false
		|| isset($this->_sre->_ini[$this->_name]['path']) === false
		|| ($this->_cfgpath = xivo_file::is_d_rwx($this->_sre->_ini[$this->_name]['path'])) === false)
			return(false);
	}

	function _chk_file($name)
	{
		$r = false;

		$name = strval($name);

		if(preg_match('/^[a-zA-Z0-9_-]+\.conf$/',$name) === 1)
			$r = xivo_file::is_f_rw($this->_cfgpath.XIVO_SEP_DIR.$name);

		return($r);
	}

	function get($name)
	{
		$r = false;

		if(($file = $this->_chk_file($name)) === false
		|| ($content = file_get_contents($file)) === false)
			return($r);

		$r = array();
		$r['name'] = $name;
		$r['file'] = $file;
		$r['content'] = &$content;

		return($r);
	}

	function edit($name,$contents)
	{
		$r = false;

		$contents = xivo_eol($contents);

		if(($file = $this->_chk_file($name)) !== false)
			$r = xivo_file::put_contents($file,$contents);

		return($r);
	}

	function get_list()
	{
		$r = xivo_file::read_d($this->_cfgpath,'file','rw','/^[a-zA-Z0-9_-]+\.conf$/');

		if($r !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
