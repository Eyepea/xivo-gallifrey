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

xivo_object::required(array('abstract','datastorage','sql.inc'),true);

class xivo_object_netiface_sql extends xivo_object_abstract_sql
{
	var $_unique	= array('name','ifname');

	function exists_link($ifname)
	{
		if(dwho_has_len($ifname) === false)
			return(null);

		$this->_dso->new_select($this->_table,array('ifname'));

		// Test if exists an alias interface
		$this->_dso->wherebegin('ifname',$ifname.':');

		// Test if exists a vlan-raw-device
		$this->_dso->orwhere(array('vlanrawdevice' => $ifname));

		return($this->_dso->select_single());
	}
	
	function get_voip_iface()
	{
		$this->_dso->new_select($this->_table,array('name', 'ifname'));
	    $this->_dso->where("networktype = \"voip\"");
	    $this->_dso->where("disable = 0");
	    
		return($this->_dso->select_single());
	}
}

?>
