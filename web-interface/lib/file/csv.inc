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

class dwho_file_csv
{
	var $_link	= null;
	var $_length	= 1024;
	var $_delimiter	= ',';
	var $_enclosure	= '"';

	function parse($data,$length=0,$delimiter=null,$enclosure=null,$isfile=true,$arr=null)
	{
		$this->_length = $this->_chk_length($length);

		if(($delimiter = $this->_chk_delimiter($delimiter)) === false
		|| ($enclosure = $this->_chk_enclosure($enclosure)) === false)
			return(false);

		$this->_delimiter = $delimiter;
		$this->_enclosure = $enclosure;

		if((bool) $isfile === true)
			return($this->_parse_file($data,$arr));

		return($this->_parse_data($data,$arr));
	}

	function _parse_file($file,$arr=null)
	{
		if(($this->_open_file($file)) === false)
			return(false);
		else if(is_array($arr) === false)
			$r = $this->_get_file_data();
		else
			$r = $this->_get_file_data_array_ref($arr);

		$this->_close_file();

		return($r);
	}

	function _parse_data($data,$arr=null)
	{
		if(is_array($arr) === false)
			return($this->_get_data($data));

		return(false);
	}

	function _get_file_data()
	{
		$r = array();
		$row = 1;

		while(($data = fgetcsv($this->_link,
				       $this->_length,
				       $this->_delimiter,
				       $this->_enclosure)) !== false)
		{
			if(($nb = count($data)) === 0)
				continue;

			for($i = 0;$i < $nb;$i++)
				$r[$row][$i] = $data[$i];

			$row++;
		}

		return($r);
	}

	function _get_data($data)
	{
		if(is_array($data) === false)
			return(false);

		$data = array_values($data);

		$r = array();

		if(($nb = count($data)) === 0)
			return($r);

		for($i = 0;$i < $nb;$i++)
			$r[] = $this->_get_line_data($data[$i]);

		return($r);
	}

	function _get_line_data($line)
	{
		$reg = preg_quote($this->_enclosure);
		$enc = $this->_enclosure.$this->_enclosure;

		if(strpos($line,$enc) === false
		|| preg_match_all('/'.$reg.'+/',$line,$match,PREG_OFFSET_CAPTURE) === 0)
			return(explode($this->_delimiter,str_replace($this->_enclosure,'',$line)));

		$r = array();

		$match = $match[0];
		$nb = count($match);

		$break = false;

		for($i = 0,$j = 0;$i < $nb;$i++)
		{
			if(isset($match[$i+1]) === false)
			{
				$sstr = $match[$i][0];
				$start = 1;
				$end = -1;
				$break = true;
			}
			else if($j === 0 && strlen($match[$i+1][0]) % 2 === 1)
			{
				$sstr = $line;
				$start = $match[$i][1]+1;
				$end = $match[++$i][1]-1;
			}
			else if(strlen($match[$i][0]) % 2 === 0)
				continue;
			else if($j !== 0)
			{
				$sstr = $line;
				$start = $j + 1;
				$end = $match[$i][1] - $j - 1;
				$j = 0;
			}
			else if($j === 0)
			{
				$j = $match[$i][1];
				continue;
			}

			$r[] = str_replace($enclosure.$enclosure,$enclosure,substr($sstr,$start,$end));

			if($break === true)
				break;
		}

		return($r);
	}

	function _get_file_data_array_ref($arr)
	{
		$r = array();

		$row = 1;
		$field = $array_ref = array();

		while(($data = fgetcsv($this->_link,
				       $this->_length,
				       $this->_delimiter,
				       $this->_enclosure)) !== false)
		{
			if(isset($data[0]) === false
			|| ($row === 1 && ($nb = count($data)) === 0))
				continue;

			if($row !== 1)
			{
				$rrow = $row - 2;
				$r[$rrow] = array();
				$ref_rs = &$r[$rrow];
			}

			for($i = 0;$i < $nb;$i++)
			{
				$ref = &$data[$i];

				if($row !== 1)
				{
					if(isset($field[$i]) === false)
						continue 2;
					else if(is_array($array_ref[$field[$i]]) === true)
					{
						$array_ref[$field[$i]]['ref'] = $ref;
						$ref_rs = array_merge_recursive(dwho_clone($array_ref[$field[$i]]['array']),$ref_rs);
					}
					else
						$ref_rs[$field[$i]] = $ref;
				}
				else
				{
					if(isset($arr[$ref]) === false)
						return(false);

					$field[$i] = $ref;

					if(isset($array_ref[$ref]) === true)
						continue;
					else if(is_array($arr[$ref]) === false)
					{
						$array_ref[$ref] = '';
						continue;
					}
					else if(($array_ref[$ref] = dwho_build_array_ref($arr[$ref])) === false)
						return(false);
				}
			}
			$row++;
		}
		return($r);
	}

	function _open_file($file)
	{
		if(($file = dwho_file::is_f_r($file)) === false
		|| ($this->_link = fopen($file,'r')) === false)
			return(false);

		return(true);
	}

	function _close_file()
	{
		if(is_resource($this->_link) === true)
			fclose($this->_link);
	}

	function _chk_length($length)
	{
		return(dwho_uint($length,1024));
	}

	function _chk_delimiter($chr)
	{
		if($chr === null)
			$chr = ',';
		else if(dwho_has_len($chr,0) === true)
			$chr = $chr{0};
		else
			$chr = false;

		return($chr);
	}

	function _chk_enclosure($chr)
	{
		if($chr === null)
			$chr = '"';
		else if(dwho_has_len($chr,0) === true)
			$chr = $chr{0};
		else
			$chr = false;

		return($chr);
	}
}

?>
