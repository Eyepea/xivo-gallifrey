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

class xivo_sysinfo_linux
{
	function cpuinfo()
	{
		if(xivo_file::is_f_r('/proc/cpuinfo') === false
		|| ($content = file('/proc/cpuinfo')) === false
		|| ($nb = count($content)) === 0)
			return(false);

		$arr = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(preg_match('/^([^:]+):\s+(.+)$/',$content[$i],$match) === 1)
				$arr[str_replace(' ','_',trim(strtolower($match[1])))] = trim($match[2]);
		}

		if(empty($arr) === true)
			return(null);

		return($arr);
	}

	function meminfo($byte=false)
	{
		if(xivo_file::is_f_r('/proc/meminfo') === false
		|| ($content = file('/proc/meminfo')) === false
		|| ($nb = count($content)) === 0)
			return(false);

		$byte = (bool) $byte;

		$arr = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(preg_match('/^([^:]+):\s+(\d+)\s*((?i:[KMGTPEZY])B)?$/',$content[$i],$match) !== 1)
				continue;
			
			$key = trim(strtolower($match[1]));
			$value = trim($match[2]);

			if($byte === true && isset($match[3]) === true)
				$arr[$key] = xivo_size_si_to_byte($match[3],$value);
			else
				$arr[$key] = $value;
		}

		if(empty($arr) === true)
			return(null);

		if(isset($arr['memtotal'],$arr['memfree']) === true)
			$arr['memused'] = $arr['memtotal'] - $arr['memfree'];

		if(isset($arr['swaptotal'],$arr['swapfree']) === true)
			$arr['swapused'] = $arr['swaptotal'] - $arr['swapfree'];

		return($arr);
	}

	function netinfo()
	{
		if(xivo_file::is_f_r('/proc/net/dev') === false
		|| ($content = file('/proc/net/dev')) === false
		|| ($nb = count($content)) === 0)
			return(false);

		$arr = array();

		for($i = 0;$i < $nb;$i++)
		{
			$line = trim($content[$i]);

			if(($pos = strpos($line,':')) === false)
				continue;

			$data = preg_split('/\s+/',$line);
			$name = substr($data[0],0,$pos);
			$data[0] = substr($data[0],$pos + 1);

			if(xivo_has_len($data[0]) === false)
				array_shift($data);

			$arr[$name] = array(
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
						'drop'		=> ($data[3] + $data[11])));
		}

		return($arr);
	}

	function uptime()
	{
		if(xivo_file::is_f_r('/proc/uptime') === false
		|| ($content = file('/proc/uptime')) === false
		|| isset($content[0]) === false
		|| ($exp = explode('.',$content[0],2)) === false)
			return(false);

		return($exp[0]);
	}
}

?>
