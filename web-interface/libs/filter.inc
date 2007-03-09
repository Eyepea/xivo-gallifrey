<?php

class xivo_filter
{
	var $_content		= array();
	var $_err		= array('msg' => array(),'nb' => 0);
	var $_result		= array();

	function xivo_filter($content)
	{
		if(is_array($content) === false)
			trigger_error('Invalid content array in '.__CLASS__,E_USER_ERROR);

		$this->_content = &$content;
	}

	function chk(&$v)
	{
		$a = (array) $this->_content;

		if(($arr = xivo_get_aks($a)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];

			$this->_chk_key_val($key,$a,$v);
		}

		return(($this->_err['nb'] > 0 ? false : true));
	}

	function get_result()
	{
		return($this->_result);
	}

	function get_err()
	{
		return($this->_err['msg']);	
	}

	function get_nb_err()
	{
		return($this->_err['nb']);
	}

	function _mk_err($key,$msg)
	{
		$this->_result[$key] = '';
		$this->_err['msg'][$key] = $msg;
		$this->_err['nb']++;
	}

	function _mk_result($key,$val='')
	{
		$this->_result[$key] = $val;
	}

	function chk_ipv4($ip)
	{
		$ip = (string) $ip;
	
		if(($long = ip2long($ip)) === -1 || $long === false)
			return(false);

		return(true);
	}

	function chk_host($host)
	{
		$host = (string) $host;

		if(gethostbyname($host) === $host)
			return(false);

		return(true);
	}

	function chk_host_ipv4($value,$strict=false)
	{
		$strict = (bool) $strict;

		if(xivo_filter::chk_host($value) === true || xivo_filter::chk_ipv4($value) === true)
			return(true);

		return(false);
	}

	function chk_host_ipv4_strict($value)
	{
		if(xivo_filter::chk_host_ipv4($value,true) === true
		&& preg_match('#^(([a-z0-9-]+)(\.[a-z0-9-]+)*(\.[a-z]{2,4})'.
				'|([1-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.'.
				'([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.'.
				'([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.'.
				'([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))$#i',$value) === 1)
			return(true);

		return(false);
	}

	function chk_bool($value)
	{
		return((xivo_bool($value) === true ? 1 : 0));
	}

	function chk_cast($type,$value)
	{
		if(isset($value{0}) === false)
			return(null);

		switch($type)
		{
			case 'digit':
				$cast = xivo_digit($value);

				return(($cast === false ? false : true));
			case 'uint':
				$cast = xivo_uint($value);
				break;
			case 'int':
				$cast = (int) $value;
				break;
			case 'float':
				$cast = (float) $value;
				break;
			case 'string':
				$cast = (string) $value;
				break;
			default:
				return(null);
		}

		if((string) $cast !== $value)
			return(false);

		return(true);
	}

	function chk_minlen($min,$value)
	{
		$min = (int) $min;

		if(strlen($value) < $min)
			return(false);

		return(true);
	}

	function chk_maxlen($max,$value)
	{
		$max = (int) $max;

		if(strlen($value) > $max)
			return(false);

		return(true);
	}

	function chk_between($beg,$end,$value)
	{
		$beg = (int) $beg;
		$end = (int) $end;
		$value = (int) $value;

		if($value < $beg || $value > $end)
			return(false);

		return(true);
	}

	function chk_func($func,$value)
	{
		if(is_array($func) === false)
		{
			$func = (string) $func;

			$afunc = explode('::',$func);

			if(isset($afunc[0],$afunc[1]) === true)
			{
				$type = 'method';
				$func = $afunc;
			}
			else
				$type = 'function';
		}
		else
			$type = 'method';
		
		if($type === 'method')
		{
			if(is_callable($func[0],$func[1]) === false)
				return(false);
		}
		else
		{
			if(function_exists($func,$value) === false)
				return(false);
		}

		return(call_user_func($func,$value));
	}

	function chk_key_value($arr,$value)
	{
		if(is_array($arr) === false)
			return(false);

		$arr = array_flip($arr);

		return(array_key_exists($value,$arr));
	}

	function chk_array_value($arr,$value)
	{
		if(is_array($arr) === false || is_array($value) === false)
			return(false);

		$value = array_values($value);
		$arr = array_flip($arr);

		if(($nb = count($value)) === 0)
			return(null);

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($arr[$value[$i]]) === false)
				return(false);
		}
		
		return(true);
	}

	function chk_value($key,$value,$arr=null)
	{
		if(is_array($arr) === false)
		{
			if(isset($this->_content[$key]) === false)
				return(false);

			$arr = &$this->_content[$key];

			$arr['set'] = isset($arr['set']) === false ? true : (bool) $arr['set'];
	
			if($arr['set'] === false && $value === null)
				return($value);
		}

		$arr['chk'] = isset($arr['chk']) === false ? 1 : xivo_uint($arr['chk']);

		if(isset($arr['bool']) === true)
			return($this->chk_bool($value));

		if(isset($arr['arrvalue']) === true)
		{
			if($this->chk_array_value($arr['arrvalue'],$value) !== true)
			{
				$this->_mk_err($key,'invalid value');
				return(false);
			}

			return($value);
		}

		if(isset($arr['arrkey']) === true)
		{
			if($this->chk_key_value($arr['arrkey'],$value) === false)
			{
				$this->_mk_err($key,'invalid value');
				return(false);
			}

			return($value);
		}

		if(isset($arr['notag']) === false)
			$value = xivo_notag($value);
		else if(isset($arr['notagscript']) === true)
			$value = xivo_notagscript($value);
	
		$len = strlen($value);

		if($arr['chk'] === 0 && $len > 0)
		{
			$this->_mk_err($key,'no empty');
			return(false);
		}
	
		if($arr['chk'] === 1 && $len === 0)
		{
			$this->_mk_err($key,'empty');
			return(false);
		}
	
		if($arr['chk'] === 2 && $len === 0)
		{
			$this->_mk_result($key);
			return('');
		}
	
		if(isset($arr['cast']) === true && $len > 0)
		{
			if($this->chk_cast($arr['cast'],$value) === false)
			{
				$this->_mk_err($key,'cast');
				return(false);
			}
		}
	
		if(isset($arr['minlen']) === true && $this->chk_minlen($arr['minlen'],$value) === false)
		{
			$this->_mk_err($key,'minimum length');
			return(false);
		}
	
		if(isset($arr['maxlen']) === true && $this->chk_maxlen($arr['maxlen'],$value) === false)
		{
			$this->_mk_err($key,'maximum length');
			return(false);
		}

		if(xivo_issa('between',$arr) === true && isset($arr['between'][0],$arr['between'][1]) === true
		&& $this->chk_between($arr['between'][0],$arr['between'][1],$value) === false)
		{
			$this->_mk_err($key,'between false');
			return(false);
		}
	
		if(isset($arr['func']) === true && $this->chk_func($arr['func'],$value) === false)
		{
			$this->_mk_err($key,'func false');
			return(false);
		}

		if(isset($arr['preg']) === true && preg_match((string) $arr['preg'],$value) !== 1)
		{
			$this->_mk_err($key,'preg false');
			return(false);
		}
	
		if(isset($arr['ereg']) === true && ereg((string) $arr['ereg'],$value) === false)
		{
			$this->_mk_err($key,'ereg false');
			return(false);
		}
	
		if(isset($arr['eregi']) === true && eregi((string) $arr['eregi'],$value) === false)
		{
			$this->_mk_err($key,'eregi false');
			return(false);
		}

		return($value);
	}

	function chk_set($key,&$value)
	{
		if(isset($value[$key]) === false)
		{
			$this->_mk_err($key,'undefined');
			return(false);
		}

		return(true);
	}

	function _mk_value($value,$arr)
	{
		if(isset($arr['notag']) === false)
			$value = xivo_notag($value);
		else if(isset($arr['notagscript']) === true)
			$value = xivo_notagscript($value);
	
		return($value);
	}

	function _chk_key_val($key,&$a,&$v)
	{
		$a = (array) $a;
		$v = (array) $v;
	
		if(isset($a[$key]) === false)
		{
			$this->_mk_result($key);
			return(true);
		}
	
		$arr = &$a[$key];
	
		$arr['set'] = isset($arr['set']) === false ? true : (bool) $arr['set'];
	
		if($arr['set'] === true)
		{
			if(isset($v[$key]) === false && isset($arr['bool']) === true)
				$v[$key] = null;
			else if($this->chk_set($key,$v) === false)
				return(false);
		}
		else if(isset($v[$key]) === false || count($arr) === 1)
		{
			$this->_mk_result($key);
			return(true);
		}
	
		$val = &$v[$key];
	
		if(isset($arr['egal'],$v[$arr['egal']]) === true)
		{
			if($v[$arr['egal']] !== $val)
			{
				$this->_mk_err($key,'no egal');
				return(false);
			}
		}
		else
		{
			if(($val = $this->chk_value($key,$val)) === false)
				return(false);
		}
	
		$this->_mk_result($key,$val);
	
		return(true);
	}
}

?>
