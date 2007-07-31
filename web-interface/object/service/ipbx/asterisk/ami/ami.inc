<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','abstract.inc'),true);

class xivo_service_asterisk_ami extends xivo_service_asterisk_abstract
{
	var $_name	= 'ami';
	var $_socket	= null;

	function xivo_service_asterisk_ami(&$sre)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;

		$this->_load_config();

		if($this->_chk_socket() === false)
			trigger_error('Invalid or missing configuration in '.$this->_sre->get_type().' ini file',E_USER_ERROR);
	}

	function _chk_socket()
	{
		if(xivo_issa($this->_name,$this->_sre->_ini) === false
		|| isset($this->_sre->_ini[$this->_name]['bindaddr'],$this->_sre->_ini[$this->_name]['port']) === false)
			return(false);

		$this->_conf['socket']['address'] = $this->_sre->_ini[$this->_name]['bindaddr'];
		$this->_conf['socket']['port'] = $this->_sre->_ini[$this->_name]['port'];
	}

	function _load_socket()
	{
		if($this->_socket !== null)
			return($this->_socket->open());

		xivo::load_class('xivo_socket');
		$this->_socket = new xivo_socket($this->_conf['socket']);

		if(xivo_issa('timeout',$this->_conf['sockect']) === true)
		{
			$this->_socket->set_option(null,
						   'sndtimeo',
						   $this->_conf['socket']['timeout']);
		}

		return(true);
	}

	function cmd($cmd)
	{
		$r = false;

		$cmd = trim((string) $cmd);

		if(xivo_cast_print_strict($cmd) === false)
			return(false);

		$cmd = $match[0];
		
		$this->_load_socket();

		if($this->_socket->is_open() === false)
			return(false);

		$r = $this->_socket->write($cmd);

		$this->_socket->close();

		return($r);
	}
}

?>
