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

	function set_top($file,$preload_i18n=false,$ext='')
	{
		$this->_set_menu($file,'top',$preload_i18n,$ext);
	}

	function set_left($file,$preload_i18n=false,$ext='')
	{
		$this->_set_menu($file,'left',$preload_i18n,$ext);
	}

	function set_bottom($file,$preload_i18n=false,$ext='')
	{
		$this->_set_menu($file,'bottom',$preload_i18n,$ext);
	}

	function set_right($file,$preload_i18n=false,$ext='')
	{
		$this->_set_menu($file,'right',$preload_i18n,$ext);
	}

	function set_toolbar($file,$preload_i18n=false,$ext='')
	{
		$this->_set_menu($file,'toolbar',$preload_i18n,$ext);
	}

	function _set_menu($file,$where,$preload_i18n=false,$ext='')
	{
		$file = dwho_file::joinpath('bloc',
					    'menu',
					    ltrim($file,DWHO_SEP_DIR).
					    $this->get_file_extension($ext));

		if(dwho_file::is_f_r($this->get_path().
				     DWHO_SEP_DIR.
				     $file) !== false)
			$this->set_file('menu_'.$where,$file,$preload_i18n);
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
		if(($file = $this->get_file('menu_'.$where)) !== null)
			$this->file_include($file,null,false,null);
	}
}

?>
