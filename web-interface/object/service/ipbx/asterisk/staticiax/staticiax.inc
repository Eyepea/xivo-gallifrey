<?php

xivo_service_asterisk::required(array('realstatic','realstatic.inc'),true);

class xivo_service_asterisk_realstatic_staticiax extends xivo_service_asterisk_realstatic
{
	var $_filename	= 'iax.conf';

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
			      '(:[a-z0-9_\.-]+)?'.
			      '@([a-z0-9\.-]+)(:[0-9]{1,5})?$#i',$val,$match) !== 1)
			return(false);

		$r = array();

		$r['username'] = $match[1];
		$r['host'] = $match[3];

		if(isset($match[2]) === true && $match[2] !== '')
			$r['password'] = substr($match[2],1);
		else
			$r['password'] = '';

		if(isset($match[4]) === true && $match[4] !== '')
			$r['port'] = substr($match[4],1);
		else
			$r['port'] = '';

		return($r);
	}
}

?>
