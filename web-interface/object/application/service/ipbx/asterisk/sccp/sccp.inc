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
	var $_usersccp     = false;
	var $_sccpline     = false;
	var $_sccpbutton   = false;

	function xivo_application_service_asterisk_sccp(&$service,&$dso,$param=array(),$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

# SHOULD NOT BE REQUIRED
#		if(is_object($dso) === false)
#			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if(($this->_usersccp   = &$this->_service->get_module('usersccp'))       === false
		|| ($this->_sccpline   = &$this->_service->get_module('sccpline'))         === false
		|| ($this->_sccpbutton = &$this->_service->get_module('sccpbuttonconfig')) === false)
			return(false);

		$this->_load_config(dirname(__FILE__));

		return(true);
	}

	function _explode_input_array($arr)
	{
		$res = array();
		
		$res['device'] = array_intersect_key($arr, $this->_usersccp->get_element());
#		$res['device']['description'] = $arr['callerid'];
		unset($res['device']['name']);
		if(isset($arr['allowtransfer']))
			$res['device']['transfer'] = $arr['allowtransfer'];

		$res['line'] = array_intersect_key($arr, $this->_sccpline->get_element());
		if(isset($arr['callerid']))
			$res['line']['cid_name'] = $arr['callerid'];
		if(isset($arr['number']))
			$res['line']['cid_num']  = $arr['number'];
		if(isset($arr['secret']))
			$res['line']['pin']  = $arr['secret'];
		if(isset($arr['allowtransfer']))
		{
			$map = array('0' => 'off', '1' => 'on', '' => null);
			$res['device']['transfer'] = $map[$arr['allowtransfer']];
			$res['line']['transfer']   = $map[$arr['allowtransfer']];
		}
		
		if(is_array($res['device']['addons']))
			$res['device']['addons'] = implode(',', $res['device']['addons']);

		return $res;
	}
	
	function chk_values($arr)
	{
		$values = $this->_explode_input_array($arr);

		if($this->_usersccp->chk_values($values['device']) === false
		|| $this->_sccpline->chk_values($values['line'])     === false)
		{
		}

		return $arr;
	}

	function add($arr)
	{
		$values = $this->_explode_input_array($arr);
		if(($lineid = $this->_sccpline->add($values['line'])) === false)
			return false;

		$values['device']['defaultline'] = $lineid;
		if(($devid  = $this->_usersccp->add($values['device'])) === false)
			return false;

		return $devid;
	}

	function edit($id, $arr)
	{
		$values = $this->_explode_input_array($arr);
		if(count($values['line']) > 0)
		{
			if(($user = $this->_usersccp->get($id)) === false)
				return false;

			if($this->_sccpline->edit($user['defaultline'], $values['line']) === false)
				return false;
		}

		if(count($values['device']) > 0
		&& $this->_usersccp->edit($id, $values['device']) === false)
			return false;

		return true;
	}

	function get($id, $disable)
	{
		if(($arr = $this->_usersccp->get($id, $disable)) === false)
			return false;

		if($arr['defaultline'])
			$line  = $this->_sccpline->get($arr['defaultline']);

		$arr = array_merge($arr, $line);
		if(strlen($arr['addons']) > 0)
			$arr['addons'] = preg_split('/,/', $arr['addons']);
		else
			$arr['addons'] = array();

		$map = array('off' => 0, 'on' => 1, '' => '');
		$arr['allowtransfer'] = $map[$arr['transfer']];
		
		$arr['callerid']      = $arr['cid_name'];
		$arr['number']        = $arr['cid_num'];
		$arr['secret']        = $arr['pin'];

		return $arr;
	}

	/*
		NOTE: we consider a line is use by ONE DEVICE ONLY
	*/
	function delete($id)
	{
		$arr = $this->_sccpbutton->get_all_where(array('device_id' => $id, 'type' => 'line'));
		foreach($arr as $btn)
			$this->_sccpline->delete_where(array('name' => $btn['options']));

		$user = $this->_usersccp->get($id);
		$this->_sccpline->delete($user['defaultline']);

		$this->_sccpbutton->delete_where(array('device_id' => $id));
		$ret = $this->_usersccp->delete($id);

		return($ret);
	}

	function get_element()
	{
		return array_merge($this->_sccpline->get_element(), $this->_usersccp->get_element());
	}
}

?>
