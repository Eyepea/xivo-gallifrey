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

class xivo_uri
{
	var $_encode		= array();
	var $_host		= array('ipliteral'	=> 1,
					'ipv4'		=> 2,
					'reg_name'	=> 3);
	var $_regexp		= array();
	var $_reipvfutureport	= '';

	function xivo_uri()
	{
		$gen_delims	= ':\/\?#\[\]@';
		$sub_delims	= '\!\$&\'\(\)\*\+,;=';
		$reserved	= $gen_delims.$sub_delims;
		$unreserved	= 'a-zA-Z0-9-\._~';
		$ipvfuture	= 'v[\da-fA-F]+\.['.$unreserved.$sub_delims.':]+';
		$port		= '[0-9]{0,5}';

		$this->_encode = array();
		$this->_encode['user']		= '/([^'.$unreserved.$sub_delims.']+)/';
		$this->_encode['passwd']	= '/([^'.$unreserved.$sub_delims.':]+)/';
		$this->_encode['reg_name']	= '/([^'.$unreserved.$sub_delims.']+)/';
		$this->_encode['path']		= '/([^'.$unreserved.$sub_delims.':@\/]+)/';
		$this->_encode['query_key']	= '/([^'.$unreserved.'\!\$\'\(\)\*,;:@\/\? ]+)/';
		$this->_encode['query_val']	= '/([^'.$unreserved.'\!\$\'\(\)\*,;:@\/\? =]+)/';
		$this->_encode['fragment']	= '/([^'.$unreserved.$sub_delims.':@\/\?])/';

		$this->_regexp = array();
		$this->_regexp['rfc3986']	= '@^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?$@';
		$this->_regexp['scheme']	= '/^[a-zA-Z][a-zA-Z0-9\+\-\.]*$/';
		$this->_regexp['user']		= '/^['.$unreserved.$sub_delims.'%]+$/';
		$this->_regexp['host']		= $this->_regexp['user'];
		$this->_regexp['passwd']	= '/^['.$unreserved.$sub_delims.'%:]+$/';
		$this->_regexp['port']		= '/^'.$port.'$/';
		$this->_regexp['path']		= '/^['.$unreserved.$sub_delims.'%:@\/]+$/';
		$this->_regexp['query']		= '/^['.$unreserved.$sub_delims.'%:@\/\?]+$/';
		$this->_regexp['ipvfuture']	= '/^'.$ipvfuture.'$/';

		$this->_reipvfutureport		= '/^\[([\da-fA-F:\.]+|'.$ipvfuture.')\](\:'.$port.')?$/';
	}

	function build_uri($arr,$encode=true,$type_host=null)
	{
		$encode = (bool) $encode;
		$scheme = $authority = $userinfo = $hostport = $path = $query = $fragment = '';

		if(is_array($arr) === false)
			return(false);
		else if(xivo_has_len($arr,'scheme') === true)
		{
			if($this->valid_scheme($arr['scheme']) === false)
				return(false);

			$scheme = $arr['scheme'];
		}

		if(xivo_issa('authority',$arr) === true)
		{
			if(isset($arr['authority']['host']) === true)
				$arr['host']	= $arr['authority']['host'];

			if(isset($arr['authority']['port']) === true)
				$arr['port']	= $arr['authority']['port'];

			if(isset($arr['authority']['user']) === true)
				$arr['user']	= $arr['authority']['user'];

			if(isset($arr['authority']['passwd']) === true)
				$arr['passwd']	= $arr['authority']['passwd'];
		}

		if(xivo_has_len($arr,'host') === true)
		{
			$arr['host'] = (string) $arr['host'];

			switch((empty($type_host) === true ? $this->get_host_type($arr['host']) : $type_host))
			{
				case $this->_host['ipliteral']:
					if($this->valid_ipliteral($arr['host']) === false)
						return(false);
					break;
				case $this->_host['ipv4']:
					if($this->valid_ipv4($arr['host']) === false)
						return(false);
					break;
				case $this->_host['reg_name']:
				default:
					if($encode === true)
						$arr['host'] = $this->encode($arr['host'],'reg_name');
					break;
			}

			$hostport = $arr['host'];
		}

		if(xivo_has_len($arr,'port') === true)
		{
			if($this->valid_port($arr['port']) === false)
				return(false);

			$hostport .= ':'.$arr['port'];
		}

		if(xivo_has_len($arr,'user') === true)
		{
			$arr['user'] = (string) $arr['user'];
			if($encode === true)
				$arr['user'] = $this->encode($arr['user'],'user');

			$userinfo = $arr['user'];
		}

		if(xivo_has_len($arr,'passwd') === true)
		{
			$arr['passwd'] = (string) $arr['passwd'];
			if($encode === true)
				$arr['passwd'] = $this->encode($arr['passwd'],'passwd');

			$userinfo .= ':'.$arr['passwd'];
		}

		if($userinfo !== '' && $hostport !== '')
			$authority = $userinfo.'@'.$hostport;
		else if($userinfo !== '')
			$authority = $userinfo.'@';
		else if($hostport !== '')
			$authority = $hostport;

		if(xivo_has_len($arr,'path') === true)
		{
			$arr['path'] = (string) $arr['path'];

			if($authority !== '' && $arr['path']{0} !== '/')
				return(false);
			else if($encode === false)
				$path = $arr['path'];
			else
			{
				$path = $this->encode($arr['path'],'path');

				if($authority === ''
				&& $scheme === ''
				&& ($pos = strpos($path,'/')) !== false)
					$path = str_replace(':',
							    '%3A',
							    substr($path,0,$pos)).
						substr($path,$pos);
			}
		}

		if(xivo_issa('query',$arr) === true)
			$query = $this->build_query($arr['query'],$encode);

		if(xivo_has_len($arr,'fragment') === true)
		{
			$arr['fragment'] = (string) $arr['fragment'];
			if($encode === true)
				$fragment = $this->encode($arr['fragment'],'fragment');
			else
				$fragment = $arr['fragment'];
		}

		$r = '';

		if($scheme !== '')
			$r .= $scheme.':';

		if($authority !== '')
			$r .= '//'.$authority;

		if($path !== '')
		{
			if($authority === '' && substr($path,0,2) === '//')
				$r .= '//';

			$r .= $path;
		}

		if($query !== '')
			$r .= '?'.$query;

		if($fragment !== '')
			$r .= '#'.$fragment;

		return($r);
	}

	function parse_uri($uri,$decode=true)
	{
		$decode = (bool) $decode;

		if(($uri = $this->uri_split($uri)) === false)
			return(false);
		else if(isset($uri['authority']) === true
		&& ($uri['authority'] = $this->split_authority($uri['authority'])) === false)
			return(false);
		else if(isset($uri['query']) === true
		&& ($uri['query'] = $this->split_query($uri['query'])) === false)
			return(false);
		else if(isset($uri['scheme']) === true && $this->valid_scheme($uri['scheme']) === false)
			return(false);

		if(xivo_issa('authority',$uri) === true)
		{
			$authority = &$uri['authority'];

			if(isset($authority['user']) === true)
			{
				if($this->valid_user($authority['user']) === false)
					return(false);
				else if($decode === true)
					$authority['user'] = urldecode($authority['user']);
			}

			if(isset($authority['passwd']) === true)
			{
				if($this->valid_passwd($authority['passwd']) === false)
					return(false);
				else if($decode === true)
					$authority['passwd'] = urldecode($authority['passwd']);
			}

			if(isset($authority['host']) === true)
			{
				if($this->valid_host($authority['host']) === false)
					return(false);
				else if($decode === true && $this->get_host_type($authority['host']) === $this->_host['reg_name'])
					$authority['host'] = urldecode($authority['host']);
			}

			if(isset($authority['port']) === true && $this->valid_port($authority['port']) === false)
				return(false);
		}

		if(isset($uri['path']) === true)
		{
			if($this->valid_path($uri['path']) === false)
				return(false);
			else if(isset($authority) === true && $uri['path']{0} !== '/')
				return(false);
			else if(isset($authority,$uri['scheme']) === false
			&& ($pos = strpos($uri['path'],':')) !== false
			&& ($poslash = strpos($uri['path'],'/')) !== false
			&& $pos < $poslash)
				return(false);

			if($decode === true)
				$uri['path'] = urldecode($uri['path']);
		}

		if(xivo_issa('query',$uri) === true && ($nb = count($uri['query'])) !== 0)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$ref = &$uri['query'][$i];

				if(xivo_has_len($ref,0) === true)
				{
					if($this->valid_query($ref[0]) === false)
						return(false);
					else if($decode === true)
						$ref[0] = urldecode($ref[0]);
				}

				if(xivo_has_len($ref,1) === false)
					continue;
				else if($this->valid_query($ref[1]) === false)
					return(false);
				else if($decode === true)
					$ref[1] = urldecode($ref[1]);
			}
		}

		if(isset($uri['fragment']) === true)
		{
			if($this->valid_fragment($uri['fragment']) === false)
				return(false);
			else if($decode === true)
				$uri['fragment'] = urldecode($uri['fragment']);
		}

		return($uri);
	}

	function uri_split($uri)
	{
		if(preg_match($this->_regexp['rfc3986'],$uri,$match) !== 1)
			return(false);

		$r = array();

		if(xivo_has_len($match,2) === true)
			$r['scheme'] = $match[2];

		if(xivo_has_len($match,4) === true)
			$r['authority'] = $match[4];

		if(xivo_has_len($match,5) === true)
			$r['path'] = $match[5];

		if(xivo_has_len($match,7) === true)
			$r['query'] = $match[7];

		if(xivo_has_len($match,9) === true)
			$r['fragment'] = $match[9];

		return($r);
	}

	function split_authority($authority)
	{
		$authority = (string) $authority;

		if($authority === '')
			return(null);

		$r = array();

		if(($pos = strpos($authority,'@')) === false)
			$hostport = $authority;
		else if($pos === 0)
			$hostport = substr($authority,1);
		else
		{
			$userinfo = substr($authority,0,$pos);
			$hostport = substr($authority,$pos + 1);

			if(($pos = strpos($userinfo,':')) === false)
				$r['user'] = $userinfo;
			else
			{
				if($pos !== 0)
					$r['user'] = substr($userinfo,0,$pos);

				if(isset($userinfo{$pos + 1}) === true)
					$r['passwd'] = substr($userinfo,$pos + 1);
			}
		}

		if($hostport === '')
			return($r);
		else if($hostport{0} === '[')
		{
			if(preg_match($this->_reipvfutureport,$hostport,$match) !== 1)
				return(false);

			$r['host'] = '['.$match[1].']';

			if(isset($match[2],$match[2]{1}) === true)
				$r['port'] = substr($match[2],1);
		}
		else if(($pos = strpos($hostport,':')) !== false)
		{
			if($pos !== 0)
				$r['host'] = substr($hostport,0,$pos);

			if(isset($hostport{$pos + 1}) === true)
				$r['port'] = substr($hostport,$pos + 1);
		}
		else
			$r['host'] = $hostport;

		return($r);
	}

	function split_query($query)
	{
		$query = (string) $query;

		if($query === '' || ($assignments = explode('&',$query)) === false)
			return(false);

		$nb = count($assignments);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if($assignments[$i] === ''
			|| ($sa = explode('=',$assignments[$i],2)) === false)
				continue;

			$r[] = $sa;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function build_query($query,$encode=true)
	{
		$encode = (bool) $encode;

		$r = '';

		if(is_array($query) === false)
			return($r);

		$query = array_values($query);

		if(($nb = count($query)) === 0)
			return($r);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$query[$i];
			$r[$i] = '';

			if(is_array($ref) === false)
				continue;

			if(xivo_has_len($ref,0) === true)
			{
				$ref[0] = (string) $ref[0];
				if($encode === true)
					$r[$i] = $this->encode($ref[0],'query_key');
				else
					$r[$i] = $ref[0];
			}

			if(xivo_has_len($ref,1) === false)
			{
				if($r[$i] === '')
					unset($r[$i]);
				continue;
			}

			$ref[1] = (string) $ref[1];

			if($encode === true)
				$r[$i] .= '='.$this->encode($ref[1],'query_val');
			else
				$r[$i] .= '='.$ref[1];
		}

		return(implode('&',$r));
	}

	function get_host_type($host)
	{
		if(xivo_has_len($host) === false)
			return($this->_host['reg_name']);

		$host = (string) $host;

		if($host{0} === '[')
			return($this->_host['ipliteral']);
		else if($this->valid_ipv4($host) === true)
			return($this->_host['ipv4']);
		else
			return($this->_host['reg_name']);
	}

	function encode($str,$type='')
	{
		$str = (string) $str;

		if(isset($this->_encode[$type]) === false)
			return(urlencode($str));

		$rs = preg_split($this->_encode[$type],$str,-1,PREG_SPLIT_DELIM_CAPTURE);
		xivo_print_r($rs);

		if(($nb = count($rs)) === 1)
		{
			if($type === 'query_key' || $type === 'query_val')
				return(str_replace(' ','+',$rs[0]));

			return($rs[0]);
		}

		$r = '';

		for($i = 0;$i < $nb;$i += 2)
		{
			$r .= $rs[$i];

			if(($i + 1) < $nb)
				$r .= urlencode($rs[$i + 1]);
		}

		if($type === 'query_key' || $type === 'query_val')
			return(str_replace(' ','+',$r));

		return($r);
	}

	function valid_scheme($scheme)
	{
		if(is_scalar($scheme) === true
		&& preg_match($this->_regexp['scheme'],$scheme) === 1)
			return(true);

		return(false);
	}

	function valid_ipv4($ip)
	{
		return((xivo_ip2long($ip) !== false));
	}

	function valid_ipv6($ip)
	{
		if(is_scalar($ip) === false)
			return(false);

		if(($pos = strpos($ip,'::')) === false)
			return(($this->_valid_rightipv6($ip) === 8));
		else if(substr_count($ip,'::') !== 1
		|| ($right = $this->_valid_rightipv6(substr($ip,$pos + 2))) === false
		|| ($left = $this->_valid_leftipv6(substr($ip,0,$pos))) === false)
			return(false);

		return(($right + $left < 8));
	}

	function _valid_h16($h16)
	{
		$h16 = (string) $h16;

		if((($b16 = base_convert($h16,16,10)) !== '0'
		   || ctype_digit($h16) === true) === true
		&& $b16 >= 0 && $b16 <= 65535)
			return(true);

		return(false);
	}

	function _valid_rightipv6($ip)
	{
		if(((string) $ip) === '')
			return(0);

		$arr = explode(':',$ip,9);
		$nb = count($arr);

		if($nb > 8 || ($nb > 7 && strpos($ip,'.') !== false) === true
		|| ($nb > 1 && ctype_xdigit(implode(array_slice($arr,0,-1))) === false) === true)
			return(false);

		$h16_cnt = 0;

		if(strpos($arr[$nb-1],'.') !== false)
		{
			if($this->valid_ipv4($arr[$nb-1]) === false)
				return(false);

			$h16_cnt = 2;

			if($nb === 1)
				return($h16_cnt);

			$nb--;
			$arr = array_slice($arr,0,-1);
		}

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_valid_h16($arr[$i]) === false)
				return(false);
		}

		return($h16_cnt + $nb);
	}

	function _valid_leftipv6($ip)
	{
		if(((string) $ip) === '')
			return(0);

		$arr = explode(':',$ip,8);
		$nb = count($arr);

		if($nb > 7
		|| ($nb > 1 && ctype_xdigit(implode(array_slice($arr,0,-1))) === false) === true)
			return(false);

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_valid_h16($arr[$i]) === false)
				return(false);
		}

		return($nb);
	}

	function valid_ipvfuture($ip)
	{
		if(is_scalar($ip) === true
		&& preg_match($this->_regexp['ipvfuture'],$ip) === 1)
			return(true);

		return(false);
	}

	function valid_ipliteral($ip)
	{
		if(is_scalar($ip) === false)
			return(false);

		$ip = (string) $ip;

		if(($len = strlen($ip)) < 2 || $ip{0} !== '[' || $ip{$len - 1} !== ']')
			return(false);

		$ip = substr($ip,1,-1);

		if($this->valid_ipv6($ip) === true || $this->valid_ipvfuture($ip) === true)
			return(true);

		return(false);
	}

	function valid_host($host)
	{
		switch($this->get_host_type($host))
		{
			case $this->_host['reg_name']:
				if(is_scalar($host) === true
				&& preg_match($this->_regexp['host'],$host) === 1)
					return(true);
				break;
			case $this->_host['ipliteral']:
				if($this->valid_ipliteral($host) === true)
					return(true);
				break;
			default:
				return(true);
		}

		return(false);
	}

	function valid_authority($host='',$user='',$passwd='',$port='')
	{
		if(is_scalar($host) === false
		|| is_scalar($user) === false
		|| is_scalar($passwd) === false
		|| is_scalar($port) === false)
			return(false);

		$host	= (string) $host;
		$user	= (string) $user;
		$passwd = (string) $passwd;
		$port	= (string) $port;

		if(($host !== ''
		   && $this->valid_host($host) === false) === true
		|| ($user !== ''
		   && $this->valid_user($user) === false) === true
		|| ($passwd !== ''
		   && $this->valid_passwd($passwd) === false) === true
		|| ($port !== ''
		   && $this->valid_port($port) === false) === true
		|| ($host === ''
		   && $user === ''
		   && $passwd === ''
		   && $port === '') === true)
			return(false);

		return(true);
	}

	function valid_user($user)
	{
		if(is_scalar($user) === true
		&& preg_match($this->_regexp['user'],$user) === 1)
			return(true);

		return(false);
	}

	function valid_passwd($passwd)
	{
		if(is_scalar($passwd) === true
		&& preg_match($this->_regexp['passwd'],$passwd) === 1)
			return(true);

		return(false);
	}

	function valid_port($port)
	{
		if(is_scalar($port) === true
		&& preg_match($this->_regexp['port'],$port) === 1)
			return(true);

		return(false);
	}

	function valid_path($path)
	{
		if(is_scalar($path) === true
		&& preg_match($this->_regexp['path'],$path) === 1)
			return(true);

		return(false);
	}

	function valid_query($query)
	{
		if(is_scalar($query) === true
		&& preg_match($this->_regexp['query'],$query) === 1)
			return(true);

		return(false);
	}

	function valid_fragment($fragment)
	{
		return($this->valid_query($fragment));
	}
}

?>
