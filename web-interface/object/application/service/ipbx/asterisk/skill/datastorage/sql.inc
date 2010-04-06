<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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

require_once(dwho_file::joinpath(XIVO_PATH_OBJECT,'application','abstract','datastorage','sql.inc'));

class xivo_application_service_asterisk_skill_sql extends xivo_application_abstract_datastorage_sql
{
	var $_table	= array(
		'skillcat'		=> '',
		'skill'			=> '');

	function get_all()
	{
		$this->_cnt = 0;
		$arr = array();

		$this->_dso->new_select($this->_table['skillcat']);
		$r = $this->_dso->select_all(null, true, true);

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		return($r);
	}

	function get_json_list()
	{
		$this->_cnt = 0;
		$arr = array();

		$this->_dso->new_select($this->_table['skillcat']);
		
		$r = $this->_dso->select_all(null, true, true);

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		return($r);
	}


	function get($id)
	{
		$res = array();

		$this->_dso->new_select($this->_table['skillcat']);
		//$this->_dso->from($this->_table['skill']);
		$this->_dso->where($this->_table['skillcat'] . ".id = $id");

		$res = $this->_dso->select_all(null, true, true);
		if($res === false)
		{ return false; }

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		$catname = $res[0]['skillcat.name'];

		$this->_dso->new_select($this->_table['skill']);
		$this->_dso->where($this->_table['skill'] . ".catid = $id");

		$res = $this->_dso->select_all(null, true, true);

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		return array('id' => $id, 'name' => $catname, 'values' => $res);
	}
}

?>
