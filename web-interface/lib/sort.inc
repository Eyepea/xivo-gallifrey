<?php

class xivo_sort
{
	var $_param	= array();
	var $_multi	= array();
	var $_nb	= 0;

	function xivo_sort($param=array())
	{
		$param = (array) $param;

		$this->_param['key'] = false;
		$this->_param['browse'] = false;

		if(isset($param['key']) === true)
			$this->_param['key'] = (string) $param['key'];

		if(isset($param['browse']) === true)
			$this->_param['browse'] = (array) $param['browse'];

		if(isset($param['sort']) === true)
			$this->_param['sort'] = $this->_chk_sort($param['sort']);
		else
			$this->_param['sort'] = $this->_chk_sort();

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

	function _chk_sort($sort=SORT_ASC)
	{
		switch($sort)
		{
			case 'desc':
			case SORT_DESC:
				$sort = SORT_DESC;
				break;
			default:
				$sort = SORT_ASC;
		}

		return($sort);
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
		if(is_array($data) === false)
			return($data);

		$val = array_values($data);

		if(($nb = count($val)) === 0)
			return($data);

		$rs = array();

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i] = array();

			for($j = 0;$j < $this->_nb;$j++)
			{
				$rs[$i][$j] = $val[$i];
				$multi = &$this->_multi[$j];

				if(($browse = $this->_multibrowse($multi['browse'],$val[$i])) !== false)
					$rs[$i][$j] = $browse;

				if($multi['key'] !== false && is_array($rs[$i][$j]) === true
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

		if($this->_param['key'] !== false && is_array($r['a']) === true && is_array($r['b']) === true
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

		if(is_array($this->_param['browse']) === false)
			return($r);

		$val = array_values($this->_param['browse']);

		if(($nb = count($val)) === 0)
			return($r);

		for($i = 0;$i < $nb;$i++)
		{
			if(is_array($r['a']) === false || is_array($r['b']) === false
			|| isset($r['a'][$val[$i]],$r['b'][$val[$i]]) === false)
				return($r);

			$r['a'] = &$r['a'][$val[$i]];
			$r['b'] = &$r['b'][$val[$i]];
		}

		return($r);
	}

	function _multibrowse($browse,$arr)
	{
		if(is_array($browse) === false || is_array($arr) === false)
			return(false);

		$val = array_values($browse);

		if(($nb = count($val)) === 0)
			return($arr);

		for($i = 0;$i < $nb;$i++)
		{
			if(is_array($arr) === false || isset($arr[$val[$i]]) === false)
				return($arr);

			$arr = &$arr[$val[$i]];
		}

		return($arr);
	}
}

?>
