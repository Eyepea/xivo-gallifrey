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

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract.inc'),true);

class xivo_application_service_asterisk_callerid_abstract extends xivo_application_service_asterisk_abstract
{
	var $_callerid	= false;
	var $_param	= array('type'	=> '');

	function xivo_application_service_asterisk_callerid_abstract(&$service,$param=array())
	{
		$this->xivo_application_service_asterisk_abstract(&$service);

		if(($this->_callerid = &$this->_service->get_module('callerid')) === false)
			return(false);

		return($this->load_init($param));
	}

	function load_init($param=array())
	{
		$this->_reset_info();
		$this->_reset_params();
		$this->_reset();

		if($this->set_params($param) === false
		|| isset($param['type']) === false
		|| $this->_callerid->chk_element_value('type',$param['type']) === false)
		{
			$this->_param['type'] = '';
			return(false);
		}

		$this->_param['type'] = $param['type'];

		return(true);
	}

	function get($typeval)
	{
		$this->_info = false;
		$this->_origin = false;

		if(($typeval = xivo_ulongint($typeval)) === 0)
			return(false);

		$where = array();
		$where['type'] = $this->_param['type'];
		$where['typeval'] = $typeval;

		if(($this->_info = $this->_callerid->get_where($where)) !== false)
			$this->_origin = $this->_callerid->get_origin();

		return($this->_info);
	}

	function get_element()
	{
		return($this->_callerid->get_element());
	}

	function set($arr)
	{
		$this->_reset();

		$info = array();
		$info['type'] = $this->_param['type'];
		$info['typeval'] = 0;

		$arr = array_merge($arr,$info);

		if(($rs = $this->_callerid->chk_values($arr)) === false)
		{
			$this->_set_error($this->_callerid->get_filter_error());
			$this->_set_result($this->_callerid->get_filter_result());
			return(false);
		}
		else if(isset($rs['callerdisplay']) === true
		&& xivo_has_len($rs['callerdisplay']) === false)
		{
			$rs['mode'] = null;
			$rs['callerdisplay'] = '';
		}

		$this->_set_result($rs);

		return(true);
	}

	function add($typeval)
	{
		if(($rs = xivo_ulongint($typeval)) === 0
		|| ($rs = $this->get_result()) === null
		|| $rs === false)
			return($rs);

		$rs['typeval'] = $typeval;

		if($this->_callerid->add($rs) === false)
			return(($this->_return = false));

		$this->_return = array();
		$this->_return['type'] = $rs['type'];
		$this->_return['typeval'] = $rs['typeval'];

		return(true);
	}

	function edit($status,$typeval)
	{
		if(($typeval = xivo_ulongint($typeval)) === 0)
			return(false);
		else if(empty($this->_origin) === true)
			return($this->add($typeval));

		$this->_return = $this->delete($status);

		if(($rs = $this->get_result()) === null)
			return($this->_return);
		else if($rs === false)
			return(false);

		$rs['typeval'] = $this->_origin['typeval'];

		if($this->_callerid->add($rs) === false)
			return(($this->_return = false));

		$this->_return = array();
		$this->_return['type'] = $rs['type'];
		$this->_return['typeval'] = $rs['typeval'];

		return(true);
	}

	function delete($status)
	{
		if(empty($this->_origin) === true)
			return(null);

		$where = array();
		$where['type'] = $this->_origin['type'];
		$where['typeval'] = $this->_origin['typeval'];

		$r = $this->_callerid->delete_where($where);

		if($status === 'delete')
			$this->_return = $r;

		return($r);
	}

	function reverse_add()
	{
		if(is_array($this->_return) === false
		|| isset($this->_return['type'],$this->_return['typeval']) === false)
			return(null);

		$where = array();
		$where['type'] = $this->_return['type'];
		$where['typeval'] = $this->_return['typeval'];

		$this->_callerid->delete_where($where);

		return(true);
	}

	function reverse_edit($status)
	{
		$this->delete($status);

		return($this->_callerid->add_origin($this->_origin));
	}
}

?>
