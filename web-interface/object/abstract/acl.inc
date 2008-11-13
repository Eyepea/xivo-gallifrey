<?php

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

		if(xivo_issa('child',$tree) === true)
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

			if(xivo_issa($k,$tree) === false
			|| ($reference = xivo_ak($k,$reference,true)) === false)
				return(false);
			else if($reference === true)
				return(true);
			else if(isset($tree[$k]['child']) === true)
				$tree = &$tree[$k]['child'];
		}

		if(xivo_ak($k,$grp,true) !== false)
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

			if(xivo_ak($k,$tree) === false)
				return(false);

			$tree = &$tree[$k];
		}

		if(is_array($tree) === false)
			return($arr_path);

		return(false);
	}
}

?>
