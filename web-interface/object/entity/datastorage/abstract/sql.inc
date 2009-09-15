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

require_once(xivo_file::joinpath(XIVO_PATH_LIB,'datastorage','helper','table.inc'));

class xivo_entity_abstract_sql extends xivo_datastorage_helper_table
{
	var $_table	= 'entity';
	var $_primary	= array('id' => 0);
	var $_disable	= 'disable';
	var $_unique	= 'name';

	function xivo_entity_abstract_sql(&$dso,$param=array())
	{
		parent::_init(&$dso,$param);
	}

	function search($value,$disable=null,$order=null,$limit=null)
	{
		$this->_cnt = 0;

		if(xivo_has_len($value) === false)
			return(false);

		$this->_dso->new_select($this->_table);
		$this->_dso->wherecontain('name',$value);
		$this->_dso->orwherecontain('displayname',$value);
		$this->_dso->orwherecontain('phonenumber',$value);
		$this->_dso->orwherecontain('faxnumber',$value);
		$this->_dso->orwherecontain('email',$value);
		$this->_dso->orwherecontain('url',$value);
		$this->_dso->orwherecontain('address1',$value);
		$this->_dso->orwherecontain('address2',$value);
		$this->_dso->orwherecontain('city',$value);
		$this->_dso->orwherecontain('state',$value);
		$this->_dso->orwherecontain('zipcode',$value);
		$this->_dso->orwherecontain('country',$value);
		$this->_dso->orwherecontain('description',$value);

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
}

?>
