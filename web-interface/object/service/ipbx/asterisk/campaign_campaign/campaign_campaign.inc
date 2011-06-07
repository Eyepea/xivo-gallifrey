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

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_campaign_campaign extends xivo_service_asterisk_abstract
{
	var $_dso	    = null;
	var $_name	= 'campaign_campaign';
	var $_filter	= false;
	var $_origin	= false;

	function xivo_service_asterisk_campaign_campaign(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get_all($a,$b,$search,$limit)
	{
		if(($r = parent::get_all($a,$b,$search,$limit)) === false)
			return $r;

		foreach($r as &$item)
			$item['occasional'] = !is_null($item['end']);

		return $r;
	}

	function get($id)
	{
		if(($r = parent::get($id)) === false)
			return $r;

		$r['occasional'] = !is_null($r['end']);
		return $r;
	}

	function add($arr, $id=0)
	{
		$arr['created_at'] = strftime("%Y-%m-%d %H:%M:%S");
		return parent::add($arr,$id);
	}

	function edit($id, $arr, $disable=null)
	{
		$arr['created_at'] = strftime("%Y-%m-%d %H:%M:%S");
		return parent::edit($id,$arr,$disable);
	}
}

?>
