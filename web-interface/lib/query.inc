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

define('DWHO_QUERY_MODE_RAW',1);
define('DWHO_QUERY_MODE_QOT',2);
define('DWHO_QUERY_MODE_UNTRIM',4);
define('DWHO_QUERY_MODE_URAW',DWHO_QUERY_MODE_RAW | DWHO_QUERY_MODE_UNTRIM);
define('DWHO_QUERY_MODE_UQOT',DWHO_QUERY_MODE_QOT | DWHO_QUERY_MODE_UNTRIM);

class dwho_query
{
	var $orig	= array('request' => array(),'qstring' => array());
	var $raw	= array();
	var $uraw	= array();
	var $qot	= array();
	var $uqot	= array();
	var $uqo	= array();
	var $qraw	= array();
	var $input	= null;
	var $_separator	= array('in' => '&','out' => '&');
	var $_mquotes	= false;

	function dwho_query()
	{
		if(isset($_SERVER['QUERY_STRING']) === true)
			parse_str($_SERVER['QUERY_STRING'],$this->_orig['qstring']);

		if(defined('DWHO_SEP_ARG_IN') === true)
			$this->_separator['in'] = DWHO_SEP_ARG_IN;

		if(defined('DWHO_SEP_ARG_OUT') === true)
			$this->_separator['out'] = DWHO_SEP_ARG_OUT;

		$this->_mquotes = (bool) get_magic_quotes_gpc();

		if(isset($_SERVER['REQUEST_METHOD']) === false)
			return(false);

		switch($_SERVER['REQUEST_METHOD'])
		{
			case 'GET':
				$this->_orig['request'] = $_GET;
				break;
			case 'POST':
				$this->_orig['request'] = $_POST;
				break;
			default:
				return(false);
		}

		$this->raw = &$this->request_meth_raw();
	}

	function &query_string($type='request',$mode=0)
	{
		$r = array();

		$type = (string) $type;
		$mode = dwho_uint($mode);

		$type = isset($this->_orig[$type]) === true ? $type : 'request';
		$data = $this->_orig[$type];

		if(($mode & DWHO_QUERY_MODE_RAW) !== 0)
			$r = &$this->_filter($data,
					     $mode,
					     ($this->_mquotes === true ? true : null));		// remove spaces and is unquoted
		else
			$r = &$this->_filter($data,
					     $mode,
					     ($this->_mquotes === false ? false : null));	// remove spaces and is quoted

		return(($ref = &$r));
	}

	function go($location='',$query='')
	{
		$location = (string) $location;

		if($query === true)
		{
			$q = $this->qstring_raw_str();

			if($this->get_qs(DWHO_SESS_NAME) === DWHO_SESS_ID)
				$query = false;
		}
		else if(is_array($query) === true)
			$q = $this->mk_query_str($query);
		else
			$q = strval($query);

		if($query !== false)
		{
			if(dwho_has_len($q) === true)
				$q .= DWHO_SEP_ARG_OUT;

			$q .= DWHO_SESS_STR;
		}

		if(dwho_has_len($q) === true)
			$location .= '?'.$q;

		die(header('Location: '.$location));
	}

	function get($name,$default=null)
	{
		if(($r = $this->get_qr($name)) !== null)
			return($r);
		else if(($r = $this->get_qs($name)) !== null)
			return($r);

		return($default);
	}

	function get_qr($name)
	{
		if(isset($this->raw[$name]) === true)
			return($this->raw[$name]);

		if(isset($this->_orig['request'][$name]) === false)
			return(null);

		$quote = $this->_mquotes === true ? true : null;

		$this->raw[$name] = &$this->_filter($this->_orig['request'][$name],DWHO_QUERY_MODE_RAW,$quote);

		return($this->raw[$name]);
	}

	function get_qs($name)
	{
		if(isset($this->qraw[$name]) === true)
			return($this->qraw[$name]);

		if(isset($this->_orig['qstring'][$name]) === false)
			return(null);

		$quote = $this->_mquotes === true ? true : null;

		$this->qraw[$name] = &$this->_filter($this->_orig['qstring'][$name],DWHO_QUERY_MODE_RAW,$quote);

		return($this->qraw[$name]);
	}

	function get_uqr($name)
	{
		if(isset($this->uraw[$name]) === true)
			return($this->uraw[$name]);

		if(isset($this->_orig['request'][$name]) === false)
			return(null);

		$quote = $this->_mquotes === true ? true : null;

		$this->uraw[$name] = &$this->_filter($this->_orig['request'][$name],DWHO_QUERY_MODE_URAW,$quote);

		return($this->uraw[$name]);
	}

	function get_qq($name)
	{
		if(isset($this->qot[$name]) === true)
			return($this->qot[$name]);

		if(isset($this->_orig['request'][$name]) === false)
			return(null);

		$quote = $this->_mquotes === false ? false : null;

		$this->qot[$name] = &$this->_filter($this->_orig['request'][$name],DWHO_QUERY_MODE_QOT,$quote);

		return($this->qot[$name]);
	}

	function get_uqq($name)
	{
		if(isset($this->uqot[$name]) === true)
			return($this->uqot[$name]);

		if(isset($this->_orig['request'][$name]) === false)
			return(null);

		$quote = $this->_mquotes === false ? false : null;

		$this->uqot[$name] = &$this->_filter($this->_orig['request'][$name],DWHO_QUERY_MODE_UQOT,$quote);

		return($this->uqot[$name]);
	}

	function get_qo($name)
	{
		if(isset($this->_orig['request'][$name]) === true)
			return($this->_orig['request'][$name]);

		return(null);
	}

	function get_uqo($name)
	{
		if(isset($this->uqo[$name]) === true)
			return($this->uqo[$name]);

		if(isset($this->_orig['request'][$name]) === false)
			return(null);

		$this->uqo[$name] = &$this->_filter($this->_orig['request'][$name]);

		return($this->uqot[$name]);
	}

	function get_qot($name)
	{
		if(isset($this->_orig['request'][$name]) === true)
			return($this->_orig['request'][$name]);
	}

	function get_qos($name)
	{
		if(isset($this->_orig['qstring'][$name]) === true)
			return($this->_orig['qstring'][$name]);

		return(null);
	}

	function get_input()
	{
		if($this->input === null)
			$this->input = file_get_contents('php://input');

		return($this->input);
	}

	function &request_meth_raw()
	{
		return(($ref = &$this->query_string('request',DWHO_QUERY_MODE_RAW)));
	}

	function request_meth_raw_str($sep='')
	{
		return($this->mk_query_str($this->request_meth_raw(),$sep));
	}

	function &request_meth_uraw()
	{
		return(($ref = &$this->query_string('request',DWHO_QUERY_MODE_URAW)));
	}

	function request_meth_uraw_str($sep='')
	{
		return($this->mk_query_str($this->request_meth_uraw(),$sep));
	}

	function &request_meth_qot()
	{
		return(($ref = &$this->query_string('request',DWHO_QUERY_MODE_QOT)));
	}

	function request_meth_qot_str($sep='')
	{
		return($this->mk_query_str($this->request_meth_qot(),$sep));
	}

	function &request_orig()
	{
		return(($ref = &$this->_orig['request']));
	}

	function request_orig_str($sep='')
	{
		return($this->mk_query_str($this->_orig['request'],$sep));
	}

	function &qstring_raw()
	{
		return(($ref = &$this->query_string('qstring',DWHO_QUERY_MODE_RAW)));
	}

	function qstring_raw_str($sep='')
	{
		return($this->mk_query_str($this->qstring_raw(),$sep));
	}

	function &qstring_qot()
	{
		return(($ref = $this->query_string('qstring',DWHO_QUERY_MODE_QOT)));
	}

	function qstring_qot_str($sep='')
	{
		return($this->mk_query_str($this->qstring_qot(),$sep));
	}

	function qstring_orig()
	{
		return($this->_orig['qstring']);
	}

	function qstring_orig_str()
	{
		return($_SERVER['QUERY_STRING']);
	}

	function get_path_info($arr,$beg=0)
	{
		$r = array();

		$beg = (int) $beg;

		$search = array('@/+@','@^/@','@/$@');
		$replace = array('/','','');

		if(is_array($arr) === false
		|| empty($arr) === true
		|| isset($_SERVER['PATH_INFO']) === false)
			return($r);

		$path_info = preg_replace($search,$replace,$_SERVER['PATH_INFO']);

		$cnt = count($arr);

		if($cnt > 20)
			$cnt = 20;

		$path = explode('/',$path_info,$cnt+1);

		for($i = $beg;$i < $cnt;$i++)
		{
			if(isset($arr[$i],$path[$i]) === true)
				$r[$arr[$i]] = $path[$i];
		}

		return($r);
	}

	function encode_query($str,$type='')
	{
		$str = (string) $str;

		$regexp = 'a-zA-Z0-9-\._~\!\$\'\(\)\*,;:@\/\? ';

		if($type === 'val')
			$regexp .= '=';
		else if($type !== 'key')
			return(urlencode($str));

		if(preg_match_all('/[^'.$regexp.']+/',$str,$match,PREG_OFFSET_CAPTURE) === 0)
			return($str);

		$match = $match[0];
		$nb = count($match);

		$r = '';
		$len = strlen($str);

		if($match[0][1] > 0)
			$r = substr($str,0,$match[0][1]);

		for($i = 0;$i < $nb;$i++)
		{
			$r .= urlencode($match[$i][0]);
			$mlen = strlen($match[$i][0]);

			if(isset($match[$i+1]) === true)
			{
				$pos = $match[$i][1] + $mlen;
				$r .= substr($str,$pos,$match[$i+1][1] - $pos);
				continue;
			}

			if(($match[$i][1] + $mlen) < $len)
				$r .= substr($str,$match[$i][1] + $mlen);
		}

		$r = str_replace(' ','+',$r);

		return($r);
	}

	function mk_query_str($a,$sep='')
	{
		$r = '';
		$a = (array) $a;
		$sep = (string) $sep;

		if($sep === '')
		{
			if(defined('DWHO_SEP_ARG_OUT') === true)
				$sep = DWHO_SEP_ARG_OUT;
			else
				$sep = '&';
		}

		if(empty($a) === true)
			return($r);

		$r = array();

		foreach($a as $k => $v)
		{
			if(is_array($v) === true || is_object($v) === true)
				$r[] = dwho_query::mk_query_str($v,$sep);
			else if(($val = dwho_query::encode_query($v,'val')) !== '')
				$r[] = dwho_query::encode_query($k,'key').'='.$val;
		}

		return(implode($sep,$r));
	}

	function &_filter($value,$mode=0,$quote=null)
	{
		$mode = dwho_uint($mode);

		if(is_array($value) === false)
		{
			if(($mode & DWHO_QUERY_MODE_UNTRIM) === 0)
				$value = trim($value);

			if($quote !== null)
				$value = (bool) $quote === true ? stripslashes($value) : addslashes($value);

			return(($ref = &$value));
		}
		else if(empty($value) === true)
			return(($ref = &$value));

		reset($value);

		while(list($key,$val) = each($value))
			$value[$key] = &$this->_filter($val,$mode,$quote);

		return(($ref = &$value));
	}
}

?>
