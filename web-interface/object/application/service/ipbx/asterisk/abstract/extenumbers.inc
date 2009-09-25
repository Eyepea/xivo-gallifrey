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

class xivo_application_service_asterisk_extenumbers_abstract extends xivo_application_service_asterisk_abstract
{
	var $_extenumbers	= false;
	var $_param		= array('type'	=> '');

	function xivo_application_service_asterisk_extenumbers_abstract(&$service,$param=array())
	{
		$this->xivo_application_service_asterisk_abstract(&$service);

		if(($this->_extenumbers = &$this->_service->get_module('extenumbers')) === false)
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
		|| $this->_extenumbers->chk_element_value('type',$param['type']) === false)
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

		if(dwho_has_len($typeval) === false)
			return(false);

		$where = array();
		$where['type'] = $this->_param['type'];
		$where['typeval'] = $typeval;

		if(($this->_info = $this->_extenumbers->get_where($where)) !== false)
			$this->_origin = $this->_extenumbers->get_origin();

		return($this->_info);
	}

	function get_element()
	{
		return($this->_extenumbers->get_element());
	}

	function set($action,$number,$context='',$typeval='')
	{
		$this->_reset();

		if($action !== 'add' && $action !== 'edit' && $action !== 'save')
			return(false);

		$extenum = array();
		$extenum['exten'] = $number;
		$extenum['context'] = $context;
		$extenum['type'] = $this->_param['type'];

		if(empty($this->_info) === false
		&& ($action === 'edit' || $action === 'save') === true)
			$extenum['typeval'] = $this->_info['typeval'];
		else
			$extenum['typeval'] = '';

		if($extenum['typeval'] === '' && $typeval !== '')
			$extenum['typeval'] = $typeval;

		$exists = false;

		if(($rs = $this->_extenumbers->chk_values($extenum)) === false
		|| ($exists = $this->_extenumbers->exists_type($rs)) !== false)
		{
			$this->_set_result($this->_extenumbers->get_filter_result());

			if($exists === false)
				$this->_set_error($this->_extenumbers->get_filter_error());
			else
				$this->_set_error('exists');

			return(false);
		}

		$this->_set_result($rs);

		return(true);
	}

	function add($typeval)
	{
		if(($rs = dwho_has_len($typeval)) === false
		|| ($rs = $this->get_result()) === null
		|| $rs === false)
			return($rs);

		$rs['typeval'] = $typeval;

		return(($this->_return = $this->_extenumbers->add($rs)));
	}

	function edit()
	{
		if(empty($this->_info) === true)
			return(false);

		$id = $this->_info['id'];

		if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);

		$rs['typeval'] = $this->_info['typeval'];

		if(($this->_return = $this->_extenumbers->edit($id,$rs)) !== false)
			$this->_return = $id;

		return($this->_return);
	}

	function save($status,$typeval)
	{
		if($status !== 'save')
			return(false);
		else if(empty($this->_info) === true)
			return($this->add($typeval));

		return($this->edit());
	}

	function delete($status)
	{
		if(empty($this->_info) === true)
			return(null);

		$r = $this->_extenumbers->delete($this->_info['id']);

		if($status === 'delete')
			$this->_return = $r;

		return($r);
	}

	function reverse_add()
	{
		if(($id = dwho_ulongint($this->_return)) !== 0)
			return($this->_extenumbers->delete($id));

		return(false);
	}

	function reverse_edit($status)
	{
		if(empty($this->_origin) === true)
			return($this->delete($status));

		return($this->_extenumbers->edit_origin($this->_origin));
	}
}

?>
