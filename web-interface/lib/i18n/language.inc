<?php

class xivo_i18n_language
{
	var $_attrib	= array();
	var $_conf	= array();

	function xivo_i18n_language()
	{
		$dir = XIVO_PATH_LIB.XIVO_SEP_DIR.'i18n'.XIVO_SEP_DIR.'languages';

		if(($this->_conf = xivo_gat::load_get('languages',$dir)) === false)
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

	function get_info($lang)
	{
		$r = false;

		if(($lang = $this->chk_lang($lang)) !== false)
		{
			$r = $this->_conf[$lang];
			$r['lang'] = $lang;
		}

		return($r);
	}

	function is_valid($lang)
	{
		$r = false;

		$lang = strtolower((string) $lang);

		if(xivo_issa($lang,$this->_conf) === true)
		{
			$r = $lang;
		}

		return($r);
	}

	function exists_default($lang)
	{
		return(xivo_ak('default',$this->_conf[$lang],true));	
	}
}

?>
