<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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

class xivo_service_asterisk_agentqueueskill_sql extends xivo_service_asterisk_abstract_sql
{
	var $_disable = null;

	function get_skills($agentid)
	{
		$this->_dso->new_select('agentqueueskill', array('weight'));
		$this->_dso->where("\"agentqueueskill\".\"agentid\" = $agentid");
		$this->_dso->leftjoin('queueskill',
			'queueskill.id = agentqueueskill.skillid',
			null,
			array('id', 'name'));
		
		$this->_dso->new_order('name', null, 'queueskill');
		$res = $this->_dso->select_all(null, true);

		$this->_dso->reset_order();
		$this->_dso->reset_limit();
		return($res);
	}
}

?>
