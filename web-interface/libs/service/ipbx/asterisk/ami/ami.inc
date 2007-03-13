<?php

require_once(XIVO_PATH_LIBS.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'abstract.inc');

class xivo_service_asterisk_ami extends xivo_service_asterisk_abstract
{
	var $_name	= 'ami';

	function xivo_service_asterisk_ami(&$sre)
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
		if(xivo_issa($this->_name,$this->_sre->_ini) === false)
			return(false);



		if(($this->_cfgpath = xivo_file::is_d_rwx($this->_sre->_ini['general']['cfgpath'])) === false)
			return(false);
	}

	function _chk_file($name)
	{
		$r = false;

		$name = (string) $name;

		if(preg_match('/^[a-zA-Z0-9_]+\.conf$/',$name) === 1)
			$r = xivo_file::is_f_rw($this->_cfgpath.XIVO_SEP_DIR.$name);

		return($r);
	}

	function get($name)
	{
		$r = false;

		$name = (string) $name;

		if(($file = $this->_chk_file($name)) === false || ($content = file_get_contents($file)) === false)
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

		$name = (string) $name;

		$contents = str_replace("\r\n","\n",$contents);

		if(($file = $this->_chk_file($name)) !== false)
			$r = xivo_file::put_contents($file,$contents);

		return($r);
	}

	function get_list()
	{
		$r = xivo_file::read_d($this->_cfgpath,'file','rw','/^[a-zA-Z0-9_]+\.conf$/');

		if($r !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
