<?php

class xivo_socket
{
	var $_param	= array(
				'address'		=> '',
				'port'			=> 0,
				'domain'		=> AF_INET,
				'type'			=> SOCK_STREAM,
				'protocol'		=> 0,
				'connect_nonblock'	=> false,
				'nonblock'		=> false);

	var $_error	= '';
	var $_errno	= 0;
	var $_link	= null;

	function xivo_socket($param=array())
	{
		$param = (array) $param;
		$open = true;

		if(isset($param['open']) === true)
			$open = false;

		if(isset($param['port']) === true)
			$this->_param['port'] = xivo_uint($param['port']);

		if(isset($param['domain']) === true)
			$this->_param['domain'] = $this->_chk_domain($param['domain']);

		if(isset($param['type']) === true)
			$this->_param['type'] = $this->_chk_type($param['type']);

		if(isset($param['protocol']) === true)
			$this->_param['protocol'] = $this->_chk_protocol($param['protocol']);

		if(isset($param['connect_nonblock']) === true)
			$this->_param['connect_nonblock'] = (bool) $param['connect_nonblock'];

		if(isset($param['nonblock']) === true)
			$this->_param['nonblock'] = (bool) $param['nonblock'];

		if(isset($param['address']) === true)
		{
			if($this->_param['domain'] === AF_UNIX)
			{
				if(($addr = xivo_file::is_f_rwx($param['address'])) !== false)
					$this->_param['address'] = $addr;
				else
					$this->open = false;
			}
			else
				$this->_param['address'] = gethostbyname($param['address']);
		}
		else
			$open = false;

		if($open === true)
			$this->open();
	}

	function set_param($name,$value)
	{
		$name = (string) $name;

		$this->_param[$name] = $value;	
	}

	function get_param($name)
	{
		$name = (string) $name;

		if(isset($this->_param[$name]) === true)
			return($this->_param[$name]);
		else
			return(null);
	}

	function _chk_option($func,$option)
	{
		if(is_string($option) === true)
			$option = strtolower($option);

		switch($option)
		{
			case 'debug':
			case SO_DEBUG:
				return(SO_DEBUG);
			case 'reuseraddr':
			case SO_REUSEADDR:
				return(SO_REUSEADDR);
			case 'keepalive':
			case SO_KEEPALIVE:
				return(SO_KEEPALIVE);
			case 'dontroute':
			case SO_DONTROUTE:
				return(SO_DONTROUTE);
			case 'linger':
			case SO_LINGER:
				return(SO_LINGER);
			case 'broadcast':
			case SO_BROADCAST:
				return(SO_BROADCAST);
			case 'oobinline':
			case SO_OOBINLINE:
				return(SO_OOBINLINE);
			case 'sndbuf':
			case SO_SNDBUF:
				return(SO_SNDBUF);
			case 'rcvbuf':
			case SO_RCVBUF:
				return(SO_RCVBUF);
			case 'sndlowat':
			case SO_SNDLOWAT:
				return(SO_SNDLOWAT);
			case 'rcvlowat':
			case SO_RCVLOWAT:
				return(SO_RCVLOWAT);
			case 'sndtimeo':
			case SO_SNDTIMEO:
				return(SO_SNDTIMEO);
			case 'rcvtimeo':
			case SO_RCVTIMEO:
				return(SO_RCVTIMEO);
			case 'type':
			case SO_TYPE:
				if($func !== 'get')
					return(false);
				else
					return(SO_TYPE);
			case 'error':
			case SO_ERROR:
				if($func !== 'get')
					return(false);
				else
					return(SO_ERROR);
		}
		return(false);
	}

	function _chk_domain($domain)
	{
		if(is_string($domain) === true)
			$domain = strtolower($domain);

		switch($domain)
		{
			case 'unix':
			case AF_UNIX:
				return(AF_UNIX);
			case 'inet':
			case AF_INET:
			default:
				return(AF_INET);
		}
	}

	function _chk_type($type)
	{
		if(is_string($type) === true)
			$type = strtolower($type);

		switch($type)
		{
			case 'dgram':
			case SOCK_DGRAM:
				return(SOCK_DGRAM);
			case 'raw':
			case SOCK_RAW:
				return(SOCK_RAW);
			case 'rdm':
			case SOCK_RDM:
				return(SOCK_RDM);
			case 'seqpacket':
			case SOCK_SEQPACKET:
				return(SOCK_SEQPACKET);
			case 'stream':
			case SOCK_STREAM:
			default:
				return(SOCK_STREAM);
		}
	}

	function _chk_protocol($protocol)
	{
		if(is_string($protocol) === true)
			$protocol = strtolower($protocol);

		switch($protocol)
		{
			case 'icmp':
			case 1:
				return(getprotobyname('icmp'));
			case 'udp':
			case SOL_UDP:
				return(SOL_UDP);
			case 'tcp':
			case SOL_TCP:
				return(SOL_TCP);
		}

		return(0);
	}

	function _chk_level($level)
	{
		$r = SOL_SOCKET;

		if(($lev = $this->_chk_protocol($level)) !== 0 || ($lev = getprotobyname($level)) !== false)
			$r = $lev;

		return($r);
	}

	function open()
	{
		if(is_resource($this->_link) === true)
			return($this->_link);

		if(($this->_link = $this->create()) === false)
		{
			$this->halt();
			return(false);
		}

		if($this->_param['connect_nonblock'] === true)
			$this->nonblock();

		if($this->connect() === false)
		{
			$this->halt('Can\'t connect to socket');
			return(false);
		}

		if($this->_param['nonblock'] === true)
			$this->nonblock();
	}

	function is_open()
	{
		return(is_resource($this->_link));
	}

	function halt($str='')
	{
		$str = (string) $str;

		if(($this->_errno = $this->errno()) === false)
			$this->_errno = 'unknown';

		if(($this->_error = $this->error()) === false)
			$this->_error = 'unknown';

		trigger_error($str.'Error: '.$this->_error.' - Errno: '.$this->_errno,E_USER_ERROR);
	}

	function connect()
	{
		$r = false;

		if(is_resource($this->_link) === false)
			return($r);

		if($this->_param['domain'] === AF_UNIX)
			$r = socket_connect($this->_link,$this->_param['address']);
		else if($this->_param['address'] !== '' && $this->_param['port'] !== 0)
			$r = socket_connect($this->_link,$this->_param['address'],$this->_param['port']);

		return($r);
	}

	function create()
	{
		$r = socket_create($this->_param['domain'],$this->_param['type'],$this->_param['protocol']);

		return($r);
	}

	function read_binary($length=4096)
	{
		$r = false;

		$length = xivo_uint($length);

		if($length !== 0 && is_resource($this->_link) === true)
		{
			$r = '';

			while(($read = socket_read($this->_link,$length,PHP_BINARY_READ)) !== '')
				$r .= $read;
		}

		return($r);
	}

	function read_normal($length=4096)
	{
		$r = false;

		$length = xivo_uint($length);

		if($length !== 0 && is_resource($this->_link) === true)
		{
			$r = '';

			while(($read = socket_read($this->_link,$length,PHP_BINARY_READ)) !== '')
				$r .= $read;
		}

		return($r);
	}

	function write($data,$length=0)
	{
		$r = false;

		if(($length = xivo_uint($length)) === 0)
			$length = strlen($data);

		if(is_resource($this->_link) === true)
			$r = socket_write($this->_link,$data,$length);

		return($r);
	}

     function recv($length = 1024) {
            $read = $buf = '';
	    
	while(socket_recv($this->_link, $buf, $length));

	    xivo_print_r($buf);
	    die();
        while(($flag=socket_recv($this->_link, $buf, $length,0))>0){
              $asc=ord(substr($buf, -1));
            if ($asc==0) {
                $read.=substr($buf,0,-1);
                break;
            }else{
                $read.=$buf;
            }
        }
           if ($flag<0){
            //error
            return false;
        }elseif ($flag==0){
            //Client disconnected
            return  false;
        }else{
              return $buf;
        }

     }

	function accept()
	{
		$r = false;

		if(is_resource($this->_link) === true)
			$r = socket_accept($this->_link);

		return($r);
	}

	function clear_error()
	{
		if(is_resource($this->_link) === true)
			socket_clear_error($this->_link);
	}

	function close()
	{
		if(is_resource($this->_link) === true)
			socket_close($this->_link);
	}

	function errno()
	{
		$r = false;

		if(is_resource($this->_link) === true)
			$r = socket_last_error($this->_link);

		return($r);
	}

	function error()
	{
		$r = false;

		if(is_resource($this->_link) === true && ($errno = $this->errno()) !== false)
			$r = socket_strerror($errno);

		return($r);
	}

	function nonblock()
	{
		$r = false;

		if(is_resource($this->_link) === true)
			$r = socket_set_nonblock($this->_link);

		return($r);
	}

	function block()
	{
		$r = false;

		if(is_resource($this->_link) === true)
			$r = socket_set_block($this->_link);

		return($r);
	}

	function set_option($level,$option,$value=array())
	{
		if(is_resource($this->_link) === true && ($option = $this->_chk_option($option)) !== false)
			$r = socket_set_option($this->_link,$this->_chk_level($level),$option,$value);
		else
			$r = false;

		return($r);
	}

	function get_option($level,$option)
	{
		if(is_resource($this->_link) === true && ($option = $this->_chk_option($option)) !== false)
			$r = socket_get_option($this->_link,$this->_chk_level($level),$option);
		else
			$r = false;

		return($r);
	}

	function shutdown($how=2)
	{
		$r = false;

		$how = xivo_uint($how,2);

		if(is_resource($this->_link) === true && $how >= 0 && $how <= 2)
			$r = socket_shutdown($this->_link,$how);

		return($r);
	}
}

?>
