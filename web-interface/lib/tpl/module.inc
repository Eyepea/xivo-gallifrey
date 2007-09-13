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

	function assign($name,$value)
	{
		return($this->_tpl->assign($name,$value));
	}

	function assign_array(&$a)
	{
		return($this->_tpl->assign_array($a));
	}

	function vars($name)
	{
		return($this->_tpl->vars($name));
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

	function chk_acl($cat='',$func='',$section='')
	{
		return($this->_tpl->chk_acl($cat,$func,$section));
	}

	function &_get_module($name)
	{
		return(($ref = &$this->_tpl->get_module($name)));
	}
}

?>
