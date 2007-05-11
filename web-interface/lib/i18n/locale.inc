<?php

class xivo_i18n_locale
{
	var $_attrib	= array();
	var $_conf	= array();

	function xivo_i18n_locale()
	{
		$dir = XIVO_PATH_LIB.XIVO_SEP_DIR.'i18n'.XIVO_SEP_DIR.'locales';

		if(($this->_conf = xivo_gat::load_get('locales',$dir)) === false)
			trigger_error('Failed to load locales listing',E_USER_ERROR);
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

	function is_valid($locale)
	{
		$r = false;

		$locale = preg_replace('/([a-z]{2,3})([\._\- ]*)([a-z]{2,3})/ie',"strtolower('\\1').'_'.strtoupper('\\3')",$locale);

		if(xivo_issa($locale,$this->_conf) === true)
		{
			$r = $locale;
		}

		return($r);
	}

	function get_info($locale)
	{
		$r = false;

		if(($locale = $this->is_valid($locale)) !== false)
		{
			$r = $this->_conf[$locale];
			$r['locale'] = $locale;
		}

		return($r);
	}

	function get_territory($locale)
	{
		$r = false;

		if(xivo_issa($locale,$this->_conf) === true && isset($this->_conf[$locale]['territo']) === true)
			$r = $this->_conf[$locale]['territo'];

		return($r);
	}

	function get_language($locale)
	{
		$r = false;

		if(xivo_issa($locale,$this->_conf) === true && isset($this->_conf[$locale]['lang']) === true)
			$r = $this->_conf[$locale]['lang'];

		return($r);
	}
}

?>
