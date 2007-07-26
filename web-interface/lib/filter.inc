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

	function chk(&$v,$set=true)
	{
		$a = (array) $this->_content;
		$set = (bool) $set;

		$this->_reset_err();

		if(($arr = xivo_get_aks($a)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$this->_chk_key_val($key,$a,$v,$set);
		}

		return(($this->_err['nb'] > 0 ? false : true));
	}

	function _reset_err()
	{
		$this->_err = array('msg' => array(),'nb' => 0);
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

	function chk_email($email,$strict=false)
	{
		$nb = 0;
		$email = strtolower((string) $email);
		$strict = (bool) $strict;
		$len = strlen($email);

		if($len < 6 || $len > 255
		|| preg_match('/^[a-z0-9_-]+(\.[a-z0-9_-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*\.[a-z]{2,4}$/',$email) !== 1)
			return(false);

		if($strict === false || function_exists('getmxrr') === false)
			return($email);

		$a = explode('@',$email);
		$m = array();

		if(getmxrr($a[1],$m) === true && $nb === 0)
			return($email);

		return(false);
	}

	function chk_email_strict($email)
	{
		return(xivo_filter::chk_email($email,true));
	}

	function chk_ipv4($ip)
	{
		$ip = (string) $ip;
	
		if(($long = ip2long($ip)) === -1 || $long === false)
			return(false);

		return(true);
	}

	function chk_ipv4_strict($ip)
	{
		$ip = (string) $ip;

		if(xivo_filter::chk_ipv4($ip) === true
		&& preg_match('#^([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.'.
				'([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.'.
				'([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.'.
				'([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$#i',$ip) === 1)
			return(true);

		return(false);
	}

	function chk_host($host)
	{
		$host = (string) $host;

		if(preg_match('#^[a-z0-9-]+(\.[a-z0-9-]+)*\.[a-z]{2,4}$#i',$host) === 1)
			return(true);

		return(false);
	}

	function chk_host_ipv4($value)
	{
		if(xivo_filter::chk_ipv4($value) === true || xivo_filter::chk_host($value) === true)
			return(true);

		return(false);
	}

	function chk_host_ipv4_strict($value)
	{
		if(xivo_filter::chk_ipv4_strict($value) === true || xivo_filter::chk_host($value) === true)
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
			case 'specials':
				return(xivo_cast_specials($value));
			case 'numspecials':
				return(xivo_cast_numspecials($value));
			case 'alphaspecials':
				return(xivo_cast_alphaspecials($value));
			case 'alphanumspecials':
				return(xivo_cast_alphanumspecials($value));
			case 'print':
				$cast = xivo_print($value);

				return(($cast === false ? false : true));
			case 'printable':
				return(xivo_printable($value));
			case 'printable_crlf':
				return(xivo_printable_crlf($value));
			case 'printable_tab':
				return(xivo_printable_tab($value));
			case 'digit':
				$cast = xivo_digit($value);

				return(($cast === false ? false : true));
			case 'xdigit':
				$cast = xivo_xdigit($value);

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
			if(function_exists($func) === false)
				return(false);
		}

		return(call_user_func($func,$value));
	}

	function chk_key_value($arr,$value)
	{
		if(is_array($arr) === false)
			return(false);

		return(in_array($value,$arr));
	}

	function chk_unarray_value($arr,$value)
	{
		if(is_array($arr) === false || is_array($value) === false)
			return(false);

		$value = array_values($value);

		if(($nb = count($value)) === 0)
			return(null);

		for($i = 0;$i < $nb;$i++)
		{
			if(in_array($value[$i],$arr) === true)
				return(false);
		}
		
		return(true);
	}

	function chk_array_value($arr,$value)
	{
		if(is_array($arr) === false || is_array($value) === false)
			return(false);

		$value = array_values($value);

		if(($nb = count($value)) === 0)
			return(null);

		for($i = 0;$i < $nb;$i++)
		{
			if(in_array($value[$i],$arr) === false)
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

			$set = isset($arr['set']) === false ? true : (bool) $arr['set'];
	
			if($set === false && $value === null)
				return($value);
		}

		$chk = isset($arr['chk']) === false ? 1 : xivo_uint($arr['chk']);
		$strict = isset($arr['strict']) === false ? true : false;

		if(isset($arr['bool']) === true)
			return($this->chk_bool($value));

		if(isset($arr['arrvalue']) === true)
		{
			if($chk === 2 && isset($value{0}) === false)
				return('');

			$arrvalue = $this->chk_array_value($arr['arrvalue'],$value);

			if($arrvalue === false && $strict === true)
			{
				$this->_mk_err($key,'invalid value');
				return(false);
			}

			if($arrvalue === true)
				return($value);
		}

		if(isset($arr['unarrvalue']) === true)
		{
			if($chk === 2 && isset($value{0}) === false)
				return('');

			$unarrvalue = $this->chk_unarray_value($arr['unarrvalue'],$value);

			if($unarrvalue === false)
			{
				$this->_mk_err($key,'invalid value');
				return(false);
			}
		}

		if(isset($arr['arrkey']) === true)
		{
			if($chk === 2 && isset($value{0}) === false)
				return('');

			$arrkey = $this->chk_key_value($arr['arrkey'],$value);

			if($arrkey === false && $strict === true)
			{
				$this->_mk_err($key,'invalid value');
				return(false);
			}

			if($arrkey === true)
				return($value);
		}

		if(isset($arr['unarrkey']) === true)
		{
			if($chk === 2 && isset($value{0}) === false)
				return('');

			$unarrkey = $this->chk_key_value($arr['unarrkey'],$value);

			if($unarrkey === true)
			{
				$this->_mk_err($key,'invalid value');
				return(false);
			}
		}

		if(isset($arr['eol']) === true)
			$value = xivo_eol($value);

		if(isset($arr['notag']) === false)
			$value = xivo_notag($value);
		else if(isset($arr['notagscript']) === true)
			$value = xivo_notagscript($value);
	
		$len = strlen($value);

		if($chk === 0 && $len > 0)
		{
			$this->_mk_err($key,'no empty');
			return(false);
		}
	
		if($chk === 1 && $len === 0)
		{
			$this->_mk_err($key,'empty');
			return(false);
		}
	
		if($chk === 2 && $len === 0)
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

	function _chk_key_val($key,&$a,&$v,$set=true)
	{
		$a = (array) $a;
		$v = (array) $v;
		$set = (bool) $set;
	
		if(isset($a[$key]) === false)
		{
			$this->_mk_result($key);
			return(true);
		}
	
		$arr = &$a[$key];

		if($set === false && isset($v[$key]) === false)
			return(true);
	
		$set = isset($arr['set']) === false ? true : (bool) $arr['set'];
	
		if($set === true)
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
