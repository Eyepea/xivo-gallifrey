<?php

class xivo_gatearray
{
	var $_array = array();

	function &getinstance()
	{
		static $instance = null;

		if($instance === null)
			$instance = new xivo_gatearray();

		return($instance);
	}

	function set($name,&$array)
	{
		$name = (string) $name;

		if(is_array($array) === false || $name === '')
			return(false);

		if(isset($this->_array[$name]) === false)
			$this->_array[$name] = &$array;

		return(true);
	}

	function load($file,$path='',$name='')
	{
		$r = false;

		$file = (string) $file;
		$name = (string) $name;

		if(($pos = strrpos($file,'.')) !== false)
		{
			$file = substr($file,0,$pos);

			if(preg_match('/^([a-z0-9\-_]+)$/i',$file) !== 1)
				return($r);
		}

		if($name === '')
			$name = $file;

		if($path === '')
			$path = XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'gatearray'.DIRECTORY_SEPARATOR.$file.'.inc';
		else
			$path = rtrim($path,DIRECTORY_SEPARATOR).DIRECTORY_SEPARATOR.$file.'.inc';

		if(xivo_file::is_i($path) === false)
			return($r);

		if(isset($this->_array[$name]) === false)
		{
			require_once($path);

			if(isset($array) === true && is_array($array) === true)
			{
				$r = $name;
				$this->_array[$name] = &$array;
			}
		}
		else $r = $name;

		return($r);
	}

	function get_file($path,$once=true)
	{
		$r = false;

		$path = (string) $path.'.inc';
		$once = (bool) $once;

		if(xivo_file::is_i($path) === false)
			return(false);

		if($once === true)
			require_once($path);
		else
			require($path);

		if(isset($array) === true && is_array($array) === true)
			$r = &$array;

		return($r);
	}

	function &get($name='')
	{
		$name = (string) $name;

		if($name === '')
			$r = &$this->_array;
		else if(xivo_issa($name,$this->_array) === true)
			$r = &$this->_array[$name];
		else
			$r = false;

		return(($ref = &$r));
	}

	function &load_get($file,$path='',$name='')
	{
		if(($name = $this->load($file,$path,$name)) !== false)
			return($this->get($name));

		return(false);
	}

	function &set_get($name,&$array)
	{
		if($this->set($name,$array) === true)
			return($this->get($name));

		return(false);
	}

	function is_valid($name)
	{
		$name = (string) $name;

		$r = false;

		if(xivo_issa($name,$this->_array) === true)
			$r = true;

		return($r);
	}
}

class xivo_gat
{
	function set($name,&$array)
	{
		$aga = &xivo_gatearray::getinstance();
		return($aga->set($name,$array));
	}

	function load($file,$path='',$name='')
	{
		$aga = &xivo_gatearray::getinstance();
		return($aga->load($file,$path,$name));
	}

	function &get($name='')
	{
		$aga = &xivo_gatearray::getinstance();
		return($aga->get($name));
	}

	function get_file($path)
	{
		return(xivo_gatearray::get_file($path));
	}

	function &load_get($file,$path='',$name='')
	{
		$aga = &xivo_gatearray::getinstance();
		return($aga->load_get($file,$path,$name));
	}

	function &set_get($name,&$array)
	{
		$aga = &xivo_gatearray::getinstance();
		return($aga->set_get($name,$array));
	}

	function is_valid($name)
	{
		$aga = &xivo_gatearray::getinstance();
		return($aga->is_valid($name));
	}
}

?>
