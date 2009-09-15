<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

class xivo_filter
{
	var $_content		= array();
	var $_error		= array();
	var $_errnb		= 0;
	var $_result		= array();

	function xivo_filter($content=array())
	{
		if(is_array($content) === true
		&& empty($content) === false)
			$this->_content = &$content;
	}

	function chk($v,$set=true,$content=null)
	{
		if(is_array($content) === true)
			$a = $this->_content = $content;
		else if(is_array($this->_content) === true)
			$a = $this->_content;
		else
			trigger_error('Invalid content array in '.__CLASS__,E_USER_ERROR);

		$set = (bool) $set;

		$this->_reset_result();
		$this->_reset_error();

		if(empty($a) === true)
			return(false);

		foreach($a as $key => $arr)
			$this->_chk_key_value($key,$arr,$v,$set);

		return(($this->_errnb > 0 ? false : true));
	}

	function _reset_result()
	{
		$this->_result = array();
	}

	function _reset_error()
	{
		$this->_error = array();
		$this->_errnb = 0;
	}

	function get_result()
	{
		return($this->_result);
	}

	function get_error()
	{
		return($this->_error);
	}

	function get_errnb()
	{
		return(xivo_uint($this->_errnb));
	}

	function _set_error($key,$msg)
	{
		$this->_result[$key] = '';
		$this->_error[$key] = $msg;
		$this->_errnb++;
	}

	function _set_result($key,$val='')
	{
		$this->_result[$key] = $val;
	}

	function chk_date($date)
	{
		$r = array();

		$date = strval($date);
		$format = $year = $month = $day = '';

		if(preg_match('/^(2[0-9]{3})(?:-(0?[1-9]|1[0-2])(?:-(0?[1-9]|1[0-9]|2[0-9]|3[0-1]))?)?$/',$date,$match) !== 1)
			return(false);

		$r['format'] = '%Y';
		$year = $r['date'] = $r['dateint'] = $match[1];

		if(isset($match[2]) === true)
		{
			$r['format'] .= '%m';
			$month = sprintf('%02u',$match[2]);
			$r['date'] .= '-'.$month;
			$r['dateint'] .= $month;
		}

		if(isset($match[3]) === true)
		{
			$r['format'] .= '%d';
			$day = sprintf('%02u',$match[3]);
			$r['date'] .= '-'.$day;
			$r['dateint'] .= $day;

			if(checkdate($month,$day,$year) === false)
				return(false);
		}

		return($r);
	}

	function chk_between_date($dbeg,$dend)
	{
		if(($dbeg = xivo_filter::chk_date($dbeg)) === false
		|| ($dend = xivo_filter::chk_date($dend)) === false)
			return(false);
		else if($dbeg['format'] !== $dend['format'] || $dbeg['dateint'] > $dend['dateint'])
			return(false);

		return(array('dbeg' => $dbeg,'dend' => $dend));
	}

	function chk_email($email,$strict=false)
	{
		$nb = 0;
		$email = strtolower(strval($email));
		$len = strlen($email);

		if($len < 6 || $len > 255
		|| preg_match('/^[a-z0-9_-]+(?:\.[a-z0-9_\-\+]+)*@[a-z0-9-]+(?:\.[a-z0-9-]+)*\.[a-z]{2,4}$/',$email) !== 1)
			return(false);
		else if((bool) $strict === false || function_exists('getmxrr') === false)
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

	function chk_macaddr($macaddr)
	{
		if(preg_match_all('/([A-F0-9]{2})[-: ]?/i',strval($macaddr),$match) !== 6)
			return(false);

		return(strtoupper(implode(':',$match[1])));
	}

	function chk_ipv4($ip)
	{
		return((xivo_ip2long($ip) !== false));
	}

	function chk_ipv4_in_lhost($ip,$host)
	{
		$ip = strval($ip);
		$host = strval($host);

		if(xivo_filter::chk_ipv4_strict($ip) === false)
		{
			$ip = gethostbyname($ip);

			if(xivo_filter::chk_ipv4_strict($ip) === false)
				return(false);
		}

		if(xivo_filter::chk_ipv4_strict($host) === true
		|| xivo_filter::chk_ipv4_subnet($host) === true)
			$lhost = (array) $host;
		else if(($lhost = gethostbynamel($host)) === false)
			return(false);

		$nb = count($lhost);

		for($i = 0;$i < $nb;$i++)
		{
			if($ip === $lhost[$i]
			|| xivo_filter::chk_ipv4_in_subnet($ip,$lhost[$i]) !== false)
				return(true);
		}

		return(false);
	}

	function chk_ipv4_in_subnet($ip,$sub)
	{
		$sub = strval($sub);

		if(($pos = strrpos($sub,'/')) === false)
			return(false);

		$ipsub = substr($sub,0,$pos);
		$mask = substr($sub,$pos+1);

		if(xivo_filter::chk_ipv4_strict($ip) === false
		|| ($mask = xivo_filter::chk_ipv4_subnet($ipsub,$mask,true)) === false)
			return(false);

		$r = array();
		$r['ip'] = $ip;
		$r['ipsubnet'] = $ipsub;
		$r['mask'] = $mask;
		$r['subnet'] = $sub;
		$r['netmask'] = (0xFFFFFFFF>>(32-$mask))<<(32-$mask);
		$r['wildcard'] =~ $r['netmask'];
		$r['network'] = ip2long($ipsub);
		$r['broadcast'] = $r['network'] | $r['wildcard'];

		$ip = ip2long($ip);

		if($ip > $r['network'] && $ip < $r['broadcast'])
			return($r);

		return(false);
	}

	function chk_ipv4_subnet($ip,$mask=null,$type=XIVO_IPV4_NETMASK_ALL,$ret=false)
	{
		$ip = strval($ip);

		if($mask === null)
		{
			if(($pos = strrpos($ip,'/')) === false)
				return(false);

			$mask = substr($ip,$pos+1);
			$ip = substr($ip,0,$pos);
		}

		$mask = strval($mask);

		if(($mask = xivo_filter::chk_ipv4_netmask($mask,$type,$ret)) === false
		|| xivo_filter::chk_ipv4_strict($ip) === false)
			return(false);

		return($mask);
	}

	function chk_ipv4_netmask($nm,$type=XIVO_IPV4_NETMASK_ALL,$ret=false)
	{
		$r = false;

		$type = intval($type);

		if(($type & XIVO_IPV4_NETMASK_ALL) === 0)
			$type = XIVO_IPV4_NETMASK_ALL;

		if(($type & XIVO_IPV4_NETMASK_BIT) !== 0)
			$r = xivo_filter::chk_ipv4_netmask_bit($nm);

		if($r === false && ($type & XIVO_IPV4_NETMASK_DOTDEC) !== 0)
			$r = xivo_filter::chk_ipv4_netmask_dotdec($nm);

		if($r !== false && (bool) $ret === false)
			$r = true;

		return($r);
	}

	function chk_ipv4_netmask_bit($bit)
	{
		if(xivo_is_uint($bit) === false
		|| $bit > 32)
			return(false);

		return(true);
	}

	function chk_ipv4_netmask_dotdec($nm)
	{
		if(($nm = xivo_ip2long($nm)) === 0)
			return(0);
		else if($nm === -1)
			return(32);
		else if($nm === false
		|| ($inv = ($nm ^ 0xFFFFFFFF)) === 0
		|| ($inv & ($inv + 1)) !== 0)
			return(false);

		return(32 - intval(log($inv + 1,2)));
	}

	function chk_ipv4_strict($ip)
	{
		$ip = strval($ip);

		return(($ip === long2ip(ip2long($ip))));
	}

	function chk_host($host)
	{
		$host = strval($host);
		$len = strlen($host);

		if($len > 3
		&& $len < 256
		&& preg_match('#^[a-z0-9-]+(?:\.[a-z0-9-]+)*\.[a-z]{2,4}$#i',$host) === 1)
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

	function chk_host_subnet_ipv4_strict($value)
	{
		if(xivo_filter::chk_ipv4_strict($value) === true
		|| xivo_filter::chk_host($value) === true
		|| xivo_filter::chk_ipv4_subnet($value) === true)
			return(true);

		return(false);
	}

	function chk_bool($value)
	{
		return((int) xivo_bool($value));
	}

	function chk_cast($type,$value)
	{
		if(xivo_has_len($value) === false)
			return(null);

		switch($type)
		{
			case 'spunct':
				return(xivo_is_spunct($value));
			case 'numspunct':
				return(xivo_is_numspunct($value));
			case 'alspunct':
				return(xivo_is_alspunct($value));
			case 'alnumspunct':
				return(xivo_is_alnumspunct($value));
			case 'punct':
				return(ctype_punct($value));
			case 'numpunct':
				return(xivo_is_numpunct($value));
			case 'alpunct':
				return(xivo_is_alpunct($value));
			case 'alnumpunct':
				return(xivo_is_alnumpunct($value));
			case 'sgraph':
				return(xivo_is_sgraph($value));
			case 'sgraph_tab':
				return(xivo_is_sgraph_tab($value));
			case 'sgraph_crlf':
				return(xivo_is_sgraph_crlf($value));
			case 'sgraph_crlf_tab':
				return(xivo_is_sgraph_crlf_tab($value));
			case 'space':
				return(ctype_space($value));
			case 'graph':
				return(ctype_graph($value));
			case 'alpha':
				return(ctype_alpha($value));
			case 'alnum':
				return(ctype_alnum($value));
			case 'punct':
				return(ctype_punct($value));
			case 'num':
			case 'digit':
				return(ctype_digit($value));
			case 'xdigit':
				return(ctype_xdigit($value));
			case 'uint':
			case 'uinteger':
			case 'unsigned-integer':
				$cast = xivo_uint($value);
				break;
			case 'ureal':
			case 'ufloat':
			case 'udouble':
			case 'unsigned-real':
			case 'unsigned-float':
			case 'unsigned-double':
				$cast = xivo_ufloat($value,0,false);
				$value = str_replace(',','.',strval($value));
				break;
			case 'ulongint':
			case 'ulonginteger':
			case 'unsigned-longinteger':
				$cast = xivo_ulongint($value);
				break;
			case 'int':
			case 'integer':
				$cast = intval($value);
				break;
			case 'real':
			case 'float':
			case 'double':
				$cast = floatval($value);
				$value = str_replace(',','.',strval($value));
				break;
			case 'string':
				$cast = strval($value);
				break;
			default:
				return(null);
		}

		return(((string) $cast === $value));
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
		$beg = strval($beg);
		$end = strval($end);
		$value = strval($value);

		if($value < $beg || $value > $end)
			return(false);

		return(true);
	}

	function callback($func,$value,$param=null)
	{
		if(is_array($func) === false)
		{
			$func = (string) $func;

			$afunc = explode('::',$func,2);

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

		if($param === null)
			return(call_user_func($func,$value));
		else
		{
			$args = array();
			$args[0] = $value;

			if(is_array($param) === true)
				$args = array_merge($args,$param);
			else
				$args[1] = $param;

			return(call_user_func_array($func,$args));
		}
	}

	function chk_array_key($arr,$value)
	{
		if(is_array($arr) === false)
			return(false);

		return(in_array($value,$arr));
	}

	function chk_noarray_value($arr,$value)
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

	function chk_array_value($arr,$value,$unique=false)
	{
		if(is_array($arr) === false || is_array($value) === false)
			return(false);

		$value = array_values($value);

		if(($nb = count($value)) === 0)
			return(null);

		$unique = (bool) $unique;
		$uniqval = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(in_array($value[$i],$arr) === false)
				return(false);
			else if($unique === false)
				continue;
			else if(isset($uniqval[$value[$i]]) === true)
				return(false);
			else
				$uniqval[$value[$i]] = 1;
		}

		return(true);
	}

	function chk_str_in_strlist($strlist,$value,$unique=false)
	{
		if(is_array($strlist) === false
		|| ($nb = count($strlist)) === 0)
			return(false);
		else if(xivo_has_len($value) === false)
			return(null);

		$strlist = array_values($strlist);

		$unique = (bool) $unique;

		for($i = 0;$i < $nb;$i++)
		{
			if($unique === true && mb_substr_count($value,$strlist[$i]) > 1)
				return(false);
			else if(($pos = mb_strpos($value,$strlist[$i])) === false)
				continue;

			$prefix = mb_substr($value,0,$pos);
			$suffix = mb_substr($value,$pos + mb_strlen($strlist[$i]));
			$value = $prefix.$suffix;
		}

		return(isset($value[0]) === false);
	}

	function chk_char_in_charlist($charlist,$value,$unique=false)
	{
		if(is_array($charlist) === false
		|| is_scalar($value) === false)
			return(false);
		else if(($len = mb_strlen($value)) === 0)
			return(null);

		$unique = (bool) $unique;
		$uniqval = array();

		for($i = 0;$i < $len;$i++)
		{
			$char = mb_substr($value,$i,1);

			if(in_array($char,$charlist) === false)
				return(false);
			else if($unique === false)
				continue;
			else if(isset($uniqval[$char]) === true)
				return(false);
			else
				$uniqval[$char] = 1;
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
		$strict = isset($arr['strict']) === false;

		if(isset($arr['bool']) === true)
		{
			if($set === false && $chk === 2 && xivo_has_len($value) === false)
				return('');

			return($this->chk_bool($value));
		}

		if(isset($arr['value']) === true)
		{
			if($chk === 2 && empty($value) === true)
				return('');

			$arrvalue = $this->chk_array_value($arr['value'],$value);

			if($arrvalue === false && $strict === true)
			{
				$this->_set_error($key,'invalid value');
				return(false);
			}
			else if($arrvalue === true)
				return($value);
		}

		if(isset($arr['uvalue']) === true)
		{
			if($chk === 2 && empty($value) === true)
				return('');

			$arrvalue = $this->chk_array_value($arr['uvalue'],$value,true);

			if($arrvalue === false && $strict === true)
			{
				$this->_set_error($key,'invalid value');
				return(false);
			}
			else if($arrvalue === true)
				return($value);
		}

		if(isset($arr['novalue']) === true)
		{
			if($chk === 2 && empty($value) === true)
				return('');

			$noarrvalue = $this->chk_noarray_value($arr['novalue'],$value);

			if($noarrvalue === false)
			{
				$this->_set_error($key,'invalid value');
				return(false);
			}
		}

		if(isset($arr['key']) === true)
		{
			if($chk === 2 && xivo_has_len($value) === false)
				return('');

			$arrkey = $this->chk_array_key($arr['key'],$value);

			if($arrkey === false && $strict === true)
			{
				$this->_set_error($key,'invalid value');
				return(false);
			}
			else if($arrkey === true)
				return($value);
		}

		if(isset($arr['nokey']) === true)
		{
			if($chk === 2 && xivo_has_len($value) === false)
				return('');

			$noarrkey = $this->chk_array_key($arr['nokey'],$value);

			if($noarrkey === true)
			{
				$this->_set_error($key,'invalid value');
				return(false);
			}
		}

		if(is_scalar($value) === false)
		{
			$this->_set_error($key,'invalid value');
			return(false);
		}

		if(isset($arr['str']) === true)
		{
			if($chk === 2 && xivo_has_len($value) === false)
				return('');

			$strlist = $this->chk_str_in_strlist($arr['str'],$value);

			if($strlist === false && $strict === true)
			{
				$this->_set_error($key,'invalid value');
				return(false);
			}
			else if($strlist === true)
				return($value);
		}

		if(isset($arr['ustr']) === true)
		{
			if($chk === 2 && xivo_has_len($value) === false)
				return('');

			$strlist = $this->chk_str_in_strlist($arr['ustr'],$value,true);

			if($strlist === false && $strict === true)
			{
				$this->_set_error($key,'invalid value');
				return(false);
			}
			else if($strlist === true)
				return($value);
		}

		if(isset($arr['char']) === true)
		{
			if($chk === 2 && xivo_has_len($value) === false)
				return('');

			$charlist = $this->chk_char_in_charlist($arr['char'],$value);

			if($charlist === false && $strict === true)
			{
				$this->_set_error($key,'invalid value');
				return(false);
			}
			else if($charlist === true)
				return($value);
		}

		if(isset($arr['uchar']) === true)
		{
			if($chk === 2 && xivo_has_len($value) === false)
				return('');

			$charlist = $this->chk_char_in_charlist($arr['uchar'],$value,true);

			if($charlist === false && $strict === true)
			{
				$this->_set_error($key,'invalid value');
				return(false);
			}
			else if($charlist === true)
				return($value);
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
			$this->_set_error($key,'no empty');
			return(false);
		}

		if($chk === 1 && $len === 0)
		{
			$this->_set_error($key,'empty');
			return(false);
		}

		if($chk === 2 && $len === 0)
		{
			$this->_set_result($key);
			return('');
		}

		if(isset($arr['cast']) === true && $len > 0)
		{
			if($this->chk_cast($arr['cast'],$value) === false)
			{
				$this->_set_error($key,'cast');
				return(false);
			}
		}

		if(isset($arr['minlen']) === true && $this->chk_minlen($arr['minlen'],$value) === false)
		{
			$this->_set_error($key,'minimum length');
			return(false);
		}

		if(isset($arr['maxlen']) === true && $this->chk_maxlen($arr['maxlen'],$value) === false)
		{
			$this->_set_error($key,'maximum length');
			return(false);
		}

		if(xivo_issa('between',$arr) === true && isset($arr['between'][0],$arr['between'][1]) === true
		&& $this->chk_between($arr['between'][0],$arr['between'][1],$value) === false)
		{
			$this->_set_error($key,'between false');
			return(false);
		}

		if(isset($arr['callback']) === true)
		{
			if(isset($arr['callbackarg']) === false)
				$callbackarg = null;
			else
				$callbackarg = $arr['callbackarg'];

			if($this->callback($arr['callback'],$value,$callbackarg) === false)
			{
				$this->_set_error($key,'callback false');
				return(false);
			}
		}

		if(isset($arr['regexp']) === true && preg_match($arr['regexp'],$value) !== 1)
		{
			$this->_set_error($key,'regexp false');
			return(false);
		}

		return($value);
	}

	function chk_set($key,&$value)
	{
		if(isset($value[$key]) === false)
		{
			$this->_set_error($key,'undefined');
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

	function _chk_key_value($key,&$arr,&$v,$set=true)
	{
		$arr = (array) $arr;
		$v = (array) $v;

		if((bool) $set === false && isset($v[$key]) === false)
		{
			if(array_key_exists($key,$v) === true)
				$this->_set_result($key,null);
			return(true);
		}

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
			$this->_set_result($key);
			return(true);
		}

		$val = &$v[$key];

		if(isset($arr['egal'],$v[$arr['egal']]) === true)
		{
			if($v[$arr['egal']] !== $val)
			{
				$this->_set_error($key,'no egal');
				return(false);
			}
		}
		else if(($val = $this->chk_value($key,$val)) === false)
				return(false);

		$this->_set_result($key,$val);

		return(true);
	}
}

?>
