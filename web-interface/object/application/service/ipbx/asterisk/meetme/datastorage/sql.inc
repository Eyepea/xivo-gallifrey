<?php

#
# XiVO Web-Interface
# Copyright (C) 2009  Proformatique <technique@proformatique.com>
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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract','datastorage','abstract','sql.inc'),true);

class xivo_application_service_asterisk_meetme_sql extends xivo_application_service_asterisk_abstract_sql
{
	var $_table	= array(
				'meetmefeatures'	=> '',
				'meetme'		=> '');

	function get_all($disable=null,$order=null,$limit=null,$value=null)
	{
		$this->_dso->new_select($this->_table['meetmefeatures']);

		$this->_dso->innerjoin($this->_table['meetme'],
				       $this->_table['meetmefeatures'].'.meetmeid = '.$this->_table['meetme'].'.id',
				       null,
				       array('commented','var_val'));

		if($disable !== null)
		{
			$commented = array('commented' => intval((bool) $disable));
			$this->_dso->where($commented,$this->_table['meetme']);
		}

		if(dwho_has_len($value) === true)
		{
			$this->_dso->aggregate_where();
			$this->_dso->wherecontain('name',$value,true,$this->_table['meetmefeatures']);
			$this->_dso->orwherecontain('number',$value,true,$this->_table['meetmefeatures']);
			$this->_dso->aggregate_where();
		}

		$this->_dso->new_order($order,null,$this->_table['meetmefeatures']);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all();

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count(null,'id',$this->_table['meetmefeatures'])) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function search($value,$disable=null,$order=null,$limit=null)
	{
		$this->_cnt = 0;

		if(dwho_has_len($value) === false)
			return(false);

		return($this->get_all($disable,$order,$limit,$value));
	}
}

?>
