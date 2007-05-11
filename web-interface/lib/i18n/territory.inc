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
		$r = false;

		$territo = strtoupper((string) $territo);

		if(xivo_issa($territo,$this->_conf) === true)
		{
			$r = $territo;
		}

		return($r);
	}

	function get_info($territo)
	{
		$r = false;

		if(($territo = $this->is_valid($territo)) !== false)
		{
			$r = $this->_conf[$territo];
			$r['territo'] = $territo;
		}

		return($r);
	}

	function exists_default($territo)
	{
		return(xivo_ak('default',$this->_conf[$lang],true));	
	}
}

?>
