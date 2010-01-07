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

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_realstatic_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_forcecolumn	= array();

	function get_all_by_var_metric($id,$disable=null)
	{
		if(($id = dwho_ulongint($id)) === 0)
			return(false);

		$this->_dso->new_select($this->_table);

		$arr = $this->_forcecolumn;
		$arr['var_metric'] = $id;

		if($disable !== null)
			$arr[$this->_disable] = intval((bool) $disable);

		$this->_dso->where($arr);

		return($this->_dso->select_all());
	}

	function get_last_var_metric($min,$max)
	{
		$min = dwho_uint($min);
		$max = dwho_uint($max);

		$column = array();
		$column['cnt'] = (object) 'COUNT(*)';
		$column['var_metric'] = (object) 'MAX(var_metric)';

		$this->_dso->new_select($this->_table,$column);
		$this->_dso->where($this->_forcecolumn);
		$this->_dso->andwherebetween('var_metric',$min,$max);

		if(($r = $this->_dso->select_single()) !== false)
			$r = $r['cnt'] > 0 ? $r['var_metric'] : $min;

		return($r);
	}

	function get_free_var_metric($min,$max)
	{
		$this->_dso->new_select($this->_table,array('id','var_metric'));

		$arr = $this->_forcecolumn;
		$arr['var_name'] = 'deleted';
		$arr[$this->_disable] = 1;

		$this->_dso->where($arr);
		$this->_dso->andwherebetween('var_metric',dwho_uint($min),dwho_uint($max));

		return($this->_dso->select_single());
	}
}

?>
