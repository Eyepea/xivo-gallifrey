<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

define('DWHO_NETWORK_RE_HOSTNAME','[a-zA-Z](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?');
define('DWHO_NETWORK_RE_IPVFUTURE','v[\da-fA-F]+\.[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=:]+');

define('DWHO_NETWORK_HOST_IPV4_DOTDEC',1);
define('DWHO_NETWORK_HOST_IPV4',2);
define('DWHO_NETWORK_HOST_HOSTNAME',4);
define('DWHO_NETWORK_HOST_HOSTNAME_TLD',8);
define('DWHO_NETWORK_HOST_IPV6',16);
define('DWHO_NETWORK_HOST_IPVFUTURE',65536);
define('DWHO_NETWORK_HOST_IP_ALL',(DWHO_NETWORK_HOST_IPV4 |
				   DWHO_NETWORK_HOST_IPV6 |
				   DWHO_NETWORK_HOST_IPVFUTURE));
define('DWHO_NETWORK_HOST_HOSTNAME_ALL',(DWHO_NETWORK_HOST_HOSTNAME |
					 DWHO_NETWORK_HOST_HOSTNAME_TLD));
define('DWHO_NETWORK_HOST_ALL',(DWHO_NETWORK_HOST_IPV4_DOTDEC |
				DWHO_NETWORK_HOST_IPV4 |
				DWHO_NETWORK_HOST_HOSTNAME |
				DWHO_NETWORK_HOST_HOSTNAME_TLD |
				DWHO_NETWORK_HOST_IPV6 |
				DWHO_NETWORK_HOST_IPVFUTURE));

define('DWHO_NETWORK_EMAIL_HOST_ALL',(DWHO_NETWORK_HOST_IPV4_DOTDEC |
				      DWHO_NETWORK_HOST_IPV6 |
				      DWHO_NETWORK_HOST_HOSTNAME |
				      DWHO_NETWORK_HOST_HOSTNAME_TLD));

define('DWHO_NETWORK_MASK_IPV4',1);
define('DWHO_NETWORK_MASK_IPV4_BIT',2);
define('DWHO_NETWORK_MASK_IPV4_DOTDEC',4);
define('DWHO_NETWORK_MASK_IPV4_ALL',(DWHO_NETWORK_MASK_IPV4 |
				     DWHO_NETWORK_MASK_IPV4_BIT |
				     DWHO_NETWORK_MASK_IPV4_DOTDEC));

class dwho_network
{
	function ipv4_long($ip)
	{
		if(is_scalar($ip) === false)
			return(false);
		else if(($long = ip2long($ip)) === -1
		&& $ip !== 0xFFFFFFFF
		&& strcasecmp($ip,'0xFF.0xFF.0xFF.0xFF') !== 0
		&& $ip !== '255.255.255.255')
			return(false);

		return($long);
	}

	function normalize_ipv4_dotdec($ip)
	{
		if(($long = dwho_network::ipv4_long($ip)) === false)
			return(false);

		return(long2ip($long));
	}

	function valid_ipv4($ip)
	{
		return((dwho_network::ipv4_long($ip) !== false));
	}

	function valid_ipv4_dotdec($ip)
	{
		if(($ipv4_dotdec = dwho_network::normalize_ipv4_dotdec($ip)) === false
		|| (string) $ip !== $ipv4_dotdec)
			return(false);

		return(true);
	}

	function valid_bitmask_ipv4($bit)
	{
		if(dwho_is_uint($bit) === false
		|| $bit > 32)
			return(false);

		return(true);
	}

	function valid_bitmask_ipv6($bit)
	{
		if(dwho_is_uint($bit) === false
		|| $bit > 128)
			return(false);

		return(true);
	}

	function netmask_ipv4_to_bitmask($nm)
	{
		if(($nm = dwho_network::ipv4_long($nm)) === 0)
			return(0);
		else if($nm === -1)
			return(32);
		else if($nm === false
		|| ($inv = ($nm ^ 0xFFFFFFFF)) === 0
		|| ($inv & ($inv + 1)) !== 0)
			return(false);

		return(32 - intval(log($inv + 1,2)));
	}

	function bitmask_to_netmask_ipv4($bit)
	{
		if(dwho_network::valid_bitmask_ipv4($bit) === false)
			return(false);

		return(long2ip((0xFFFFFFFF >> (32 - $bit)) << (32 - $bit)));
	}

	function bitmask_to_netmask_ipv6($bit)
	{
		if(dwho_network::valid_bitmask_ipv6($bit) === false)
			return(false);

		$r = array();

		$nb = (int) floor(floatval($bit) / 16);

		if($nb > 0)
		{
			$r = array_fill(0,$nb,'ffff');
			$bit -= (16 * $nb);
		}

		if($bit > 0)
			$r[] = bin2hex(pack('n',(0xFFFF >> (16 - $bit)) << (16 - $bit)));

		return(implode(':',array_pad($r,8,'0000')));
	}

	function valid_mask_ipv4($nm,$mode=null,$ret=false)
	{
		$mode = intval($mode);

		if(($mode & DWHO_NETWORK_MASK_IPV4_ALL) === 0)
			$mode = DWHO_NETWORK_MASK_IPV4_ALL;

		if(($mode & DWHO_NETWORK_MASK_IPV4) !== 0
		&& ($bit = dwho_network::netmask_ipv4_to_bitmask($nm)) !== false)
			return((bool) $ret === false ? true : $bit);

		if(($mode & DWHO_NETWORK_MASK_IPV4_BIT) !== 0
		&& dwho_network::valid_bitmask_ipv4($nm) === true)
			return((bool) $ret === false ? true : $nm);

		if(($mode & DWHO_NETWORK_MASK_IPV4_DOTDEC) !== 0
		&& dwho_network::valid_ipv4_dotdec($nm) !== false
		&& ($bit = dwho_network::netmask_ipv4_to_bitmask($nm)) !== false)
			return((bool) $ret === false ? true : $bit);

		return(false);
	}

	function valid_subnet_ipv4($host,$mask=null,$mask_mode=null,$host_mode=null,$ret=false)
	{
		if(is_scalar($host) === false)
			return(false);

		$host = (string) $host;

		if($mask === null)
		{
			if(($pos = strrpos($host,'/')) === false)
				return(false);

			$mask = substr($host,$pos + 1);
			$host = substr($host,0,$pos);
		}

		if(($host_mode & DWHO_NETWORK_HOST_ALL) === 0)
			$host_mode = DWHO_NETWORK_HOST_IPV4;

		if(($bitmask = dwho_network::valid_mask_ipv4($mask,$mask_mode,$ret)) === false
		|| dwho_network::valid_host($host,$host_mode) === false)
			return(false);

		return($bitmask);
	}

	function ipv4_calc($ip,$mask)
	{
		if(dwho_network::valid_ipv4($ip) === false
		|| ($bit = dwho_network::valid_subnet_ipv4($ip,
							    $bit,
							    DWHO_NETWORK_MASK_IPV4_ALL,
							    DWHO_NETWORK_HOST_IPV4,
							    true)) === false)
			return(false);

		$decmask	= (0xFFFFFFFF >> (32 - $bit)) << (32 - $bit);
		$wildcard	=~ $decmask;
		$network	= ip2long($ip) & $decmask;
		$broadcast	= $network | $wildcard;

		$r = array();
		$r['ip']	= $ip;
		$r['mask']	= $bit;
		$r['netmask']	= long2ip($decmask);
		$r['wildcard']	= long2ip($wildcard);
		$r['network']	= long2ip($network);
		$r['broadcast']	= long2ip($broadcast);

		return($r);
	}

	function ipv4_in_subnet($ip,$sub)
	{
		if(is_string($sub) === false || ($pos = strrpos($sub,'/')) === false)
			return(false);

		$ipsub = substr($sub,0,$pos);
		$mask = substr($sub,$pos + 1);

		if(dwho_network::valid_ipv4($ip) === false
		|| ($bit = dwho_network::valid_subnet_ipv4($ipsub,
							   $mask,
							   DWHO_NETWORK_MASK_IPV4_ALL,
							   DWHO_NETWORK_HOST_IPV4,
							   true)) === false)
			return(false);

		$decip		= ip2long($ip);
		$decmask	= (0xFFFFFFFF >> (32 - $bit)) << (32 - $bit);
		$wildcard	=~ $decmask;
		$network	= ip2long($ipsub) & $decmask;
		$broadcast	= $network | $wildcard;

		return(($decip > $network && $decip < $broadcast));
	}

	function ipv4_in_lhost($ip,$host)
	{
		if(is_scalar($ip) === false || is_scalar($host) === false)
			return(false);

		$ip = (string) $ip;
		$host = (string) $host;

		if(($ipv4_dotdec = dwho_network::normalize_ipv4_dotdec($ip)) !== false)
			$ip = $ipv4_dotdec;
		else
		{
			$ip = gethostbyname($ip);

			if(dwho_network::valid_ipv4($ip) === false)
				return(false);
		}

		if(dwho_network::valid_ipv4($host) === true
		|| dwho_network::valid_subnet_ipv4($host) === true)
			$lhost = (array) $host;
		else if(($lhost = gethostbynamel($host)) === false)
			return(false);

		$nb = count($lhost);

		for($i = 0;$i < $nb;$i++)
		{
			if(strcasecmp($ip,$lhost[$i]) === 0
			|| dwho_network::ipv4_in_subnet($ip,$lhost[$i]) === true)
				return(true);
		}

		return(false);
	}

	function valid_ipv6($ip)
	{
		if(is_scalar($ip) === false)
			return(false);

		if(($pos = strpos($ip,'::')) === false)
			return((dwho_network::valid_ipv6_right($ip) === 8));
		else if(substr_count($ip,'::') !== 1
		|| ($right = dwho_network::valid_ipv6_right(substr($ip,$pos + 2))) === false
		|| ($left = dwho_network::valid_ipv6_left(substr($ip,0,$pos))) === false)
			return(false);

		return(($right + $left < 8));
	}

	function valid_ipv6_h16($h16)
	{
		if(is_scalar($h16) === false)
			return(false);

		if((($b16 = base_convert($h16,16,10)) !== '0'
		   || ctype_digit($h16) === true) === true
		&& $b16 >= 0 && $b16 <= 65535)
			return(true);

		return(false);
	}

	function valid_ipv6_left($ip)
	{
		if(is_scalar($ip) === false)
			return(false);
		else if(($ip = (string) $ip) === '')
			return(0);

		$arr = explode(':',$ip,8);
		$nb = count($arr);

		if($nb > 7
		|| ($nb > 1 && ctype_xdigit(implode(array_slice($arr,0,-1))) === false) === true)
			return(false);

		for($i = 0;$i < $nb;$i++)
		{
			if(dwho_network::valid_ipv6_h16($arr[$i]) === false)
				return(false);
		}

		return($nb);
	}

	function valid_ipv6_right($ip)
	{
		if(is_scalar($ip) === false)
			return(false);
		else if(($ip = (string) $ip) === '')
			return(0);

		$arr = explode(':',$ip,9);
		$nb = count($arr);

		if($nb > 8
		|| ($nb > 7 && strpos($ip,'.') !== false) === true
		|| ($nb > 1 && ctype_xdigit(implode(array_slice($arr,0,-1))) === false) === true)
			return(false);

		$h16_cnt = 0;

		if(strpos($arr[$nb-1],'.') !== false)
		{
			if(dwho_network::valid_ipv4($arr[$nb-1]) === false)
				return(false);

			$h16_cnt = 2;

			if($nb === 1)
				return($h16_cnt);

			$nb--;
			$arr = array_slice($arr,0,-1);
		}

		for($i = 0;$i < $nb;$i++)
		{
			if(dwho_network::valid_ipv6_h16($arr[$i]) === false)
				return(false);
		}

		return($h16_cnt + $nb);
	}

	function valid_ipvfuture($ip)
	{
		if(is_scalar($ip) === true
		&& preg_match('/^'.DWHO_NETWORK_RE_IPVFUTURE.'$/',$ip) === 1)
			return(true);

		return(false);
	}

	function valid_hostname_part($hostpart)
	{
		if(is_string($hostpart) === true
		&& preg_match('/^('.DWHO_NETWORK_RE_HOSTNAME.')$/',$hostpart,$match) === 1)
			return(true);

		return(false);
	}

	function valid_hostname($host)
	{
		if(is_string($host) === true
		&& strlen($host) < 256
		&& preg_match('/^(?:'.DWHO_NETWORK_RE_HOSTNAME.'\.)*'.
				'('.DWHO_NETWORK_RE_HOSTNAME.')$/',$host,$match) === 1)
			return(true);

		return(false);
	}

	function valid_hostname_tld($host)
	{
		if(is_string($host) === true
		&& strlen($host) < 256
		&& preg_match('/^(?:'.DWHO_NETWORK_RE_HOSTNAME.'\.)+'.
				'('.DWHO_NETWORK_RE_HOSTNAME.')$/',$host,$match) === 1)
			return(true);

		return(false);
	}

	function valid_hostname_list($list,$mode=null,$max=6,$sep=null)
	{
		if(is_scalar($list) === false)
			return(false);

		$mode = intval($mode);

		if(($mode & (DWHO_NETWORK_HOST_HOSTNAME | DWHO_NETWORK_HOST_HOSTNAME_TLD)) === 0)
			$mode = (DWHO_NETWORK_HOST_HOSTNAME | DWHO_NETWORK_HOST_HOSTNAME_TLD);

		$max = dwho_uint($max,6);

		if(dwho_has_len($sep) === false)
			$sep = ' ';

		$hosts = explode($sep,$list,$max + 1);

		if(($nb = count($hosts)) > $max)
			return(false);

		for($i = 0;$i < $nb;$i++)
		{
			if(dwho_network::valid_host($hosts[$i],$mode) === false)
				return(false);
		}

		return(true);
	}

	function valid_host($host,$mode=null)
	{
		$mode = intval($mode);

		if(($mode & DWHO_NETWORK_HOST_ALL) === 0)
			$mode = (DWHO_NETWORK_HOST_IPV4 | DWHO_NETWORK_HOST_HOSTNAME);

		if(($mode & DWHO_NETWORK_HOST_IPV4) !== 0
		&& dwho_network::valid_ipv4($host) === true)
			return(true);

		if(($mode & DWHO_NETWORK_HOST_IPV4_DOTDEC) !== 0
		&& dwho_network::valid_ipv4_dotdec($host) === true)
			return(true);

		if(($mode & DWHO_NETWORK_HOST_HOSTNAME) !== 0
		&& dwho_network::valid_hostname($host) === true)
			return(true);

		if(($mode & DWHO_NETWORK_HOST_HOSTNAME_TLD) !== 0
		&& dwho_network::valid_hostname_tld($host) === true)
			return(true);

		if(($mode & DWHO_NETWORK_HOST_IPV6) !== 0
		&& dwho_network::valid_ipv6($host) === true)
			return(true);

		if(($mode & DWHO_NETWORK_HOST_IPVFUTURE) !== 0
		&& dwho_network::valid_ipvfuture($host) === true)
			return(true);

		return(false);
	}

	function valid_host_subnet_ipv4($host,$host_mode=null,$mask_mode=null,$subnet_host_mode=null)
	{
		if(dwho_network::valid_host($host,$host_mode) === true
		|| dwho_network::valid_subnet_ipv4($host,null,$mask_mode,$subnet_host_mode) === true)
			return(true);

		return(false);
	}

	function valid_port_number($port)
	{
		if(is_scalar($port) === false
		|| ctype_digit((string) $port) === false
		|| (int) $port < 1
		|| (int) $port > 65535)
			return(false);

		return(true);
	}
	function normalize_macaddr($macaddr)
	{
		if(is_scalar($macaddr) === false
		|| preg_match_all('/([A-F0-9]{1,2})[-: ]?/i',$macaddr,$match) !== 6)
			return(false);

		$r = '';

		for($i = 0;$i < 6;$i++)
			$r .= sprintf(':%02X',base_convert($match[1][$i],16,10));

		return(substr($r,1));
	}

	function valid_macaddr($macaddr)
	{
		if(is_string($macaddr) === false
		|| preg_match('/^([A-F0-9]{2}:){5}([A-F0-9]{2})$/i',$macaddr,$match) !== 1
		|| $macaddr === '00:00:00:00:00:00')
			return(false);

		return(true);
	}

	function valid_email_localpart($localpart)
	{
		$atom		= '\!#\$%&\'\*\+\-\/0-9\=\?A-Z\^_`a-z\{\|\}~';
		$qtext		= '\x20\x21\x23-\x5B\x5D-\x7E';
		$quotedpair	= '\(\)\<\>\[\]\:;@\\\,\."';

		if(is_scalar($localpart) === false
		|| strlen($localpart) > 64
		|| preg_match('/^(?:['.$atom.']+(?:\.['.$atom.']+)*|'.
				 '"(?:['.$qtext.']|'.
				     '\\\['.$quotedpair.'])+")$/',
			      $localpart) !== 1)
			return(false);

		return(true);
	}

	function valid_email_address_literal($address,$host_mode=null,$chkmx=false)
	{
		if(is_string($address) === false
		|| isset($address{0}) === false)
			return(false);

		$host_mode	= intval($host_mode);
		$chkmx		= (bool) $chkmx;
		$mxhosts	= array();

		if(($host_mode & DWHO_NETWORK_EMAIL_HOST_ALL) === 0)
			$host_mode = DWHO_NETWORK_EMAIL_HOST_ALL;

		if($address{0} === '[' && substr($address,-1) === ']')
		{
			if(strncmp('[IPv6:',$address,6) === 0)
			{
				if(($host_mode & DWHO_NETWORK_HOST_IPV6) === 0)
					return(false);

				$address = substr($address,6,-1);

				if(dwho_network::valid_ipv6($address) === false)
					return(false);

				return(($chkmx === false ? true : getmxrr($address,$mxhosts)));
			}

			if(($host_mode & DWHO_NETWORK_HOST_IPV4_DOTDEC) === 0)
				return(false);

			$address = substr($address,1,-1);

			if(dwho_network::valid_ipv4_dotdec($address) === false)
				return(false);

			return(($chkmx === false ? true : getmxrr($address,$mxhosts)));
		}

		$mode = 0;

		if(($host_mode & DWHO_NETWORK_HOST_HOSTNAME) !== 0)
			$mode |= DWHO_NETWORK_HOST_HOSTNAME;

		if(($host_mode & DWHO_NETWORK_HOST_HOSTNAME_TLD) !== 0)
			$mode |= DWHO_NETWORK_HOST_HOSTNAME_TLD;

		if($mode === 0 || dwho_network::valid_host($address,$mode) === false)
			return(false);

		return(($chkmx === false ? true : getmxrr($address,$mxhosts)));
	}

	function valid_email($email,$host_mode=null,$chkmx=false)
	{
		if(is_string($email) === false
		|| strlen($email) > 320
		|| ($pos = strrpos($email,'@')) === false
		|| $pos === 0)
			return(false);

		$localpart	= substr($email,0,$pos);
		$address	= substr($email,$pos + 1);

		if(dwho_network::valid_email_localpart($localpart) === false
		|| dwho_network::valid_email_address_literal($address,$host_mode,$chkmx) === false)
			return(false);

		return(true);
	}
}

?>
