<?php

class xivo_gatearray
{
	var $_array = array();

	function &getinstance()
	{
		static $instance = null;

		if($instance === null)
			$instance = new xivo_gatearray();

		return(($ref = &$instance));
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
		$file = (string) $file;
		$name = (string) $name;

		if(($pos = strrpos($file,'.')) !== false)
		{
			$file = substr($file,0,$pos);

			if(preg_match('/^[a-z0-9\-_]+$/i',$file) !== 1)
				return(false);
		}

		if($name === '')
			$name = $file;

		if($path === '')
			$path = XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'gatearray'.DIRECTORY_SEPARATOR.$file.'.inc';
		else if(is_array($path) === true)
			$path = implode(DIRECTORY_SEPARATOR,$path).DIRECTORY_SEPARATOR.$file.'.inc';
		else
			$path = rtrim($path,DIRECTORY_SEPARATOR).DIRECTORY_SEPARATOR.$file.'.inc';

		if(xivo_file::is_i($path) === false)
			return(false);
		else if(isset($this->_array[$name]) === false)
		{
			require_once($path);

			if(isset($array) === false || is_array($array) === false)
				return(false);

			$this->_array[$name] = &$array;
		}

		return($name);
	}

	function get_file($path,$once=false)
	{
		if(is_array($path) === true)
			$path = implode(DIRECTORY_SEPARATOR,$path);

		$path = (string) $path.'.inc';
		$once = (bool) $once;

		if(xivo_file::is_i($path) === false)
			return(false);

		if($once === false)
			require($path);
		else
			require_once($path);

		if(isset($array) === true && is_array($array) === true)
			$r = &$array;
		else
			$r = false;

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
		$r = false;

		if(($name = $this->load($file,$path,$name)) !== false)
			$r = &$this->get($name);

		return(($ref = &$r));
	}

	function &set_get($name,&$array)
	{
		$r = false;

		if($this->set($name,$array) === true)
			$r = &$this->get($name);

		return(($ref = &$r));
	}

	function is_valid($name)
	{
		$name = (string) $name;

		if(xivo_issa($name,$this->_array) === true)
			return(true);

		return(false);
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
		return(($ref = &$aga->get($name)));
	}

	function get_file($path,$once=false)
	{
		return(xivo_gatearray::get_file($path,$once));
	}

	function &load_get($file,$path='',$name='')
	{
		$aga = &xivo_gatearray::getinstance();
		return(($ref = &$aga->load_get($file,$path,$name)));
	}

	function &set_get($name,&$array)
	{
		$aga = &xivo_gatearray::getinstance();
		return(($ref = &$aga->set_get($name,$array)));
	}

	function is_valid($name)
	{
		$aga = &xivo_gatearray::getinstance();
		return($aga->is_valid($name));
	}
}

?>
