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

class dwho_sort
{
	var $_param	= array();
	var $_multi	= array();
	var $_nb	= 0;

	function dwho_sort($param=array())
	{
		$param = (array) $param;

		$this->_param['key'] = false;
		$this->_param['browse'] = false;

		if(isset($param['key']) === true)
			$this->_param['key'] = (string) $param['key'];

		if(isset($param['browse']) === true)
			$this->_param['browse'] = (array) $param['browse'];

		if(isset($param['sort']) === true)
			$this->_param['sort'] = $this->_set_sort($param['sort']);
		else
			$this->_param['sort'] = $this->_set_sort();

		if(isset($param['type']) === true)
			$this->_param['type'] = $this->_chk_type($param['type']);
		else
			$this->_param['type'] = $this->_chk_type();
	}

	function add_multi($param=array())
	{
		$this->_multi[$this->_nb]['key'] = false;
		$this->_multi[$this->_nb]['browse'] = false;

		if(isset($param['key']) === true)
			$this->_multi[$this->_nb]['key'] = (string) $param['key'];

		if(isset($param['browse']) === true)
			$this->_multi[$this->_nb]['browse'] = (array) $param['browse'];

		$this->_nb++;
	}

	function _set_sort($sort='')
	{
		$sort = strtoupper($sort);

		switch($sort)
		{
			case 'D':
			case 'DESC':
			case SORT_DESC:
				return(SORT_DESC);
		}

		return(SORT_ASC);
	}

	function _chk_type($type=SORT_REGULAR)
	{
		switch($type)
		{
			case 'num':
			case 'numeric':
			case SORT_NUMERIC:
				$type = SORT_NUMERIC;
				break;
			case 'str':
			case 'string':
			case SORT_STRING:
				$type = SORT_STRING;
				break;
			default:
				$type = SORT_REGULAR;
		}

		return($type);
	}

	function multisort($data)
	{
		if(is_array($data) === false || empty($data) === true)
			return($data);

		$rs = array();

		$i = -1;

		foreach($data as $val)
		{
			$rs[++$i] = array();

			for($j = 0;$j < $this->_nb;$j++)
			{
				$rs[$i][$j] = $val;
				$multi = &$this->_multi[$j];

				if(($browse = $this->_multibrowse($multi['browse'],$val)) !== false)
					$rs[$i][$j] = $browse;

				if($multi['key'] !== false
				&& is_array($rs[$i][$j]) === true
				&& isset($rs[$i][$j][$multi['key']]) === true)
					$rs[$i][$j] = &$rs[$i][$j][$multi['key']];
			}
		}

		array_multisort($rs,$data);
	}

	function str_usort($a,$b)
	{
		$usort = $this->_usort($a,$b);

		$a = strtolower(strval($usort['a']));
		$b = strtolower(strval($usort['b']));

		return(strcmp($a,$b));
	}

	function strnat_usort($a,$b)
	{
		$usort = $this->_usort($a,$b);

		$a = strtolower(strval($usort['a']));
		$b = strtolower(strval($usort['b']));

		return(strnatcmp($a,$b));
	}

	function num_usort($a,$b)
	{
		$usort = $this->_usort($a,$b);

		$a = intval($usort['a']);
		$b = intval($usort['b']);

		if($a === $b)
			return(0);

		return(($a < $b ? -1 : 1));
	}

	function _usort($a,$b)
	{
		$r = array('a' => $a,'b' => $b);

		if(is_array($this->_param['browse']) === true)
		{
			$browse = $this->_browse($r['a'],$r['b']);
			$r['a'] = $browse['a'];
			$r['b'] = $browse['b'];
		}

		if($this->_param['key'] !== false
		&& is_array($r['a']) === true
		&& is_array($r['b']) === true
		&& isset($r['a'][$this->_param['key']],$r['b'][$this->_param['key']]) === true)
		{
			$r['a'] = &$r['a'][$this->_param['key']];
			$r['b'] = &$r['b'][$this->_param['key']];
		}

		return($r);
	}

	function _browse($a,$b)
	{
		$r = array('a' => $a,'b' => $b);

		if(is_array($this->_param['browse']) === false
		|| empty($this->_param['browse']) === true)
			return($r);

		foreach($this->_param['browse'] as $val)
		{
			if(is_array($r['a']) === false
			|| is_array($r['b']) === false
			|| isset($r['a'][$val],$r['b'][$val]) === false)
				return($r);

			$r['a'] = &$r['a'][$val];
			$r['b'] = &$r['b'][$val];
		}

		return($r);
	}

	function _multibrowse($browse,$arr)
	{
		if(is_array($browse) === false
		|| empty($browse) === true
		|| is_array($arr) === false)
			return(false);

		foreach($browse as $val)
		{
			if(is_array($arr) === false || isset($arr[$val]) === false)
				return($arr);

			$arr = &$arr[$val];
		}

		return($arr);
	}
}

?>
