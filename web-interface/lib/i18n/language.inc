<?php

class xivo_i18n_language
{
	var $_attrib	= array();
	var $_conf	= array();

	function xivo_i18n_language()
	{
		$file = xivo_file::joinpath(XIVO_PATH_LIB,'i18n','languages','languages');

		if(($this->_conf = xivo_gat::get_file($file)) === false)
			trigger_error('Failed to load languages listing',E_USER_ERROR);
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

	function is_valid($lang)
	{
		$lang = strtolower($lang);

		if(xivo_issa($lang,$this->_conf) === false)
			return(false);

		return($lang);
	}

	function get_info($lang)
	{
		if(($lang = $this->is_valid($lang)) === false)
			return(false);

		$r = $this->_conf[$lang];
		$r['lang'] = $lang;

		return($r);
	}

	function get_locale_list($lang)
	{
		if(($lang = $this->is_valid($lang)) === false)
			return(false);

		return($this->_conf[$lang]['locale']);
	}

	function exists_default($lang)
	{
		if(($lang = $this->is_valid($lang)) === false)
			return(false);

		return(xivo_ak('default',$this->_conf[$lang],true));
	}

	function get_list()
	{
		return($this->_conf);
	}
}

?>
