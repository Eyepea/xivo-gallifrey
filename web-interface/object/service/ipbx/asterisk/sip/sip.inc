<?php

xivo_service_asterisk::required(array('realstatic','realstatic.inc'),true);

class xivo_service_asterisk_realstatic_sip extends xivo_service_asterisk_realstatic
{
	var $_filename	= 'sip.conf';

	function _prepare($data)
	{
		if($data['category'] === 'general'
		&& $data['var_name'] === 'register'
		&& is_scalar($data['var_val']) === true
		&& ($register = $this->parse_register($data['var_val'])) !== false)
			$data = array_merge($register,$data);

		return(parent::_prepare($data));
	}

	function parse_register($val)
	{
		if(preg_match('#^((?:[a-z0-9_\.-]+)(?:@[a-z0-9\.-]+)?)'.
			      '(?:\:([a-z0-9_\.-]+)(:[a-z0-9_\.-]+)?)?'.
			      '@([a-z0-9\.-]+)(:[0-9]{1,5})?(/[a-z0-9]+)?$#i',$val,$match) !== 1)
			return(false);

		$r = array();

		$r['username'] = $match[1];
		$r['host'] = $match[4];

		if(isset($match[2]) === true)
			$r['password'] = $match[2];
		else
			$r['password'] = '';

		if($r['password'] !== '' && isset($match[3]) === true && $match[3] !== '')
			$r['authuser'] = substr($match[3],1);
		else
			$r['authuser'] = '';

		if(isset($match[5]) === true && $match[5] !== '')
			$r['port'] = substr($match[5],1);
		else
			$r['port'] = '';

		if(isset($match[6]) === true && $match[6] !== '')
			$r['contact'] = substr($match[6],1);
		else
			$r['contact'] = '';

		return($r);
	}
}

?>
