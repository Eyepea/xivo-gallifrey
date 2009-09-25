<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

require_once(dwho_file::joinpath(DWHO_PATH_ROOT,'tpl','module.inc'));

class dwho_tpl_menu extends dwho_tpl_module
{
	var $_tpl	= null;

	function dwho_tpl_menu(&$tpl)
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
			$ext = DWHO_TPL_FILE_EXT;

		$file = dwho_file::joinpath('bloc','menu',ltrim($file,DWHO_SEP_DIR).'.'.$ext);

		if(dwho_file::is_f_r($this->get_option('tpl_path').DWHO_SEP_DIR.$file) !== false)
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
