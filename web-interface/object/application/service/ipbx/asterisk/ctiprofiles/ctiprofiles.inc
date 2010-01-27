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

class xivo_application_service_asterisk_ctiprofiles extends xivo_application_asterisk_abstract
{
	var $_ctiprofiles = false;

	function xivo_application_service_asterisk_ctiprofiles(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);
		if(($this->_ctiprofiles = &$this->_service->get_module('ctiprofiles')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function get_profiles_edit()
	{
		return(false);
	}

	function get_elements()
	{
		$r = array();
		$r['profiles'] = $this->_ctiprofiles->get_element();

		return($r);
	}

	function delete()
	{
		$r = true;

		$this->_profiles = 'delete';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->delete_profiles() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_profiles = '';

		return($r);
	}

	function delete_profiles()
	{
		if(dwho_issa('ctiprofiles',$this->_info) === false)
			return(false);

		$r = $this->_ctiprofiles->delete($this->_info['ctiprofiles']['id']);

		if($this->_profiles === 'delete')
			$this->_return['profiles'] = $r;

		return($r);
	}

	function _reverse_delete()
	{
		if($this->_profiles !== 'delete'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'profiles':
                    if(isset($this->_origin['profiles']) === true
                    && $this->_origin['profiles'] !== false)
                        $this->_ctiprofiles->add_origin($this->_origin['profiles']);
					break;
			}
		}
		return(true);
	}

	function enable()
	{
		$r = true;

		$this->_profiles = 'enable';
		$this->_return = array();

		if($this->enable_profiles() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_profiles = '';

		return($r);
	}

	function disable()
	{
		$r = true;

		$this->_profiles = 'disable';
		$this->_return = array();

		if($this->disable_profiles() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_profiles = '';

		return(true);
	}

	function enable_profiles()
	{
		return($this->_enable_disable_profiles(false));
	}

	function disable_profiles()
	{
		return($this->_enable_disable_profiles(true));
	}

	function _enable_disable_profiles($disable=false)
	{
		if(dwho_issa('profiles',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_ctiprofiles->enable($this->_info['profiles']['id']);
		else
			$r = $this->_ctiprofiles->disable($this->_info['profiles']['id']);

		if($this->_profiles === 'enable' || $this->_profiles === 'disable')
			$this->_return['profiles'] = $r;

		return($r);
	}

	function _reverse_enable_disable()
	{
		if(($this->_profiles !== 'enable' && $this->_profiles !== 'disable') === true
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		$disable = $this->_profiles === 'enable';

		$this->_profiles = '';

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'profiles':
					$this->_enable_disable_profiles($disable);
					break;
			}
		}

		return(true);
	}

	function get_profiles_list($presid=null, $order=null,$limit=null,$assoc=false)
	{
		$arr = array();
		if($presid === null)
		{
			$rs = $this->_ctiprofiles->get_all();
		}
		else
		{
			$arr['presence_id'] = $presid;
			$rs = $this->_ctiprofiles->get_all_where($arr);
		}
		if($rs === false || ($nb = count($rs)) === 0)
		{
			$this->_cnt = $this->_ctiprofiles->get_cnt();
			return(false);
		}

		$r = $info = array();
		for($i = 0;$i < $nb;$i++)
		{
			$info['ctiprofiles'] = $rs[$i];
			$r[$info['ctiprofiles']['id']] = $info;
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function getprofiles($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		$this->_info['profiles'] = $this->_ctiprofiles->get($id);

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
		|| dwho_issa('profiles',$arr) === false)
			return(false);

		$this->set_profiles($arr['profiles']);

		return(($this->get_errnb() < 1));
	}

	function set_profiles($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('profiles',null);
			$this->_set_error('profiles','invalid data');
			return(false);
		}
		else if(($rs = $this->_ctiprofiles->chk_values($arr)) === false)
		{
			$this->_set_result('profiles',$this->_ctiprofiles->get_filter_result());
			$this->_set_error('profiles',$this->_ctiprofiles->get_filter_error());
			return(false);
		}

		$this->_set_result('profiles',$rs);

		return(true);
	}

	function get($id, $disable=null)
	{
		$this->_info = $this->_origin = array();
		$this->_info['ctiprofiles'] = $this->_ctiprofiles->get($id);

		if($this->_components === false)
			return($this->_info);

		return($this->_info);
	}

	function edit()
	{
		$r = true;

		$this->_profiles = 'edit';
		$this->_return = array();

		$n = $this->get_errnb();
		if($this->get_errnb() > 0
		|| $this->edit_profiles() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_profiles = '';

		return($r);
	}

    function edit_profiles()
    {
		$t = dwho_issa('ctiprofiles',$this->_info);
		$h = $this->get_result('profiles');

        if(dwho_issa('ctiprofiles',$this->_info) === false
        || ($rs = $this->get_result('profiles')) === null
        || $rs === false)
            return(false);

		$this->_return['profiles'] = $this->_ctiprofiles->edit($this->_info['ctiprofiles']['id'],$rs);
        return($this->_return['profiles']);
    }

	function _reverse_edit()
	{
		if($this->get_errnb() > 0
		|| $this->_profiles !== 'edit'
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

		$this->_profiles = 'add';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| ($profilesid = $this->add_profiles()) === false)
		{
			$r = false;
			if($profilesid !== null)
				$this->_reverse_add();
		}

		$this->_profiles = '';

		return($r);
	}

	function add_profiles()
	{
		if(($rs = $this->get_result('profiles')) === null
		|| $rs === false)
			return(false);

		$rs['deletable'] = 1;
		$this->_return['profiles'] = $this->_ctiprofiles->add($rs);
		return($this->_return['profiles']);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
		|| $this->_profiles !== 'add'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'profiles':
					$this->_ctiprofiles->delete($val);
					break;
			}
		}

		return(true);
	}
}
?>
