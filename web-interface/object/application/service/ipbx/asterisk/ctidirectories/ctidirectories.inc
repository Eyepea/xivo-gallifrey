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

class xivo_application_service_asterisk_ctidirectories extends xivo_application_asterisk_abstract
{
	var $_ctidirectories = false;
	var $_ctistatus = false;

	function xivo_application_service_asterisk_ctidirectories(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);
		if(($this->_ctidirectories = &$this->_service->get_module('ctidirectories')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function get_directories_list($order=null,$limit=null,$assoc=false)
	{
		if(($rs = $this->_ctidirectories->get_all(null,true,$order,$limit)) === false
		|| ($nb = count($rs)) === 0)
		{
			$this->_cnt = $this->_ctidirectories->get_cnt();
			return(false);
		}

		$r = $info = array();
		for($i = 0;$i < $nb;$i++)
		{
			$info['ctidirectories'] = $rs[$i];
			$r[$info['ctidirectories']['id']] = $info;
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function get_directories_edit()
	{
		return(false);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		$this->_info['directories'] = $this->_ctidirectories->get($id);

		if($this->_components === false)
			return($this->_info);

		return($this->_info);
	}

	function get_nb($arr=null,$disable=null)
	{
		return($this->_ctidirectories->get_nb($arr,$disable));
	}

	function get_elements()
	{
		$r = array();
		$r['directories'] = $this->_ctidirectories->get_element();

		return($r);
	}

	function set_add($arr)
	{
		return($this->_set('add',$arr));
	}

	function set_edit($arr)
	{
		if(empty($this->_info) === true)
			return(false);

		return($this->_set('edit',$arr));
	}

	function _set($action,$arr)
	{
		$this->_reset();

		if(($action !== 'add' && $action !== 'edit') === true
		|| dwho_issa('directories',$arr) === false)
			return(false);

		$this->set_directories($arr['directories']);

		return(($this->get_errnb() < 1));
	}

	function set_directories($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('directories',null);
			$this->_set_error('directories','invalid data');
			return(false);
		}
		else if(($rs = $this->_ctidirectories->chk_values($arr)) === false)
		{
			$this->_set_result('directories',$this->_ctidirectories->get_filter_result());
			$this->_set_error('directories',$this->_ctidirectories->get_filter_error());
			return(false);
		}

		$this->_set_result('directories',$rs);

		return(true);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| ($directoriesid = $this->add_directories()) === false)
		{
			$r = false;
			if($directoriesid !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_directories()
	{
		if(($rs = $this->get_result('directories')) === null
		|| $rs === false)
			return(false);

		$this->_return['directories'] = $this->_ctidirectories->add($rs);
		return($this->_return['directories']);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'add'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'directories':
					$this->_ctidirectories->delete($val);
					break;
			}
		}

		return(true);
	}

	function edit()
	{
		$r = true;

		$this->_status = 'edit';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_directories() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

    function edit_directories()
    {
        if(dwho_issa('directories',$this->_info) === false
        || ($rs = $this->get_result('directories')) === null
        || $rs === false)
            return(false);

        return(($this->_return['directories'] = $this->_ctidirectories->edit($this->_info['directories']['id'],$rs)));
    }

	function _reverse_edit()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'edit'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'directories':
					if(isset($this->_origin['directories']) === false
					|| $this->_origin['directories'] === false)
						$this->delete_directories();
					else
						$this->_ctidirectories->edit_origin($this->_origin['directories']);
					break;
			}
		}

		return(true);
	}

	function delete()
	{
		$r = true;

		$this->_status = 'delete';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->delete_directories() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
	}

	function delete_directories()
	{
		if(dwho_issa('directories',$this->_info) === false)
			return(false);

		$r = $this->_ctidirectories->delete($this->_info['directories']['id']);

		if($this->_status === 'delete')
			$this->_return['directories'] = $r;

		return($r);
	}

	function _reverse_delete()
	{
		if($this->_status !== 'delete'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'directories':
                    if(isset($this->_origin['directories']) === true
                    && $this->_origin['directories'] !== false)
                        $this->_ctidirectories->add_origin($this->_origin['directories']);
					break;
			}
		}

		return(true);
	}

	function enable()
	{
		$r = true;

		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_directories() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

		return($r);
	}

	function disable()
	{
		$r = true;

		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_directories() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

		return(true);
	}

	function enable_directories()
	{
		return($this->_enable_disable_directories(false));
	}

	function disable_directories()
	{
		return($this->_enable_disable_directories(true));
	}

	function _enable_disable_directories($disable=false)
	{
		if(dwho_issa('directories',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_ctidirectories->enable($this->_info['directories']['id']);
		else
			$r = $this->_ctidirectories->disable($this->_info['directories']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['directories'] = $r;

		return($r);
	}

	function _reverse_enable_disable()
	{
		if(($this->_status !== 'enable' && $this->_status !== 'disable') === true
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		$disable = $this->_status === 'enable';

		$this->_status = '';

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'directories':
					$this->_enable_disable_directories($disable);
					break;
			}
		}

		return(true);
	}
}

?>
