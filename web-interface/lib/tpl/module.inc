<?php

class xivo_tpl_module
{
	function bbf($name,$arr=array(),$namespace='')
	{
		return($this->_tpl->bbf($name,$arr,$namespace));
	}

	function load_i18n_file($file,$namespace='')
	{
		return($this->_tpl->load_i18n_file($file,$namespace));
	}

	function set_option($name,$value)
	{
		return($this->_tpl->set_option($name,$value));
	}

	function get_option($name)
	{
		return($this->_tpl->get_option($name));
	}

	function set_var($name,$value)
	{
		return($this->_tpl->set_var($name,$value));
	}

	function set_varra(&$a)
	{
		return($this->_tpl->set_varra($a));
	}

	function get_var($name)
	{
		return($this->_tpl->get_var($name));
	}

	function get_varra($name,$keys)
	{
		return($this->_tpl->get_varra($name,$keys));
	}

	function set_file($name,$value)
	{
		return($this->_tpl->set_file($name,$value));
	}

	function get_file($name)
	{
		return($this->_tpl->get_file($name));
	}

	function file_include($file,$arr=array(),$ext='')
	{
		return($this->_tpl->file_include($file,$arr,$ext));
	}

	function file_time($file)
	{
		return($this->_tpl->file_time($file));
	}

	function url($url)
	{
		return($this->_tpl->url($url));
	}

	function &_get_module($name)
	{
		return(($ref = &$this->_tpl->get_module($name)));
	}
}

?>
