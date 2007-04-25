<?php

class xivo_gateclass
{
	var $_class = array();

	function &getinstance()
	{
		static $instance = null;

		if($instance === null)
			$instance = new xivo_gateclass();

		return(($ref = &$instance));
	}

	function set(&$obj,$name='')
	{
		$r = false;

		$name = (string) $name;

		if(is_object($obj) === true)
		{
			if($name === '')
				$name = get_class($obj);
			if($name !== '' && preg_match('/^([a-z0-9\-_]+)$/i',$name) !== 1)
				return($r);

			if(isset($this->_class[$name]) === false)
			{
				$r = true;
				$this->_class[$name] = &$obj;
			}
			else $r = true;
		}

		return($r);
	}

	function &get($name)
	{
		$r = false;

		$name = (string) $name;

		if(isset($this->_class[$name]) === true)
			$r = $this->_class[$name];

		return(($ref = &$r));
	}

	function &set_get(&$obj,$name='')
	{
		$r = false;

		$name = (string) $name;

		if($this->set($obj,$name) === true)
			$r = &$this->get(($name === '' ? get_class($obj) : $name));

		return(($ref = &$r));
	}

	function is_valid($name)
	{
		$r = false;

		$name = (string) $name;

		if(isset($this->_class[$name]) === true && is_object($this->_class[$name]) === true)
			$r = true;

		return($r);
	}
}

class xivo_gct
{
	function set(&$obj,$name='')
	{
		$agc = &xivo_gateclass::getinstance();
		return($agc->set($obj,$name));
	}

	function &get($name)
	{
		$agc = &xivo_gateclass::getinstance();
		return(($ref = &$agc->get($name)));
	}

	function &set_get(&$obj,$name='')
	{
		$agc = &xivo_gateclass::getinstance();
		return(($ref = &$agc->set_get($obj,$name)));
	}

	function is_valid($name)
	{
		$agc = &xivo_gateclass::getinstance();
		return($agc->is_valid($name));
	}
}

?>
