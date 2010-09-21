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

require_once(dwho_file::joinpath(DWHO_PATH_ROOT,'network.inc'));

class dwho_asterisk_uri extends dwho_uri
{
	function dwho_asterisk_uri()
	{
		$this->dwho_uri();

		$sub_delims	= '\!\$&\'\(\)\*\+,;=';
		$unreserved	= 'a-zA-Z0-9-\._~';
		$this->_regexp['user']		= '/^['.$unreserved.$sub_delims.'@+%]+$/';
	}

	function build_uri($arr,$encode=true,$type_host=null,$query_sep=null)
	{
		$encode = (bool) $encode;
		$scheme = $authority = $userinfo = $hostport = $path = $query = $fragment = '';

		if(is_array($arr) === false)
			return(false);
		else if(dwho_has_len($arr,'scheme') === true)
		{
			if($this->valid_scheme($arr['scheme']) === false)
				return(false);

			$scheme = $arr['scheme'];
		}

		if(dwho_issa('authority',$arr) === true)
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

		if(dwho_has_len($arr,'host') === true)
		{
			$arr['host'] = (string) $arr['host'];

			switch((empty($type_host) === true ? $this->get_host_type($arr['host']) : $type_host))
			{
				case $this->_host['ipliteral']:
					if($this->valid_ipliteral($arr['host']) === false)
						return(false);
					break;
				case $this->_host['ipv4']:
					if(dwho_network::valid_ipv4($arr['host']) === false)
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

		if(dwho_has_len($arr,'port') === true)
		{
			if(dwho_network::valid_port_number($arr['port']) === false)
				return(false);

			$hostport .= ':'.$arr['port'];
		}

		if(dwho_has_len($arr,'user') === true)
		{
			$userinfo = (string) $arr['user'];
		}

		if(dwho_has_len($arr,'passwd') === true)
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

		if(dwho_has_len($arr,'path') === true)
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

		if(dwho_issa('query',$arr) === true)
			$query = $this->build_query($arr['query'],$encode,$query_sep);

		if(dwho_has_len($arr,'fragment') === true)
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

	function parse_uri($uri,$decode=true,$query_sep=null)
	{
		$decode = (bool) $decode;

		if(($uri = $this->uri_split($uri)) === false)
			return(false);
		else if(isset($uri['authority']) === true
		&& ($uri['authority'] = $this->split_authority($uri['authority'])) === false)
			return(false);
		else if(isset($uri['query']) === true
		&& ($uri['query'] = $this->split_query($uri['query'],$query_sep)) === false)
			return(false);
		else if(isset($uri['scheme']) === true && $this->valid_scheme($uri['scheme']) === false)
			return(false);

		if(dwho_issa('authority',$uri) === true)
		{
			$authority = &$uri['authority'];

			if(isset($authority['user']) === true)
			{
				if($this->valid_user($authority['user']) === false)
					return(false);
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
				else if($decode === true
				&& $this->get_host_type($authority['host']) === $this->_host['reg_name'])
					$authority['host'] = urldecode($authority['host']);
			}

			if(isset($authority['port']) === true && dwho_network::valid_port_number($authority['port']) === false)
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

		if(dwho_issa('query',$uri) === true && ($nb = count($uri['query'])) !== 0)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$ref = &$uri['query'][$i];

				if(dwho_has_len($ref,0) === true)
				{
					if($this->valid_query($ref[0]) === false)
						return(false);
					else if($decode === true)
						$ref[0] = urldecode($ref[0]);
				}

				if(dwho_has_len($ref,1) === false)
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

	function split_authority($authority)
	{
		$authority = (string) $authority;

		if($authority === '')
			return(null);

		$r = array();

		if(($pos = strrpos($authority,'@')) === false)
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
}

?>
