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

class xivo_service_asterisk_voicemail_identity
{
	function _prepare_identity(&$data)
	{
		$data['fullmailbox'] = $data['mailbox'].'@'.$data['context'];
		$data['identity'] = $data['fullname'].' ('.$data['fullmailbox'].')';

		return($data);
	}

	function mk_identity($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['mailbox'],$arr['fullname'],$arr['context']) === false)
			return(false);

		$this->_prepare_identity($arr);

		return($arr['identity']);
	}
}

?>
