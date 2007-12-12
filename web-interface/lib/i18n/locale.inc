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
		if(isset($this->_conf[$locale]) === false)
			$locale = preg_replace('/([a-z]{2,3})[\._\- ]+([a-z]{2,3})/ie',"strtolower('\\1').'_'.strtoupper('\\2')",$locale);

		if(xivo_issa($locale,$this->_conf) === false)
			return(false);

		return($locale);
	}

	function get_info($locale)
	{
		if(($locale = $this->is_valid($locale)) === false)
			return(false);

		$exp = explode('_',$locale,2);

		$r = $this->_conf[$locale];
		$r['locale'] = $locale;
		$r['lang'] = $exp[0];
		$r['territo'] = $exp[1];

		return($r);
	}

	function get_territory($locale)
	{
		if(($locale = $this->is_valid($locale)) === false)
			return(false);

		$territo = explode('_',$locale,2);

		return($territo[1]);
	}

	function get_language($locale)
	{
		if(($locale = $this->is_valid($locale)) === false)
			return(false);

		$lang = explode('_',$locale,2);

		return($lang[0]);
	}

	function get_list()
	{
		return($this->_conf);
	}
}

?>
