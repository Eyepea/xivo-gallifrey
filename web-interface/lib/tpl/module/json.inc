<?php

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'tpl'.XIVO_SEP_DIR.'module.inc');

class xivo_tpl_json extends xivo_tpl_module
{
	var $_tpl	= null;
	var $_json	= null;

	function xivo_tpl_json(&$tpl)
	{
		$this->_tpl = &$tpl;

		if(xivo::load_class('xivo_json') === false)
			trigger_error('Unable to load xivo_json',E_USER_ERROR);

		$this->_json = new xivo_json();
	}

	function encode($data,$recursive=false)
	{
		return($this->_json->encode($data,$recursive));
	}

	function display()
	{
		return($this->_tpl->display('/json'));
	}
}

?>
