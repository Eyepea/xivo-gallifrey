<?php

class xivo_authent_abstract_sql
{
	var $_param	= array('table' => 'user');
	var $_attrib	= array();
	var $_dso	= null;

	function _init(&$dso,$param=array())
	{
		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		$param = (array) $param;

		if(isset($param['table']) === false)
			$param['table'] = $this->_param['table'];

		$this->_param = (array) $param;
	}

	function set_attrib($name,$value)
	{
		$name = (string) $name;
		$this->_attrib[$name] = $value;
	}

	function get_attrib($name)
	{
		$name = (string) $name;

		if(isset($this->_attrib[$name]) === true)
			return($this->_attrib[$name]);
		else
			return(null);
	}

	function chk($login,$pass,$meta=false)
	{
		$meta = (bool) $meta;

		$select = 'login, passwd';

		if($meta === true)
			$select .= ',meta';

		$where = 'login = \''.$this->_dso->escape_string($login).'\''.
			 ' AND passwd = \''.$this->_dso->escape_string($pass).'\''.
			 ' AND valid = 1';

		if(($r = $this->_dso->select_one($this->_param['table'],$select,$where)) !== false)
		{
			if($r['login'] === $login && $r['passwd'] === $pass)
				$r = $meta === true ? $r['meta'] : true;
		}

		return($r);
	}
}

?>
