<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

class dwho_filter
{
	var $_validator		= null;
	var $_error		= array();
	var $_errnb		= 0;
	var $_result		= array();

	function dwho_filter($validator=array())
	{
		if(is_array($validator) === true
		&& empty($validator) === false)
			$this->_validator = &$validator;
	}

	function set_validator($validator)
	{
		if(is_array($validator) === false)
			return(false);

		$this->_validator = $validator;

		return(true);
	}

	function chk($v,$set=true,$validator=null)
	{
		if(is_array($validator) === true)
			$this->_validator = $validator;
		else if(is_array($this->_validator) === false)
			trigger_error('Invalid validator array in '.__CLASS__,E_USER_ERROR);

		$set = (bool) $set;

		$this->reset_result();
		$this->reset_error();

		if(empty($this->_validator) === true)
			return(false);

		foreach($this->_validator as $key => $arr)
			$this->_chk_key_value($key,$arr,$v,$set);

		return(($this->_errnb < 1));
	}

	function reset_result()
	{
		$this->_result = array();
	}

	function reset_error()
	{
		$this->_error = array();
		$this->_errnb = 0;
	}

	function reset_all()
	{
		$this->_validator = null;
		$this->reset_result();
		$this->reset_error();
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
		return(dwho_uint($this->_errnb));
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

	function chk_date($date,$fill=false)
	{
		$r = array();

		$date = strval($date);
		$format = $year = $month = $day = '';

		if(preg_match('/^(2[0-9]{3})'.
				'(?:-(0?[1-9]|1[0-2])'.
				   '(?:-(0?[1-9]|1[0-9]|2[0-9]|3[0-1]))?)?$/',$date,$match) !== 1)
			return(false);

		$r['format'] = '%Y';
		$year = $r['date'] = $r['dateint'] = $match[1];

		if((bool) $fill === true)
		{
			if(isset($match[2]) === false)
				$match[2] = '01';

			if(isset($match[3]) === false)
				$match[3] = '01';
		}

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
		if(($dbeg = dwho_filter::chk_date($dbeg)) === false
		|| ($dend = dwho_filter::chk_date($dend)) === false
		|| $dbeg['format'] !== $dend['format']
		|| $dbeg['dateint'] > $dend['dateint'])
			return(false);

		return(array('dbeg' => $dbeg,'dend' => $dend));
	}

	function chk_time($time,$fill=false)
	{
		$time = strval($time);

		if(preg_match('/^([01]?[0-9]|2[0-3])'.
				'(?:\:([0-5]?[0-9])'.
				   '(?:\:([0-5]?[0-9]))?)?$/',$time,$match) !== 1)
			return(false);

		$r['format'] = '%H';
		$hour = $r['time'] = $r['timeint'] = sprintf('%02u',$match[1]);

		if((bool) $fill === true)
		{
			if(isset($match[2]) === false)
				$match[2] = '00';

			if(isset($match[3]) === false)
				$match[3] = '00';
		}

		if(isset($match[2]) === true)
		{
			$r['format'] .= '%M';
			$minute = sprintf('%02u',$match[2]);
			$r['time'] .= ':'.$minute;
			$r['timeint'] .= $minute;
		}

		if(isset($match[3]) === true)
		{
			$r['format'] .= '%S';
			$second = sprintf('%02u',$match[3]);
			$r['time'] .= ':'.$second;
			$r['timeint'] .= $second;
		}

		return($r);
	}

	function chk_between_time($tbeg,$tend)
	{
		if(($tbeg = dwho_filter::chk_time($tbeg)) === false
		|| ($tend = dwho_filter::chk_time($tend)) === false
		|| $tbeg['format'] !== $tend['format']
		|| $tbeg['timeint'] > $tend['timeint'])
			return(false);

		return(array('tbeg' => $tbeg,'tend' => $tend));
	}

	function chk_datetime($datetime,$fill=false)
	{
		$exp = explode(' ',strval($datetime),2);

		if(isset($exp[0],$exp[1]) === false
		|| ($date = dwho_filter::chk_date($exp[0],$fill)) === false
		|| ($time = dwho_filter::chk_time($exp[1],$fill)) === false)
			return(false);

		return(array('format'		=> $date['format'].' '.$time['format'],
			     'datetime'		=> $date['date'].' '.$time['time'],
			     'datetimeint'	=> $date['dateint'].$time['timeint']));
	}

	function chk_between_datetime($dtbeg,$dtend)
	{
		if(($dtbeg = dwho_filter::chk_datetime($dtbeg)) === false
		|| ($dtend = dwho_filter::chk_datetime($dtend)) === false
		|| $dtbeg['format'] !== $dtend['format']
		|| $dtbeg['datetimeint'] > $dtend['datetimeint'])
			return(false);

		return(array('dtbeg' => $dtbeg,'dtend' => $dtend));
	}

	function chk_bool($value)
	{
		return((int) dwho_bool($value));
	}

	function chk_cast($type,$value)
	{
		if(dwho_has_len($value) === false)
			return(null);

		switch($type)
		{
			case 'spunct':
				return(dwho_is_spunct($value));
			case 'numspunct':
				return(dwho_is_numspunct($value));
			case 'alspunct':
				return(dwho_is_alspunct($value));
			case 'alnumspunct':
				return(dwho_is_alnumspunct($value));
			case 'punct':
				return(ctype_punct($value));
			case 'numpunct':
				return(dwho_is_numpunct($value));
			case 'alpunct':
				return(dwho_is_alpunct($value));
			case 'alnumpunct':
				return(dwho_is_alnumpunct($value));
			case 'sgraph':
				return(dwho_is_sgraph($value));
			case 'sgraph_tab':
				return(dwho_is_sgraph_tab($value));
			case 'sgraph_crlf':
				return(dwho_is_sgraph_crlf($value));
			case 'sgraph_crlf_tab':
				return(dwho_is_sgraph_crlf_tab($value));
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
			case 'digit':
				return(ctype_digit($value));
			case 'xdigit':
				return(ctype_xdigit($value));
			case 'uint':
			case 'uinteger':
			case 'unsigned-integer':
				return(dwho_is_uint($value));
			case 'ureal':
			case 'ufloat':
			case 'udouble':
			case 'unsigned-real':
			case 'unsigned-float':
			case 'unsigned-double':
				return(dwho_is_unumber($value));
			case 'ulongint':
			case 'ulonginteger':
			case 'unsigned-longinteger':
				$cast = dwho_ulongint($value);
				break;
			case 'int':
			case 'integer':
				$cast = intval($value);
				break;
			case 'real':
			case 'float':
			case 'double':
				return(dwho_is_number($value));
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
		if(strlen($value) < (int) $min)
			return(false);

		return(true);
	}

	function chk_maxlen($max,$value)
	{
		if(strlen($value) > (int) $max)
			return(false);

		return(true);
	}

	function chk_between($beg,$end,$value,$mod=null)
	{
		$value_is_number = dwho_is_number($value);

		if(dwho_is_number($beg) === false)
		{
			$beg = strval($beg);
			$end = strval($end);
			$value = strval($value);
		}
		else if(dwho_is_number($end) === false
		|| $value_is_number === false)
			return(false);

		if($value < $beg || $value > $end)
			return(false);
		else if(is_numeric($mod) === false)
			return(true);
		else if($value_is_number === false)
			return(false);
		else if($value % $mod === 0
		|| $beg === $value
		|| $end === $value)
			return(true);

		return(false);
	}

	function callback($func,$value,$param=null)
	{
		if(is_string($func) === true)
		{
			$afunc = explode('::',$func,2);

			if(isset($afunc[0],$afunc[1]) === true)
			{
				$type = 'method';
				$func = $afunc;
			}
			else
				$type = 'function';
		}
		else if(is_array($func) === true
		&& isset($func[0],$func[1]) === true)
			$type = 'method';
		else
			return(false);

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
		else if(dwho_has_len($value) === false)
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
			if(isset($this->_validator[$key]) === false)
				return(false);

			$arr = &$this->_validator[$key];

			$set = isset($arr['set']) === false ? true : (bool) $arr['set'];

			if($set === false && $value === null)
				return($value);
		}

		$chk = isset($arr['chk']) === false ? 1 : dwho_uint($arr['chk']);
		$strict = isset($arr['strict']) === false;

		if(isset($arr['bool']) === true)
		{
			if($set === false && $chk === 2 && dwho_has_len($value) === false)
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
			if($chk === 2 && dwho_has_len($value) === false)
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
			if($chk === 2 && dwho_has_len($value) === false)
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
			if($chk === 2 && dwho_has_len($value) === false)
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
			if($chk === 2 && dwho_has_len($value) === false)
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
			if($chk === 2 && dwho_has_len($value) === false)
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
			if($chk === 2 && dwho_has_len($value) === false)
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
			$value = dwho_eol($value);

		if(isset($arr['notag']) === false)
			$value = dwho_notag($value);
		else if(isset($arr['notagscript']) === true)
			$value = dwho_notagscript($value);

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

		if(dwho_issa('between',$arr) === true
		&& isset($arr['between'][0],$arr['between'][1]) === true)
		{
			if(isset($arr['between'][2]) === true)
				$mod = $arr['between'][2];
			else
				$mod = null;

			if($this->chk_between($arr['between'][0],$arr['between'][1],$value,$mod) === false)
			{
				$this->_set_error($key,'between false');
				return(false);
			}
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
			$value = dwho_notag($value);
		else if(isset($arr['notagscript']) === true)
			$value = dwho_notagscript($value);

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

		if(($val = $this->chk_value($key,$val)) === false)
			return(false);
		else if(isset($arr['egal'],$v[$arr['egal']]) === true)
		{
			if($v[$arr['egal']] !== $val)
			{
				$this->_set_error($key,'no egal');
				return(false);
			}
		}

		$this->_set_result($key,$val);

		return(true);
	}
}

?>
