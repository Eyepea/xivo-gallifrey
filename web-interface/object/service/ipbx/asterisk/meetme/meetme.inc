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

xivo_service_asterisk::required(array('realstatic','realstatic.inc'),true);

class xivo_service_asterisk_realstatic_meetme extends xivo_service_asterisk_realstatic
{
	var $_filename	= 'meetme.conf';

	function _prepare($data)
	{
		if($data['category'] === 'rooms'
		&& $data['var_name'] === 'conf')
			return($this->parse_room($data));

		return(parent::_prepare($data));
	}

	function parse_room($data)
	{
		if(is_array($data) === false || isset($data['var_val']) === false)
			return(false);

		$val = explode(',',$data['var_val'],3);

		$data['number'] = xivo_has_len($val,0) === true ? $val[0] : '';
		$data['pin'] = xivo_has_len($val,1) === true ? $val[1] : '';
		$data['admin-pin'] = xivo_has_len($val,2) === true ? $val[2] : '';

		return($data);
	}
}

?>
