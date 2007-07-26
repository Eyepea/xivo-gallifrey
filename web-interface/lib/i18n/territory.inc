<?php

class xivo_i18n_territory
{
	var $_attrib	= array();
	var $_conf	= array();

	function xivo_i18n_territory()
	{
		$dir = XIVO_PATH_LIB.XIVO_SEP_DIR.'i18n'.XIVO_SEP_DIR.'territories';

		if(($this->_conf = xivo_gat::load_get('territories',$dir)) === false)
			trigger_error('Failed to load territories listing',E_USER_ERROR);
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

	function is_valid($territo)
	{
		$territo = strtoupper((string) $territo);

		if(xivo_issa($territo,$this->_conf) === false)
			return(false);

		return($territo);
	}

	function get_info($territo)
	{
		if(($territo = $this->is_valid($territo)) === false)
			return(false);

		$r = $this->_conf[$territo];
		$r['territo'] = $territo;

		return($r);
	}

	function get_timezone($territo)
	{
		if(($territo = $this->is_valid($territo)) === false
		|| xivo_issa('tz',$this->_conf[$territo]) === false
		|| count($this->_conf[$territo]['tz']) !== 1)
			return(false);

		reset($this->_conf[$territo]['tz']);

		return(key($this->_conf[$territo]['tz']));
	}
	
	function get_timezone_list($territo)
	{
		if(($territo = $this->is_valid($territo)) === false
		|| xivo_issa('tz',$this->_conf[$territo]) === false)
			return(false);

		return($this->_conf[$territo]['tz']);	
	}

	function exists_default($territo)
	{
		if(($territo = $this->is_valid($territo)) === false)
			return(false);

		return(xivo_ak('default',$this->_conf[$territo],true));	
	}

	function get_list()
	{
		return($this->_conf);
	}
}

?>
