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

		if(xivo_issa('sndtimeo',$this->_conf['socket']) === true)
		{
			$this->_socket->set_option(null,
						   'sndtimeo',
						   $this->_conf['socket']['sndtimeo']);
		}
		
		if(xivo_issa('rcvtimeo',$this->_conf['socket']) === true)
		{
			$this->_socket->set_option(null,
						   'rcvtimeo',
						   $this->_conf['socket']['rcvtimeo']);
		}

		return(true);
	}

	function cmd($cmd,$read=false)
	{
		$r = false;

		$cmd = trim((string) $cmd);
		$read = (bool) $read;

		if(xivo_cast_print_strict($cmd) === false)
			return(false);

		$this->_load_socket();

		if($this->_socket->is_open() === false)
			return(false);

		if(($r = $this->_socket->write($cmd)) !== false && $read === true)
			$r = $this->_parse_cti_recv($this->_socket->read_binary());

		$this->_socket->close();

		return($r);
	}

	function _parse_cti_recv($recv)
	{
		$recv = (string) $recv;
		$recv = str_replace("\r",'',$recv);
		$recv = explode("\n",$recv);

		if(($nb = count($recv)) === 0)
			return(false);
		
		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$msg = explode(':',$recv[$i],2);

			if(isset($msg[1]) === false)
				$msg[1] = '';

			if($msg[0] === 'XIVO-CLI-PHP')
			{
				if(substr($msg[1],0,2) === 'KO')
					return(false);
				continue;
			}
			else if($msg[0] === 'Privilege')
				continue;

			$r[] = $recv[$i];
		}

		return($r);
	}
}

?>
