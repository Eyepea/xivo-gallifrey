<?php

#
# XiVO Web-Interface
# Copyright (C) 2009  Proformatique <technique@proformatique.com>
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

define('DWHO_SYSINFO_SYS_CLASS_NET_PATH',dwho_file::joinpath(DWHO_SEP_DIR.'sys','class','net'));

class dwho_sysinfo_linux_net
{
	function netstats()
	{
		if(($r = dwho_sysinfo_linux_net::netstats_interface()) === false)
			return(false);

		reset($r);

		while(list($iface) = each($r))
		{
			$r[$iface]['interface']	= $iface;
			$r[$iface]['hwtype']	= dwho_sysinfo_linux_net::netinfo_hwtype($iface);
			$r[$iface]['hwtypeid']	= dwho_sysinfo_linux_net::netinfo_hwtypeid($iface);
			$r[$iface]['hwaddress']	= dwho_sysinfo_linux_net::netinfo_hwaddress($iface);
			$r[$iface]['carrier']	= dwho_sysinfo_linux_net::netinfo_carrier($iface);
			$r[$iface]['flags']	= dwho_sysinfo_linux_net::netinfo_iff($iface);
			$r[$iface]['mtu']	= dwho_sysinfo_linux_net::netinfo_mtu($iface);
			$r[$iface]['physical']	= dwho_sysinfo_linux_net::netinfo_interface_is_physical($iface);
		}

		return($r);
	}

	function netinfo()
	{
		if(($list = dwho_sysinfo_linux_net::netinfo_interface()) === false)
			return(false);

		$r = array();

		foreach($list as $iface)
		{
			$r[$iface] = array();
			$r[$iface]['interface']	= $iface;
			$r[$iface]['hwtype']	= dwho_sysinfo_linux_net::netinfo_hwtype($iface);
			$r[$iface]['hwtypeid']	= dwho_sysinfo_linux_net::netinfo_hwtypeid($iface);
			$r[$iface]['hwaddress']	= dwho_sysinfo_linux_net::netinfo_hwaddress($iface);
			$r[$iface]['carrier']	= dwho_sysinfo_linux_net::netinfo_carrier($iface);
			$r[$iface]['flags']	= dwho_sysinfo_linux_net::netinfo_iff($iface);
			$r[$iface]['mtu']	= dwho_sysinfo_linux_net::netinfo_mtu($iface);
			$r[$iface]['physical']	= dwho_sysinfo_linux_net::netinfo_interface_is_physical($iface);
		}

		return($r);
	}

	function netinfo_interface()
	{
		if(($list = dwho_sysinfo_linux_net::netinfo_interface_from_sys()) !== false)
			return($list);

		return(dwho_sysinfo_linux_net::netinfo_interface_from_proc());
	}

	function netinfo_interface_exists($iface)
	{
		$iface = basename($iface);

		if(substr($iface,0,1) === '.')
			return(false);

		$path = dwho_file::joinpath(DWHO_SYSINFO_SYS_CLASS_NET_PATH,$iface);

		if((bool) dwho_file::is_d($path) === true)
			return(true);
		else if(($list = dwho_sysinfo_linux_net::netinfo_interface_from_proc()) !== false)
			return(in_array($iface,$list));

		return(false);
	}

	function netinfo_interface_is_physical($iface)
	{
		$iface = basename($iface);

		if(substr($iface,0,1) === '.')
			return(false);

		$path = dwho_file::joinpath(DWHO_SYSINFO_SYS_CLASS_NET_PATH,$iface);

		if((bool) dwho_file::is_d($path) === true)
			return((bool) dwho_file::is_d(dwho_file::joinpath($path,'device')));

		return(null);
	}

	function netinfo_interface_realname($iface)
	{
		if(($pos = strpos($iface,':')) > 0)
			return(substr($iface,0,$pos));

		return($iface);
	}

	function netinfo_interface_is_virtual($iface)
	{
		return((strpos($iface,':') > 0));
	}

	function netstats_interface()
	{
		if(($stats = dwho_sysinfo_linux_net::netstats_interface_from_sys()) !== false)
			return($stats);

		return(dwho_sysinfo_linux_net::netstats_interface_from_proc());
	}

	function netinfo_interface_from_sys()
	{
		return(dwho_file::read_d(DWHO_SYSINFO_SYS_CLASS_NET_PATH,
					 'dir',
					 FILE_R_OK,
					 '/^[^\.]/'));
	}

	function netstats_interface_from_sys()
	{
		if(($ifaces = dwho_sysinfo_linux_net::netinfo_interface_from_sys()) === false
		|| ($nb = count($ifaces)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$path = dwho_file::joinpath(DWHO_SYSINFO_SYS_CLASS_NET_PATH,
						    $ifaces[$i],
						    'statistics');

			$r[$ifaces[$i]] = array(
						'statistics' => array('rx' => array(),
								      'tx' => array()));

			$ref = &$r[$ifaces[$i]]['statistics'];

			while(list($key) = each($ref))
			{
				$bytes_file = dwho_file::joinpath($path,$key.'_bytes');

				if(($bytes_file = dwho_file::is_f_r($bytes_file)) === false
				|| ($ref[$key]['bytes'] = trim(file_get_contents($bytes_file))) === '')
					return(false);

				$packets_file = dwho_file::joinpath($path,$key.'_packets');

				if(($packets_file = dwho_file::is_f_r($packets_file)) === false
				|| ($ref[$key]['packets'] = trim(file_get_contents($packets_file))) === '')
					return(false);

				$errs_file = dwho_file::joinpath($path,$key.'_errors');

				if(($errs_file = dwho_file::is_f_r($errs_file)) === false
				|| ($ref[$key]['errs'] = trim(file_get_contents($errs_file))) === '')
					return(false);

				$drop_file = dwho_file::joinpath($path,$key.'_dropped');

				if(($drop_file = dwho_file::is_f_r($drop_file)) === false
				|| ($ref[$key]['drop'] = trim(file_get_contents($drop_file))) === '')
					return(false);
			}

			$ref['total']['bytes'] = $ref['rx']['bytes'] + $ref['tx']['bytes'];
			$ref['total']['packets'] = $ref['rx']['packets'] + $ref['tx']['packets'];
			$ref['total']['errs'] = $ref['rx']['errs'] + $ref['tx']['errs'];
			$ref['total']['drop'] = $ref['rx']['drop'] + $ref['tx']['drop'];
		}

		return($r);
	}

	function netstats_interface_from_proc($stats=true)
	{
		$stats = (bool) $stats;

		$path = dwho_file::joinpath(DWHO_SEP_DIR.'proc','net','dev');

		if(dwho_file::is_f_r($path) === false
		|| ($content = file($path)) === false
		|| ($nb = count($content)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$line = trim($content[$i]);

			if(($pos = strpos($line,':')) === false)
				continue;

			$data = preg_split('/\s+/',$line);
			$name = substr($data[0],0,$pos);
			$data[0] = substr($data[0],$pos + 1);

			if(dwho_has_len($data[0]) === false)
				array_shift($data);

			if($stats === true)
			{
				$r[$name] = array(
					'statistics'	=> array(
							'rx'	=> array(
									'bytes'		=> $data[0],
									'packets'	=> $data[1],
									'errs'		=> $data[2],
									'drop'		=> $data[3]),
							'tx'	=> array(
									'bytes'		=> $data[8],
									'packets'	=> $data[9],
									'errs'		=> $data[10],
									'drop'		=> $data[11]),
							'total'	=> array(
									'bytes'		=> ($data[0] + $data[8]),
									'packets'	=> ($data[1] + $data[9]),
									'errs'		=> ($data[2] + $data[10]),
									'drop'		=> ($data[3] + $data[11]))));
			}
			else
				$r[] = $name;
		}

		return($r);
	}

	function netinfo_interface_from_proc()
	{
		return(dwho_sysinfo_linux_net::netstats_interface_from_proc(false));
	}

	function netinfo_hwtype($interface,$retid=false)
	{
		$arphrd = &dwho_gat::load_get('arphrd',
					      dwho_file::joinpath(DWHO_PATH_ROOT,'sysinfo','linux','net'),
					      'dwho_sysinfo_linux_net_arphrd');

		$path = dwho_file::joinpath(DWHO_SYSINFO_SYS_CLASS_NET_PATH,
					    basename($interface),
					    'type');

		if(dwho_file::is_f_r($path) === false
		|| ($content = trim(file_get_contents($path))) === ''
		|| isset($arphrd[$content]) === false)
			return((bool) $retid === false ? 'none' : 0xFFFE);

		return((bool) $retid === false ? $arphrd[$content] : (int) $content);
	}

	function netinfo_hwtypeid($interface)
	{
		return(dwho_sysinfo_linux_net::netinfo_hwtype($interface,true));
	}

	function netinfo_hwtype_id2name($id)
	{
		$arphrd = &dwho_gat::load_get('arphrd',
					      dwho_file::joinpath(DWHO_PATH_ROOT,'sysinfo','linux','net'),
					      'dwho_sysinfo_linux_net_arphrd');

		if(isset($arphrd[$id]) === true)
			return($arphrd[$id]);

		return(false);
	}

	function netinfo_hwtype_name2id($name)
	{
		$arphrd = &dwho_gat::load_get('arphrd',
					      dwho_file::joinpath(DWHO_PATH_ROOT,'sysinfo','linux','net'),
					      'dwho_sysinfo_linux_net_arphrd');

		return(array_search($name,$arphrd));
	}

	function netinfo_hwaddress($interface)
	{
		$path = dwho_file::joinpath(DWHO_SYSINFO_SYS_CLASS_NET_PATH,
					    basename($interface),
					    'address');

		if(dwho_file::is_f_r($path) === false
		|| ($content = file_get_contents($path)) === false)
			return(false);

		return(trim($content));
	}

	function netinfo_carrier($interface)
	{
		$path = dwho_file::joinpath(DWHO_SYSINFO_SYS_CLASS_NET_PATH,
					    basename($interface),
					    'carrier');

		if(dwho_file::is_f_r($path) === false
		|| ($content = trim(file_get_contents($path))) === '')
			return(null);

		return((bool) $content{0});
	}

	function netinfo_mtu($interface)
	{
		$path = dwho_file::joinpath(DWHO_SYSINFO_SYS_CLASS_NET_PATH,
					    basename($interface),
					    'mtu');

		if(dwho_file::is_f_r($path) === false
		|| ($content = file_get_contents($path)) === false)
			return(false);

		return(trim($content));
	}

	function netinfo_iff($interface)
	{
		$flags = &dwho_gat::load_get('iff',
					     dwho_file::joinpath(DWHO_PATH_ROOT,'sysinfo','linux','net'),
					     'dwho_sysinfo_linux_net_iff');

		$path = dwho_file::joinpath(DWHO_SYSINFO_SYS_CLASS_NET_PATH,
					    basename($interface),
					    'flags');

		if(dwho_file::is_f_r($path) === false
		|| ($content = trim(file_get_contents($path))) === '')
			return(dwho_array_combine(array_keys($flags),
						  array_fill(0,count($flags),null)));

		$r = array();

		$flag = hexdec($content);

		foreach($flags as $name => $bit)
			$r[$name] = (bool) ($flag & $bit);

		return($r);
	}
}

?>
