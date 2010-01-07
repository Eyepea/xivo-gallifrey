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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_accessfeatures extends xivo_application_asterisk_abstract
{
	var $_accessfeatures	= false;
	var $_param		= array('feature'	=> '');

	function xivo_application_service_asterisk_accessfeatures(&$service,$param=array())
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_accessfeatures = &$this->_service->get_module('accessfeatures')) === false)
			return(false);

		return($this->load_init($param));
	}

	function load_init($param=array())
	{
		if($this->set_params($param) === false
		|| isset($param['feature']) === false
		|| $this->_accessfeatures->chk_element_value('feature',$param['feature']) === false)
			return(false);

		$this->_param['feature'] = $param['feature'];

		return(true);
	}

	function get()
	{
		$this->_info = false;
		$this->_origin = false;

		$where = array();
		$where['feature'] = $this->_param['feature'];

		if(($this->_info = $this->_accessfeatures->get_all_where($where)) === false)
			return(false);

		$this->_origin = $this->_accessfeatures->get_origin_list();

		return($this->_info);
	}

	function get_elements()
	{
		return($this->_accessfeatures->get_element());
	}

	function set($list)
	{
		$this->_reset();

		if(is_array($list) === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === 0)
			return(null);

		$info = array();
		$info['feature'] = $this->_param['feature'];

		$tmp = array();

		for($i = $j = $k = 0;$i < $nb;$i++)
		{
			$info['host'] = $list[$i];

			if(($rs = $this->_accessfeatures->chk_values($info)) === false
			|| isset($tmp[$rs['host']]) === true)
			{
				$this->_set_error($j++,'wrong host');
				continue;
			}

			$tmp[$rs['host']] = 1;
			$this->_set_result($k++,$rs);
		}

		return(($this->get_errnb() < 1));
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->add_accessfeatures() === false)
		{
			$r = false;
			$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_accessfeatures()
	{
		if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if(($this->_return[] = $this->_accessfeatures->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return[0]) === false)
			$this->_return = false;

		return(true);
	}

	function edit()
	{
		$r = true;

		$this->_status = 'edit';
		$this->_return = array();

		if($this->_edit_save() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function save()
	{
		$r = true;

		$this->_status = 'save';
		$this->_return = array();

		if($this->_edit_save() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function _edit_save()
	{
		if(empty($this->_info) === true)
			return($this->add_accessfeatures());

		$this->_return = $this->delete_accessfeatures();

		if(($rs = $this->get_result()) === null)
			return($this->_return);
		else if($rs === false)
			return(false);

		$this->_return = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($this->_info[$i]) === true)
				$aid = $this->_info[$i]['id'];
			else
				$aid = 0;

			if(($this->_return[] = $this->_accessfeatures->add($rs[$i],$aid)) === false)
				return(false);
		}

		if(isset($this->_return[0]) === false)
			$this->_return = false;

		return(true);
	}

	function delete()
	{
		$r = true;

		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_accessfeatures() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
	}

	function delete_accessfeatures()
	{
		if(empty($this->_info) === true)
			return(null);

		$where = array();
		$where['feature'] = $this->_param['feature'];

		$r = $this->_accessfeatures->delete_where($where);

		if($this->_status === 'delete')
			$this->_return = $r;

		return($r);
	}

	function _reverse_add()
	{
		if($this->_status !== 'add'
		|| is_array($this->_return) === false
		|| ($nb = count($this->_return)) === 0)
			return(false);

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_return[$i] !== false)
				$this->_accessfeatures->delete($this->_return[$i]);
		}

		return(true);
	}

	function _reverse_edit()
	{
		if($this->_status !== 'edit' && $this->_status !== 'save')
			return(false);

		$this->delete_accessfeatures();

		if(empty($this->_origin) === false)
			return($this->_accessfeatures->add_origin_list($this->_origin));

		return(true);
	}

	function _reverse_delete()
	{
		if($this->_status !== 'delete')
			return(false);
		else if(empty($this->_origin) === false)
			return($this->_accessfeatures->add_origin_list($this->_origin));

		return(true);
	}
}

?>
