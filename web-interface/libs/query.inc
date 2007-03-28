<?php

define('XIVO_QUERY_MODE_RAW',1);
define('XIVO_QUERY_MODE_QOT',2);
define('XIVO_QUERY_MODE_UNTRIM',4);
define('XIVO_QUERY_MODE_URAW',XIVO_QUERY_MODE_RAW | XIVO_QUERY_MODE_UNTRIM);
define('XIVO_QUERY_MODE_UQOT',XIVO_QUERY_MODE_QOT | XIVO_QUERY_MODE_UNTRIM);

class xivo_query
{
	var $orig	= array('request' => array(),'qstring' => array());
	var $raw	= array();
	var $uraw	= array();
	//var $_raw	= array();
	var $qot	= array();
	var $uqot	= array();
	var $uqo	= array();
	var $_separator	= array('in' => '&','out' => '&');
	var $_mquotes	= false;

	function xivo_query()
	{
		if(isset($_SERVER['QUERY_STRING']) === true)
			parse_str($_SERVER['QUERY_STRING'],$this->_orig['qstring']);

		if(defined('XIVO_SEP_ARG_IN') === true)
			$this->_separator['in'] = XIVO_SEP_ARG_IN;

		if(defined('XIVO_SEP_ARG_OUT') === true)
			$this->_separator['out'] = XIVO_SEP_ARG_OUT;

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

		$this->_mquotes = (bool) get_magic_quotes_gpc();
		$this->raw = &$this->request_meth_raw();
	}

	function &query_string($type='request',$mode=0)
	{
		$r = array();
		
		$type = (string) $type;
		$mode = xivo_uint($mode);

		$type = isset($this->_orig[$type]) === true ? $type : 'request';
		$data = $this->_orig[$type];

		if(($mode & XIVO_QUERY_MODE_RAW) !== 0)
			$r = &$this->_filter($data,$mode,($this->_mquotes === true ? true : null));	// remove spaces and is unquoted
		else
			$r = &$this->_filter($data,$mode,($this->_mquotes === false ? false : null));	// remove spaces and is quoted

		return(($ref = &$r));
	}

	function get_qr($name)
	{
		if(isset($this->raw[$name]) === true)
			return($this->raw[$name]);

		if(isset($this->_orig['request'][$name]) === false)
			return(null);

		$quote = $this->_mquotes === true ? true : null;

		$this->raw[$name] = &$this->_filter($this->_orig['request'][$name],XIVO_QUERY_MODE_RAW,$quote);

		return($this->raw[$name]);
	}

	function get_uqr($name)
	{
		if(isset($this->uraw[$name]) === true)
			return($this->uraw[$name]);
	
		if(isset($this->_orig['request'][$name]) === false)
			return(null);

		$quote = $this->_mquotes === true ? true : null;

		$this->uraw[$name] = &$this->_filter($this->_orig['request'][$name],XIVO_QUERY_MODE_URAW,$quote);

		return($this->uraw[$name]);
	}

	function get_qq($name)
	{
		if(isset($this->qot[$name]) === true)
			return($this->qot[$name]);

		if(isset($this->_orig['request'][$name]) === false)
			return(null);

		$quote = $this->_mquotes === false ? false : null;

		$this->qot[$name] = &$this->_filter($this->_orig['request'][$name],XIVO_QUERY_MODE_QOT,$quote);

		return($this->qot[$name]);
	}

	function get_uqq($name)
	{
		if(isset($this->uqot[$name]) === true)
			return($this->uqot[$name]);

		if(isset($this->_orig['request'][$name]) === false)
			return(null);

		$quote = $this->_mquotes === false ? false : null;

		$this->uqot[$name] = &$this->_filter($this->_orig['request'][$name],XIVO_QUERY_MODE_UQOT,$quote);

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

	function &request_meth_raw()
	{
		return($this->query_string('request',XIVO_QUERY_MODE_RAW));
	}

	function request_meth_raw_str($pre='',$key='',$sep='')
	{
		return($this->mk_query_str($this->request_meth_raw(),$pre,$key,$sep));
	}

	function &request_meth_uraw()
	{
		return($this->query_string('request',XIVO_QUERY_MODE_URAW));
	}

	function request_meth_uraw_str($pre='',$key='',$sep='')
	{
		return($this->mk_query_str($this->request_meth_uraw(),$pre,$key,$sep));
	}

	function &request_meth_qot()
	{
		return($this->query_string('request',XIVO_QUERY_MODE_QOT));
	}

	function request_meth_qot_str($pre='',$key='',$sep='')
	{
		return($this->mk_query_str($this->request_meth_qot(),$pre,$key,$sep));
	}

	function &request_orig()
	{
		return($this->_orig['request']);
	}

	function request_orig_str($pre='',$key='',$sep='')
	{
		return($this->mk_query_str($this->_orig['request'],$pre,$key,$sep));
	}

	function &qstring_raw()
	{
		return($this->query_string('qstring',XIVO_QUERY_MODE_RAW));
	}

	function qstring_raw_str($pre='',$key='',$sep='')
	{
		return($this->mk_query_str($this->qstring_raw(),$pre,$key,$sep));
	}

	function &qstring_qot()
	{
		return($this->query_string('qstring',XIVO_QUERY_MODE_QOT));
	}

	function qstring_qot_str($pre='',$key='',$sep='')
	{
		return($this->mk_query_str($this->qstring_qot(),$pre,$key,$sep));
	}

	function &qstring_orig()
	{
		return($this->_orig['qstring']);
	}

	function &qstring_orig_str()
	{
		return($_SERVER['QUERY_STRING']);
	}

	function get_path_info($arr,$beg=0)
	{
		$r = array();

		$beg = (int) $beg;

		$search = array('@/+@','@^/@','@/$@');
		$replace = array('/','','');

		if(is_array($arr) === false || empty($arr) === true || isset($_SERVER['PATH_INFO']) === false)
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

	function mk_query_str($a,$pre='',$key='',$sep='')
	{
		$r = '';
		$a = (array) $a;
		$sep = (string) $sep;
		$key = (string) $key;
		$sep = (string) $sep;

		if($sep === '')
		{
			if(defined('XIVO_SEP_ARG_OUT') === true)
				$sep = XIVO_SEP_ARG_OUT;
			else
				$sep = '&';
		}

		if(($arr = xivo_get_aks($a)) === false)
			return($r);

		$r = array();
	
		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = &$arr['keys'][$i];
			$v = &$a[$k];

			$tmp = $pre !== '' && is_int($k) === true ? $pre.$k : $k;

			if($key !== '')
				$tmp = $key.'['.$tmp.']';

			if(is_array($v) === true || is_object($v) === true)
				$r[] = xivo_query::mk_query_str($v,'','',$sep);
			else
				$r[] = $tmp.'='.urlencode($v);
		}

		return(implode($sep,$r));
	}

	function &_filter($value,$mode=0,$quote=null)
	{
		$mode = xivo_uint($mode);

		if(is_array($value) === false)
		{
			if(($mode & XIVO_QUERY_MODE_UNTRIM) === 0)
				$value = trim((string) $value);

			if($quote !== null)
				$value = (bool) $quote === true ? stripslashes($value) : addslashes($value);

			return(($ref = &$value));
		}

		if(($arr = xivo_get_aks($value)) === false)
			return(($ref = &$value));

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = &$arr['keys'][$i];
			$value[$k] = &$this->_filter($value[$k],$mode,$quote);
		}

		return(($ref = &$value));
	}
}

function xivo_mk_query($a,$pre='',$key='',$sep='')
{
	return(xivo_query::mk_query_str($a,$pre,$key,$sep));
}

?>
