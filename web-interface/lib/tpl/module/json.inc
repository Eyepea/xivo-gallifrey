<?php

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'tpl'.XIVO_SEP_DIR.'module.inc');

class xivo_tpl_json extends xivo_tpl_module
{
	var $_tpl	= null;

	function xivo_tpl_json(&$tpl)
	{
		$this->_tpl = &$tpl;

		xivo::load_extension('json');

		if(xivo::chk_extension_loaded('json') === false)
			trigger_error('Unable to load JSON',E_USER_ERROR);
	}

	function encode($data)
	{
		return(json_encode($data));
	}

	function display()
	{
		return($this->_tpl->display('/json'));
	}
}

?>
