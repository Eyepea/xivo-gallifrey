<?php

require_once(xivo_file::joinpath(XIVO_PATH_LIB,'tpl','module.inc'));

class xivo_tpl_menu extends xivo_tpl_module
{
	var $_tpl	= null;

	function xivo_tpl_menu(&$tpl)
	{
		$this->_tpl = &$tpl;
	}

	function set_top($file,$ext='')
	{
		$this->_set_menu($file,'top',$ext);
	}

	function set_left($file,$ext='')
	{
		$this->_set_menu($file,'left',$ext);
	}

	function set_bottom($file,$ext='')
	{
		$this->_set_menu($file,'bottom',$ext);
	}

	function set_right($file,$ext='')
	{
		$this->_set_menu($file,'right',$ext);
	}

	function set_toolbar($file,$ext='')
	{
		$this->_set_menu($file,'toolbar',$ext);
	}

	function _set_menu($file,$where,$ext='')
	{
		$where = (string) $where;
		$ext = (string) $ext;

		if($ext === '')
			$ext = XIVO_TPL_FILE_EXT;

		$file = xivo_file::joinpath('bloc','menu',ltrim($file,XIVO_SEP_DIR).'.'.$ext);

		if(xivo_file::is_f_r($this->get_option('tpl_path').XIVO_SEP_DIR.$file) !== false)
			$this->set_file('menu_'.$where,$file);
	}

	function mk_top()
	{
		$this->_mk_menu('top');
	}

	function mk_left()
	{
		$this->_mk_menu('left');
	}

	function mk_bottom()
	{
		$this->_mk_menu('bottom');
	}

	function mk_right()
	{
		$this->_mk_menu('right');
	}

	function mk_toolbar()
	{
		$this->_mk_menu('toolbar');
	}

	function _mk_menu($where)
	{
		if(($file = $this->get_file('menu_'.(string) $where)) !== null)
			$this->file_include($file,null,null);
	}
}

?>
