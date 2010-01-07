<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

require_once(DWHO_PATH_ROOT.DWHO_SEP_DIR.'datastorage'.DWHO_SEP_DIR.'helper'.DWHO_SEP_DIR.'table.inc');

class xivo_user_abstract_sql extends dwho_datastorage_helper_table
{
	var $_table	= 'user';
	var $_primary	= array('id' => 0);
	var $_disable	= 'valid';
	var $_unique	= array(array('login','meta'));

	function xivo_user_abstract_sql(&$dso,$param=array())
	{
		parent::_init(&$dso,$param);
	}

	function get($id,$disable=null)
	{
		if(($r = parent::get($id,$disable)) === false)
			return(false);

		$r['obj'] = dwho_unserialize($r['obj']);

		return($r);
	}

	function chk_update($id,$dupdate)
	{
		if(($id = dwho_ulongint($id)) === 0 || ($dupdate = dwho_uint($dupdate)) === 0)
			return(false);

		$this->_dso->new_select($this->_table,'id');
		$this->_dso->where(array('id' => $id));
		$this->_dso->andwherenot(array('dupdate' => $dupdate));

		if(($r = $this->_dso->select_one()) !== false)
			$r = true;

		return($r);
	}

	function acl_edit($id,$arr)
	{
		if(($obj = $this->_get_obj($id)) === false)
			return(false);

		$obj['acl'] = $arr;

		return($this->_save_obj($id,$obj));
	}

	function _save_obj($id,$obj)
	{
		if(($id = dwho_ulongint($id)) === 0 || is_array($obj) === false)
			return(false);

		return($this->_dso->update($this->_table,
					   array('obj' => serialize($obj)),
					   array('id' => $id)));
	}

	function _get_obj($id)
	{
		if(($id = dwho_ulongint($id)) === 0)
			return(false);

		$this->_dso->new_select($this->_table,'obj');
		$this->_dso->where(array('id' => $id));

		if(($r = $this->_dso->select_one()) === false)
			return(false);

		return(dwho_unserialize($r));
	}
}

?>
