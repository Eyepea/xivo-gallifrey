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

require_once(xivo_file::joinpath(XIVO_PATH_OBJECT,'service','abstract.inc'));

class xivo_service_monit extends xivo_service_abstract
{
	var $_ini	= array();
	var $_type	= 'monitoring';
	var $_name	= 'monit';
	var $_instance	= null;

	function xivo_service_monit($ini)
	{
		if(xivo::load_class('xivo_monit') === false)
			trigger_error('Unable to load class xivo_monit',E_USER_ERROR);

		$this->_load_config();

		if(xivo_issa('general',$ini) === false)
			trigger_error('Invalid parameter in '.__CLASS__,E_USER_ERROR);

		$this->_ini = $ini;
	}

	function __instance()
	{
		$this->_instance = new xivo_monit($this->_ini);
		$this->_instance->recv_info();

		return($this->_instance);
	}
}

?>
