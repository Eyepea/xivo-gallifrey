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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','realstatic','abstract.inc'),true);

class xivo_application_service_asterisk_realstatic_meetme_room extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name		= 'meetmeroom';
	var $_modulename	= 'staticmeetme';
	var $_category		= 'rooms';
	var $_cat_metric	= 1;
	var $_var_name		= 'conf';
	var $_filename		= 'meetme.conf';

	function xivo_application_service_asterisk_realstatic_meetme_room(&$service)
	{
		return($this->_init(&$service));
	}

	function _load_config()
	{
		return(parent::_load_config(dirname(__FILE__)));
	}

	function get_meetmeroom($id,$disable=null)
	{
		$this->_info = $this->_origin = false;

		if(($info = $this->_realstatic->get($id,$disable)) === false)
			return(false);

		$this->_info = $this->_origin = array();

		$this->_info[$this->_name] = $info;
		$this->_origin[$this->_name] = $this->_realstatic->get_origin();

		return($this->_info[$this->_name]);
	}

	function set_meetmeroom($arr)
	{
		$this->_reset();

		if(is_array($arr) === false)
		{
			$this->_set_result($this->_name,null);
			$this->_set_error($this->_name,'invalid data');
			return(false);
		}
		else if(($rs = $this->chk_values($arr)) === false)
		{
			$this->_set_result($this->_name,$this->get_filter_result());
			$this->_set_error($this->_name,$this->get_filter_error());
			return(false);
		}

		if(isset($rs['confno']) === true)
			$rs['confno'] = strtolower($rs['confno']);

		$this->_set_result($this->_name,$rs);

		return(true);
	}

	function _prepare_result()
	{
		if(($rs = $this->get_result($this->_name)) === null || $rs === false)
			return($rs);

		$arr = array();
		$arr['category']	= $this->_category;
		$arr['cat_metric']	= $this->_cat_metric;
		$arr['var_name']	= $this->_var_name;

		$arr['var_val'] = $rs['confno'].',';

		if(dwho_has_len($rs,'pin') === true)
			$arr['var_val'] .= $rs['pin'];

		if(dwho_has_len($rs,'pinadmin') === true)
			$arr['var_val'] .= ','.$rs['pinadmin'];

		$arr['var_val'] = rtrim($arr['var_val'],',');

		return($arr);
	}

	function add_meetmeroom()
	{
		if(($rs = $this->_prepare_result()) === null || $rs === false)
			return($rs);

		return(($this->_return[$this->_name] = $this->_realstatic->add($rs)));
	}

	function edit_meetmeroom($id)
	{
		if(($id = dwho_ulongint($id)) === 0)
			return(false);
		else if(($rs = $this->_prepare_result()) === null || $rs === false)
			return($rs);

		return(($this->_return[$this->_name] = $this->_realstatic->edit($id,$rs)));
	}

	function delete_meetmeroom($status,$id)
	{
		if(($id = dwho_ulongint($id)) === 0)
			return(null);

		$r = $this->_realstatic->delete($id);

		if($status === 'delete')
			$this->_return[$this->_name] = $r;

		return($r);
	}
}

?>
