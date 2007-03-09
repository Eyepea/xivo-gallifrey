<?php

class xivo_authent_sqlite
{
	var $_param	= array();

	var $_attrib	= array();

	var $_dso	= null;

	function xivo_authent_sqlite(&$dso,$param=array())
	{
		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

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

		$r = false;

		$rs = $this->_dso->query('SELECT login, passwd'.($meta === true ? ', meta ' : ' ').
					'FROM user WHERE login = \''.$this->_dso->escape_string($login).'\' '.
					'AND passwd = \''.$this->_dso->escape_string($pass).'\' AND valid = \'true\' LIMIT 1');

		if($rs !== false && ($rw = $this->_dso->fetch_assoc($rs)) !== false)
		{
			if($rw['login'] === $login && $rw['passwd'] === $pass)
				$r = $meta === true ? $rw['meta'] : true;
		}

		return($r);
	}
}

?>
