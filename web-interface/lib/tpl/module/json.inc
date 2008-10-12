<?php

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'tpl'.XIVO_SEP_DIR.'module.inc');

class xivo_tpl_json extends xivo_tpl_module
{
	var $_tpl	= null;

	function xivo_tpl_json(&$tpl)
	{
		$this->_tpl = &$tpl;

		if(xivo::load_class('xivo_json') === false)
			trigger_error('Failed to load json class',E_USER_ERROR);
	}

	function get_header()
	{
		return(xivo_json::get_header());
	}

	function encode($data)
	{
		return(xivo_json::encode($data));
	}

	function decode($data,$assoc=false)
	{
		return(xivo_json::decode($data,$assoc));
	}

	function display()
	{
		return($this->_tpl->display('/json'));
	}
}

?>
