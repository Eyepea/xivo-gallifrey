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

class xivo_service_asterisk_usersip_sql extends xivo_service_asterisk_abstract_sql
{
	var $_forcecolumn	= array('type'		=> 'friend',
					'protocol'	=> XIVO_SRE_IPBX_AST_PROTO_SIP,
					'category'	=> 'user');
	var $_unique		= 'name';

	function get_nb($arr=null,$disable=null,$initialized=null)
	{
		if(is_array($arr) === false)
			$arr = $this->_forcecolumn;
		else
			$arr = array_merge($arr,$this->_forcecolumn);

		if($disable !== null && isset($this->_disable) === true)
			$arr[$this->_disable] = intval((bool) $disable);

		$this->_dso->where($arr);

		if($initialized !== null)
		{
			if((bool) $initialized === false)
				$this->_dso->andwhere('IFNULL(fullcontact,\'\') = \'\'');
			else
				$this->_dso->andwherenot('IFNULL(fullcontact,\'\') = \'\'');
		}

		return(dwho_ulongint($this->_dso->select_count()));
	}
}

?>
