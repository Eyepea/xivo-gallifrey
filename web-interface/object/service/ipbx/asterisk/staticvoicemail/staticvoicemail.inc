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

class xivo_service_asterisk_realstatic_staticvoicemail extends xivo_service_asterisk_realstatic
{
	var $_filename	= 'voicemail.conf';

	function _prepare($data)
	{
		if($data['category'] === 'zonemessages')
		{
			$data['name'] = $data['var_name'];

			if(($pos = strpos($data['var_val'],'|')) !== false)
			{
				$data['timezone'] = substr($data['var_val'],0,$pos);
				$data['msg_format'] = substr($data['var_val'],$pos + 1);
			}
			else
			{
				$data['timezone'] = $data['var_val'];
				$data['msg_format'] = '';
			}
		}

		return(parent::_prepare($data));
	}
}

?>
