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

class dwho_sysinfo_linux
{
	function net()
	{
		if(dwho::load_class('dwho::sysinfo::linux::net') === false)
			trigger_error('Failed to load SysInfo Net class for Linux',E_USER_ERROR);

		$name = 'dwho_sysinfo_linux_net';

		return(new $name());
	}

	function cpustats()
	{
		$path = dwho_file::joinpath(DWHO_SEP_DIR.'proc','cpuinfo');

		if(dwho_file::is_f_r($path) === false
		|| ($content = file($path)) === false
		|| ($nb = count($content)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(preg_match('/^([^:]+):\s+(.+)$/',$content[$i],$match) === 1)
				$r[str_replace(' ','_',trim(strtolower($match[1])))] = trim($match[2]);
		}

		if(empty($r) === true)
			return(null);

		return($r);
	}

	function memstats($byte=false)
	{
		$path = dwho_file::joinpath(DWHO_SEP_DIR.'proc','meminfo');

		if(dwho_file::is_f_r($path) === false
		|| ($content = file($path)) === false
		|| ($nb = count($content)) === 0)
			return(false);

		$byte = (bool) $byte;

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(preg_match('/^([^:]+):\s+(\d+)\s*((?i:[KMGTPEZY])B)?$/',$content[$i],$match) !== 1)
				continue;

			$key = trim(strtolower($match[1]));
			$value = trim($match[2]);

			if($byte === true && isset($match[3]) === true)
				$r[$key] = dwho_size_si_to_byte($match[3],$value);
			else
				$r[$key] = $value;
		}

		if(empty($r) === true)
			return(null);

		if(isset($r['memtotal'],$r['memfree']) === true)
			$r['memused'] = $r['memtotal'] - $r['memfree'];

		if(isset($r['swaptotal'],$r['swapfree']) === true)
			$r['swapused'] = $r['swaptotal'] - $r['swapfree'];

		return($r);
	}

	function uptime()
	{
		$path = dwho_file::joinpath(DWHO_SEP_DIR.'proc','uptime');

		if(dwho_file::is_f_r($path) === false
		|| ($content = file($path)) === false
		|| isset($content[0]) === false
		|| ($exp = explode('.',$content[0],2)) === false)
			return(false);

		return($exp[0]);
	}
}

?>
