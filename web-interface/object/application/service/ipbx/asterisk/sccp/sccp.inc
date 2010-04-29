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


dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_sccp extends xivo_application_asterisk_abstract
{
	var $_dso		= null;
	var $_sccpdevice   = false;
	var $_sccpline     = false;
	var $_sccpbutton   = false;

	function xivo_application_service_asterisk_sccp(&$service,&$dso,$param=array(),$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if(($this->_sccpdevice = &$this->_service->get_module('sccpdevice'))       === false
		|| ($this->_sccpline   = &$this->_service->get_module('sccpline'))         === false
		|| ($this->_sccpbutton = &$this->_service->get_module('sccpbuttonconfig')) === false)
			return(false);

		$this->_load_config(dirname(__FILE__));

		return(true);
	}

	function _explode_input_array($arr)
	{
		$res = array();
		
		$res['device'] = array_intersect_key($arr, $this->_sccpdevice->get_element());
		$res['device']['description'] = $arr['callerid'];
		unset($res['device']['name']);

		$res['line'] = array_intersect_key($arr, $this->_sccpline->get_element());
		$res['line']['cid_name'] = $arr['callerid'];
		$res['line']['cid_num']  = $arr['name'];

		return $res;
	}
	
	function chk_values($arr)
	{
		$values = $this->_explode_input_array($arr);

		if($this->_sccpdevice->chk_values($values['device']) === false
		|| $this->_sccpline->chk_values($values['line'])     === false)
		{
		}

		return $arr;
	}

	function add($arr)
	{
		$values = $this->_explode_input_array($arr);

		if(($devid  = $this->_sccpdevice->add($values['device'])) === false
		|| ($lineid = $this->_sccpline->add($values['line'])) === false)
			return false;

		// create default button: device -> line
		$button = array(
			'device_id'  => $devid, 
			'position'   => 0, 
			'type'       => 'line', 
			'options'    => $values['line']['name']
		);

		if($this->_sccpbutton->add($button) === false)
			return false;

		return true;
	}
}

?>
