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

class xivo_application_service_asterisk_ctistatus extends xivo_application_asterisk_abstract
{
	var $_ctistatus = false;

	function xivo_application_service_asterisk_ctistatus(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);
		if(($this->_ctistatus = &$this->_service->get_module('ctistatus')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function get_status_edit()
	{
		return(false);
	}

	function get_elements()
	{
		$r = array();
		$r['status'] = $this->_ctistatus->get_element();

		return($r);
	}

	function delete()
	{
		$r = true;

		$this->_status = 'deletestatus';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->delete_status() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
	}

	function delete_status()
	{
		if(dwho_issa('ctistatus',$this->_info) === false
		|| (string) $this->_info['ctistatus']['id'] === '1')
			return(false);

		$r = $this->_ctistatus->delete($this->_info['ctistatus']['id']);

		if($this->_status === 'delete')
			$this->_return['status'] = $r;

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
				case 'status':
                    if(isset($this->_origin['status']) === true
                    && $this->_origin['status'] !== false)
                        $this->_ctistatus->add_origin($this->_origin['status']);
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

		if($this->enable_status() === false)
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

		if($this->disable_status() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

		return(true);
	}

	function enable_status()
	{
		return($this->_enable_disable_status(false));
	}

	function disable_status()
	{
		return($this->_enable_disable_status(true));
	}

	function _enable_disable_status($disable=false)
	{
		if(dwho_issa('status',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_ctistatus->enable($this->_info['status']['id']);
		else
			$r = $this->_ctistatus->disable($this->_info['status']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['status'] = $r;

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
				case 'status':
					$this->_enable_disable_status($disable);
					break;
			}
		}

		return(true);
	}

	function get_status_list($presid=null, $order=null,$limit=null,$assoc=false)
	{
		$arr = array();
		if($presid === null)
		{
			$rs = $this->_ctistatus->get_all();
		}
		else
		{
			$arr['presence_id'] = $presid;
			$rs = $this->_ctistatus->get_all_where($arr);
		}
		if($rs === false || ($nb = count($rs)) === 0)
		{
			$this->_cnt = $this->_ctistatus->get_cnt();
			return(false);
		}

		$r = $info = array();
		for($i = 0;$i < $nb;$i++)
		{
			$info['ctistatus'] = $rs[$i];
			$r[$info['ctistatus']['id']] = $info;
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function getstatus($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		$this->_info['status'] = $this->_ctistatus->get($id);

		if($this->_components === false)
			return($this->_info);

		return($this->_info);
	}
	
	function set_edit($arr)
	{
		if(empty($this->_info) === true)
			return(false);

		return($this->_set('edit',$arr));
	}

	function set_add($arr)
	{
		return($this->_set('add',$arr));
	}

	function _set($action,$arr)
	{
		$this->_reset();
		if(($action !== 'add' && $action !== 'edit') === true
		|| dwho_issa('status',$arr) === false)
			return(false);

		$this->set_status($arr['status']);

		return(($this->get_errnb() < 1));
	}

	function set_status($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('status',null);
			$this->_set_error('status','invalid data');
			return(false);
		}
		else if(($rs = $this->_ctistatus->chk_values($arr)) === false)
		{
			$this->_set_result('status',$this->_ctistatus->get_filter_result());
			$this->_set_error('status',$this->_ctistatus->get_filter_error());
			return(false);
		}

		$this->_set_result('status',$rs);

		return(true);
	}

	function get($id, $disable=null)
	{
		$this->_info = $this->_origin = array();
		$this->_info['ctistatus'] = $this->_ctistatus->get($id);

		if($this->_components === false)
			return($this->_info);

		return($this->_info);
	}

	function edit()
	{
		$r = true;

		$this->_status = 'edit';
		$this->_return = array();

		$n = $this->get_errnb();
		if($this->get_errnb() > 0
		|| $this->edit_status() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

    function edit_status()
    {
		$t = dwho_issa('status',$this->_info);
		$h = $this->get_result('status');

        if(dwho_issa('status',$this->_info) === false
        || ($rs = $this->get_result('status')) === null
        || $rs === false)
            return(false);

		$this->_return['status'] = $this->_ctistatus->edit($this->_info['status']['id'],$rs);
        return($this->_return['status']);
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
				case 'presences':
					if(isset($this->_origin['presences']) === false
					|| $this->_origin['presences'] === false)
						$this->delete_presences();
					else
						$this->_ctipresences->edit_origin($this->_origin['presences']);
					break;
			}
		}
		return(true);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| ($statusid = $this->add_status()) === false)
		{
			$r = false;
			if($statusid !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_status()
	{
		if(($rs = $this->get_result('status')) === null
		|| $rs === false)
			return(false);

		$rs['deletable'] = 1;
		$this->_return['status'] = $this->_ctistatus->add($rs);
		return($this->_return['status']);
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
				case 'status':
					$this->_ctistatus->delete($val);
					break;
			}
		}

		return(true);
	}
}
?>
