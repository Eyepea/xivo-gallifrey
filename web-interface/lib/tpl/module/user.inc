<?php

require_once(xivo_file::joinpath(XIVO_PATH_LIB,'tpl','module.inc'));

class xivo_tpl_user extends xivo_tpl_module
{
	var $_tpl	= null;

	function xivo_tpl_user(&$tpl)
	{
		$this->_tpl = &$tpl;
		$this->_usr = xivo_gct::get('_USR');
	}

	function get_info($name)
	{
		return($this->_usr->get_info($name));
	}
}

?>
