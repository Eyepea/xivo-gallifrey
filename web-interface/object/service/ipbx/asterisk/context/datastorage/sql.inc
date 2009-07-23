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

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_context_sql extends xivo_service_asterisk_abstract_sql
{
	var $_autoincrement	= false;
	var $_unique		= 'name';

	function search($value,$disable=null,$order=null,$limit=null)
	{
		$this->_cnt = 0;

		$this->_dso->new_select($this->_table);
		$this->_dso->wherecontain('name',$value);
		$this->_dso->orwherecontain('displayname',$value);
		$this->_dso->orwherecontain('entity',$value);

		if($this->_dso->aggregate_where() === false)
			return(false);

		$arr = array();

		if($disable !== null && isset($this->_disable) === true)
			$this->_dso->andwhere(array($this->_disable => intval((bool) $disable)));

		$this->_dso->new_order($order);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all();

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count()) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_except($name,$order=null,$limit=null)
	{
		$this->_dso->new_select($this->_table,'*');

		if(isset($this->_forcecolumn) === true)
			$this->_dso->where($this->_forcecolumn);

		if(xivo_haslen($name) === true)
			$this->_dso->andwherenot(array('name' => $name));

		$this->_dso->new_order($order);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all();

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
