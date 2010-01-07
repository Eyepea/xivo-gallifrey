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

class xivo_acl_abstract
{
	var $_acl_tree	= null;
	var $_full_tree	= null;

	function xivo_acl_abstract($arr)
	{
		if(is_array($arr) === false)
			trigger_error('Invalid excepted argument in '.__CLASS__,E_USER_ERROR);

		$this->_acl_tree = $arr;
	}

	function get()
	{
		return(null);
	}

	function get_full_tree()
	{
		if(isset($this->_full_tree) === false)
			$this->_full_tree = $this->build_full_tree($this->_acl_tree);

		return($this->_full_tree);
	}

	function build_tree_by_path($pathlist)
	{
		$r = array();

		$pathlist = array_values((array) $pathlist);

		$nb = count($pathlist);

		for($i = 0;$i < $nb;$i++)
		{
			if(($arr_path = $this->_chk_path($pathlist[$i],$this->_acl_tree)) === false)
				continue;

			$cnt = count($arr_path) - 1;

			$ref = &$r;

			for($j = 0;$j < $cnt;$j++)
			{
				if(isset($ref[$arr_path[$j]]) === false)
					$ref[$arr_path[$j]] = array();

				$ref = &$ref[$arr_path[$j]];
			}

			$ref[$arr_path[$j]] = true;
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function build_full_tree($arr,$tree=null)
	{
		if(is_array($arr) === false || empty($arr) === true)
			return(false);

		$id = $path = '';
		$level = 1;
		$parent = false;

		if(dwho_issa('child',$tree) === true)
		{
			$level = $tree['level'] + 1;
			$id = $tree['id'];
			$path = $tree['path'];
			$parent = $tree;
			unset($parent['child'],$parent['parent']);
			$tree = &$tree['child'];
		}

		foreach($arr as $k => $v)
		{
			if(isset($tree[$k]) === false)
				$tree[$k] = array();

			$tree[$k]['name'] = $k;
			$tree[$k]['id'] = ($id !== '' ? $id.'-' : '').$k;
			$tree[$k]['path'] = ($path !== '' ? $path.'/' : '').$k;
			$tree[$k]['level'] = $level;

			if($parent !== false)
				$tree[$k]['parent'] = $parent;

			if(is_array($v) === true)
			{
				$tree[$k]['child'] = array();
				$tree[$k]['child'] = $this->build_full_tree($v,$tree[$k]);
			}
			else
				$tree[$k]['value'] = (bool) $v;
		}

		return($tree);
	}

	function get_access_tree($id)
	{
		if(($info = $this->get($id)) === null)
			return(false);

		return($this->_mk_access_tree($this->get_full_tree(),$info));
	}

	function _mk_access_tree($tree,$reference)
	{
		if(is_array($tree) === false || empty($tree) === true)
			return($tree);

		reset($tree);

		while(list($key) = each($tree))
		{
			$ref = &$tree[$key];
			$ref['access'] = $this->has_access($ref['path'],$reference);

			if(isset($ref['child']) === true)
				$ref['child'] = $this->_mk_access_tree($ref['child'],$reference);
		}

		return($tree);
	}

	function has_access($section,$reference)
	{
		if($reference === null || is_bool($reference) === true)
			return((bool) $reference);

		$section = trim(preg_replace('@/+@','/',$section),'/');
		$tree = $this->get_full_tree();

		$arr = explode('/',$section);
		$nb = count($arr);

		for($i = 0;$i < $nb;$i++)
		{
			$k = &$arr[$i];
			$grp = $reference;

			if(dwho_issa($k,$tree) === false
			|| ($reference = dwho_ak($k,$reference,true)) === false)
				return(false);
			else if($reference === true)
				return(true);
			else if(isset($tree[$k]['child']) === true)
				$tree = &$tree[$k]['child'];
		}

		if(dwho_ak($k,$grp,true) !== false)
			return(true);

		return(false);
	}

	function _chk_path($path,$tree)
	{
		if(is_scalar($path) === false
		|| is_array($tree) === false)
			return(false);

		$path = (string) $path;

		$arr = explode('/',$path);
		$arr_path = $arr;

		if(($nb = count($arr)) === 0)
			return(false);

		for($i = 0;$i < $nb;$i++)
		{
			$k = &$arr[$i];

			if(dwho_ak($k,$tree) === false)
				return(false);

			$tree = &$tree[$k];
		}

		if(is_array($tree) === false)
			return($arr_path);

		return(false);
	}

	function get_section($category='',$subcategory='',$section='')
	{
		if($category !== true)
			$category = strval($category);

		$subcategory = strval($subcategory);
		$section = strval($section);

		if($section === '')
		{
			if((($section = dwho_location::get_current_location(false))) === false)
				return(false);
			else if(($pos = strrpos($section,'/')) !== false)
				$section = substr($section,0,$pos);
		}

		if($category === true)
		{
			if(isset($_SERVER['PATH_INFO']) === true)
				$section .= $_SERVER['PATH_INFO'];
		}
		else if($category !== '')
		{
			$section .= '/'.$category;

			if($subcategory !== '')
				$section .= '/'.$subcategory;
		}

		return($section);
	}

	function _chk_section($section,$id)
	{
		if(is_scalar($section) === false
		|| dwho_ulongint($id) === false)
			return(false);

		$section = trim(preg_replace('@/+@','/',$section),'/');

		$tree = $this->get_full_tree();
		$info = $this->get($id);

		$r = false;

		if($info === true)
			$r = true;
		else if(is_array($info) === true)
			$r = false;
		else
			return(false);

		$arr = explode('/',$section);
		$nb = count($arr);

		for($i = 0;$i < $nb;$i++)
		{
			$key = &$arr[$i];
			$grp = $info;

			if(dwho_issa($key,$tree) === false)
				return(false);
			else if($r === false
			&& is_bool(($info = dwho_ak($key,$info,true))) === true)
			{
				if(isset($tree[$key]['value']) === true
				&& (bool) $tree[$key]['value'] === false)
					return(false);

				return($info);
			}
			else if(isset($tree[$key]['child']) === true)
				$tree = &$tree[$key]['child'];
		}

		if($r === false && isset($key,$grp) === true)
			$r = dwho_ak($key,$grp,true) !== false;

		return($r);
	}
}

?>
