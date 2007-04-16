<?php

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'tpl'.XIVO_SEP_DIR.'module.inc');

class xivo_tpl_user extends xivo_tpl_module
{
	var $_tpl	= null;

	function xivo_tpl_user(&$tpl)
	{
		$this->_tpl = &$tpl;
		$this->_usr = xivo_gct::get('_USR');
	}

	function get_infos($name)
	{
		return($this->_usr->get_infos($name));
	}
}

?>
