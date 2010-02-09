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

class xivo_application_service_asterisk_ctipresences extends xivo_application_asterisk_abstract
{
	var $_ctipresences = false;
	var $_ctistatus = false;

	function xivo_application_service_asterisk_ctipresences(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);
		if(($this->_ctipresences = &$this->_service->get_module('ctipresences')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function get_presences_list($order=null,$limit=null,$assoc=false)
	{
		if(($rs = $this->_ctipresences->get_all(null,true,$order,$limit)) === false
		|| ($nb = count($rs)) === 0)
		{
			$this->_cnt = $this->_ctipresences->get_cnt();
			return(false);
		}

		$r = $info = array();
		for($i = 0;$i < $nb;$i++)
		{
			$info['ctipresences'] = $rs[$i];
			$r[$info['ctipresences']['id']] = $info;
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function get_presences_edit()
	{
		return(false);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		$this->_info['presences'] = $this->_ctipresences->get($id);

		if($this->_components === false)
			return($this->_info);

		return($this->_info);
	}

	function get_nb($arr=null,$disable=null)
	{
		return($this->_ctipresences->get_nb($arr,$disable));
	}

	function get_elements()
	{
		$r = array();
		$r['presences'] = $this->_ctipresences->get_element();

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
		|| dwho_issa('presences',$arr) === false)
			return(false);

		$this->set_presences($arr['presences']);

		return(($this->get_errnb() < 1));
	}

	function set_presences($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('presences',null);
			$this->_set_error('presences','invalid data');
			return(false);
		}
		else if(($rs = $this->_ctipresences->chk_values($arr)) === false)
		{
			$this->_set_result('presences',$this->_ctipresences->get_filter_result());
			$this->_set_error('presences',$this->_ctipresences->get_filter_error());
			return(false);
		}

		$this->_set_result('presences',$rs);

		return(true);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| ($presencesid = $this->add_presences()) === false)
		{
			$r = false;
			if($presencesid !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_presences()
	{
		if(($rs = $this->get_result('presences')) === null
		|| $rs === false)
			return(false);

		$rs['deletable'] = 1;
		$this->_return['presences'] = $this->_ctipresences->add($rs);
		return($this->_return['presences']);
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
				case 'presences':
					$this->_ctipresences->delete($val);
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
		|| $this->edit_presences() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

    function edit_presences()
    {
        if(dwho_issa('presences',$this->_info) === false
        || ($rs = $this->get_result('presences')) === null
        || $rs === false)
            return(false);

        return(($this->_return['presences'] = $this->_ctipresences->edit($this->_info['presences']['id'],$rs)));
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

	function delete()
	{
		$r = true;

		$this->_status = 'delete';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->delete_presences() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
	}

	function delete_presences()
	{
		if(dwho_issa('presences',$this->_info) === false
		|| (string) $this->_info['presences']['id'] === '1')
			return(false);

		$r = $this->_ctipresences->delete($this->_info['presences']['id']);

		if($this->_status === 'delete')
			$this->_return['presences'] = $r;

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
				case 'presences':
                    if(isset($this->_origin['presences']) === true
                    && $this->_origin['presences'] !== false)
                        $this->_ctipresences->add_origin($this->_origin['presences']);
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

		if($this->enable_presences() === false)
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

		if($this->disable_presences() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

		return(true);
	}

	function enable_presences()
	{
		return($this->_enable_disable_presences(false));
	}

	function disable_presences()
	{
		return($this->_enable_disable_presences(true));
	}

	function _enable_disable_presences($disable=false)
	{
		if(dwho_issa('presences',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_ctipresences->enable($this->_info['presences']['id']);
		else
			$r = $this->_ctipresences->disable($this->_info['presences']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['presences'] = $r;

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
				case 'presences':
					$this->_enable_disable_presences($disable);
					break;
			}
		}

		return(true);
	}
}

?>
