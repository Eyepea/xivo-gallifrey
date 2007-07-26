<?php

class xivo_i18n_timezone
{
	var $_attrib	= array();
	var $_conf	= array();

	function xivo_i18n_timezone()
	{
		$dir = XIVO_PATH_LIB.XIVO_SEP_DIR.'i18n'.XIVO_SEP_DIR.'timezones';

		if(($this->_conf = xivo_gat::load_get('timezones',$dir)) === false)
			trigger_error('Failed to load timezones listing',E_USER_ERROR);
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

	function is_valid($tz)
	{
		$tz = (string) $tz;

		if(xivo_issa($tz,$this->_conf) === false)
			return(false);

		return($tz);
	}

	function get_info($tz)
	{
		if(($tz = $this->is_valid($tz)) === false)
			return(false);

		$r = $this->_conf[$tz];
		$r['tz'] = $tz;

		return($r);
	}

	function get_territory($tz)
	{
		if($this->is_valid($tz) === false)
			return(false);

		return($this->_conf[$tz]['territo']);
	}

	function get_list()
	{
		return($this->_conf);
	}
}

?>
