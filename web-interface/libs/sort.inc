<?php

class xivo_sort
{
	var $_param = array();

	function xivo_sort($param=array())
	{
		$param = (array) $param;

		$this->_param['key'] = false;
		$this->_param['browse'] = false;

		if(isset($param['key']) === true)
			$this->_param['key'] = (string) $param['key'];

		if(isset($param['browse']) === true)
			$this->_param['browse'] = (array) $param['browse'];
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

		return(($a > $b ? -1 : 1));
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
}

?>
