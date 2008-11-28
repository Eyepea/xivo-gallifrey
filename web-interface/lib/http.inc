<?php

class xivo_http
{
	var $_param	= array('version'	=> '1.0');
	var $_headers	= array();

	function xivo_http($param=array())
	{
		$param = (array) $param;

		if(isset($param['version']) === true
		&& ($version = $this->_chk_version($param['version'])) === true)
			$this->_param['version'] = $version;
	}

	function _chk_version($version)
	{
		$version = strval($version);

		if($version === '1.0' || $version === '1.1')
			return($version);

		return(false);
	}

	function set_status($code)
	{
		switch(intval($code))
		{
			case 100:
				$this->_mk_status_reason(100,'Continue');
			break;
			case 101:
				$this->_mk_status_reason(101,'Switching Protocols');
			break;
			case 200:
				$this->_mk_status_reason(200,'OK');
			break;
			case 201:
				$this->_mk_status_reason(201,'Created');
			break;
			case 202:
				$this->_mk_status_reason(202,'Accepted');
			break;
			case 203:
				$this->_mk_status_reason(203,'Non-Authoritative Information');
			break;
			case 204:
				$this->_mk_status_reason(204,'No Content');
			break;
			case 205:
				$this->_mk_status_reason(205,'Reset Content');
			break;
			case 206:
				$this->_mk_status_reason(206,'Partial Content');
			break;
			case 300:
				$this->_mk_status_reason(300,'Multiple Choices');
			break;
			case 301:
				$this->_mk_status_reason(301,'Moved Permanently');
			break;
			case 302:
				$this->_mk_status_reason(302,'Found');
			break;
			case 303:
				$this->_mk_status_reason(303,'See Other');
			break;
			case 304:
				$this->_mk_status_reason(304,'Not Modified');
			break;
			case 305:
				$this->_mk_status_reason(305,'Use Proxy');
			break;
			case 307:
				$this->_mk_status_reason(307,'Temporary Redirect');
			break;
			case 400:
				$this->_mk_status_reason(400,'Bad Request');
			break;
			case 401:
				$this->_mk_status_reason(401,'Unauthorized');
			break;
			case 402:
				$this->_mk_status_reason(402,'Payment Required');
			break;
			case 403:
				$this->_mk_status_reason(403,'Forbidden');
			break;
			case 404:
				$this->_mk_status_reason(404,'Not Found');
			break;
			case 405:
				$this->_mk_status_reason(405,'Method Not Allowed');
			break;
			case 406:
				$this->_mk_status_reason(406,'Not Acceptable');
			break;
			case 407:
				$this->_mk_status_reason(407,'Proxy Authentication Required');
			break;
			case 408:
				$this->_mk_status_reason(408,'Request Time-out');
			break;
			case 409:
				$this->_mk_status_reason(409,'Conflict');
			break;
			case 410:
				$this->_mk_status_reason(410,'Gone');
			break;
			case 411:
				$this->_mk_status_reason(411,'Length Required');
			break;
			case 412:
				$this->_mk_status_reason(412,'Precondition Failed');
			break;
			case 413:
				$this->_mk_status_reason(413,'Request Entity Too Large');
			break;
			case 414:
				$this->_mk_status_reason(414,'Request-URI Too Large');
			break;
			case 415:
				$this->_mk_status_reason(415,'Unsupported Media Type');
			break;
			case 416:
				$this->_mk_status_reason(416,'Requested range not satisfiable');
			break;
			case 417:
				$this->_mk_status_reason(417,'Expectation Failed');
			break;
			case 500:
				$this->_mk_status_reason(500,'Internal Server Error');
			break;
			case 501:
				$this->_mk_status_reason(501,'Not Implemented');
			break;
			case 502:
				$this->_mk_status_reason(502,'Bad Gateway');
			break;
			case 503:
				$this->_mk_status_reason(503,'Service Unavailable');
			break;
			case 504:
				$this->_mk_status_reason(504,'Gateway Time-out');
			break;
			case 505:
				$this->_mk_status_reason(505,'HTTP Version not supported');
			break;
		}
	}

	function _mk_status_reason($code,$reason)
	{
		$this->_headers['status_code'] = sprintf('HTTP/%s %d %s',
							 $this->_param['version'],
							 $code,
							 $reason);
	}

	function _chk_realm($realm)
	{
		$realm = strval($realm);

		if(preg_match('/^[\x09\x20\x21\x23-\x7E]+$/',$realm) === 1)
			return($realm);

		return(false);
	}

	function set_authent_basic($realm='')
	{
		if(($realm = $this->_chk_realm($realm)) === false)
			$realm = '';

		$this->_headers['www-authenticate'] = sprintf('WWW-Authenticate: Basic realm="%s"',$realm);

		return(true);
	}

	function send($die=false)
	{
		foreach($this->_headers as $value)
			header($value);

		if((bool) $die === true)
			die();
	}
}

?>
