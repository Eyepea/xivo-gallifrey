<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

class xivo_application_xivo_object_iproute_sql extends xivo_application_abstract_datastorage_sql
{
	var $_table	= array('iproute'	=> '');

	function search($value,$disable=null,$order=null,$limit=null)
	{
		$this->_cnt = 0;

		if(dwho_has_len($value) === false)
			return(false);

		$this->_dso->new_select($this->_table['iproute']);
		$this->_dso->wherecontain('name',$value);
		$this->_dso->orwherecontain('iface',$value);
		$this->_dso->orwherecontain('destination',$value);
		$this->_dso->orwherecontain('netmask',$value);
		$this->_dso->orwherecontain('gateway',$value);
		$this->_dso->orwherecontain('description',$value);

		if($this->_dso->aggregate_where() === false)
			return(false);

		if($disable !== null)
			$this->_dso->andwhere(array('disable' => intval((bool) $disable)));

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
