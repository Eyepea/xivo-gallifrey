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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract.inc'),true);

class xivo_application_service_asterisk_contextnummember_abstract extends xivo_application_service_asterisk_abstract
{
	var $_context		= false;
	var $_contextnumbers	= false;
	var $_contextnummember	= false;
	var $_param		= array('type'	=> '');

	function xivo_application_service_asterisk_contextnummember_abstract(&$service,$param=array())
	{
		$this->xivo_application_service_asterisk_abstract(&$service);

		if(($this->_context = &$this->_service->get_module('context')) === false
		|| ($this->_contextnumbers = &$this->_service->get_module('contextnumbers')) === false
		|| ($this->_contextnummember = &$this->_service->get_module('contextnummember')) === false)
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
		|| $this->_contextnummember->chk_element_value('type',$param['type']) === false)
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

		if(($typeval = dwho_ulongint($typeval)) === 0)
			return(false);

		$where = array();
		$where['type'] = $this->_param['type'];
		$where['typeval'] = $typeval;

		if(($this->_info = $this->_contextnummember->get_where($where)) !== false)
			$this->_origin = $this->_contextnummember->get_origin();

		return($this->_info);
	}

	function set($context,$number='')
	{
		$this->_reset();

		$info = array();
		$info['context'] = $context;
		$info['type'] = $this->_param['type'];
		$info['typeval'] = 0;

		if(dwho_has_len($number) === false)
		{
			$info['number'] = '';
			$chkerror = 'invalid context';

			$chk = $this->_context->get_where(array('name' => $context));
		}
		else
		{
			$chkerror = 'out of interval';
			$info['number'] = $this->_service->display_extension($this->_service->clean_extension($number));

			$chk = $this->_contextnumbers->interval_number_exists($context,
									      $this->_param['type'],
									      $info['number']);
		}

		if($chk === false)
		{
			$this->_set_result($info);
			$this->_set_error($chkerror);
			return(false);
		}
		else if(($rs = $this->_contextnummember->chk_values($info)) === false)
		{
			$this->_set_result($this->_contextnummember->get_filter_result());
			$this->_set_error($this->_contextnummember->get_filter_error());
			return(false);
		}

		$this->_set_result($rs);

		return(true);
	}

	function add($typeval)
	{
		if(($typeval = dwho_ulongint($typeval)) === 0)
			return(false);
		else if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);

		$rs['typeval'] = $typeval;

		if($this->_contextnummember->add($rs) === false)
			return(($this->_return = false));

		$this->_return = array();
		$this->_return['type'] = $rs['type'];
		$this->_return['typeval'] = $rs['typeval'];

		return(true);
	}

	function edit($status,$typeval)
	{
		if(($typeval = dwho_ulongint($typeval)) === 0)
			return(false);
		else if(empty($this->_origin) === true)
			return($this->add($typeval));

		$this->_return = $this->delete($status);

		if(($rs = $this->get_result()) === null)
			return($this->_return);
		else if($rs === false)
			return(false);

		$rs['typeval'] = $this->_origin['typeval'];

		if($this->_contextnummember->add($rs) === false)
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

		$r = $this->_contextnummember->delete_where($where);

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

		$this->_contextnummember->delete_where($where);

		return(true);
	}

	function reverse_edit($status)
	{
		$this->delete($status);

		return($this->_contextnummember->add_origin($this->_origin));
	}
}

?>
