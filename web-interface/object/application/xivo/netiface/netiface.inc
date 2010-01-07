<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

dwho::load_class('dwho_network');

dwho_file::required(array(XIVO_PATH_OBJECT,'application','xivo.inc'),true);

class xivo_application_xivo_object_netiface extends xivo_application_xivo_object_abstract
{
	var $_netiface	= false;
	var $_sysnet	= false;

	function xivo_application_xivo_object_netiface(&$xobj)
	{
		$this->xivo_application_xivo_object_abstract(&$xobj);

		if(($this->_netiface = &$this->_xobj->get_module('netiface')) === false)
			return(false);

		$sysinfo = new dwho_sysinfo();
		$this->_sysnet = $sysinfo->net();

		return(true);
	}

	function get($name,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['netiface'] = $this->_netiface->get($name)) === false)
			return(false);

		$this->_origin['netiface'] = $this->_netiface->get_origin();

		$this->_info['deletable'] = $this->_netiface->exists_link($this->_info['netiface']['name']) === false;

		return($this->_info);
	}

	function get_elements()
	{
		$r = array();
		$r['netiface'] = $this->_netiface->get_element();

		return($r);
	}
	
	function get_physical_interfaces_for_vlan($disable=null,$except=null)
	{
		if(($rs = $this->_netiface->get_all($disable,true)) === false
		|| empty($rs) === true)
			return(false);

		$netinfo = $this->_sysnet->netinfo();

		if(dwho_has_len($except) === true
		&& isset($netinfo[$except]) === true)
		{
			if((bool) $netinfo[$except]['physical'] === true)
				return(false);

			unset($netinfo[$except]);
		}

		reset($rs);

		while(list($key,$val) = each($rs))
		{
			if(isset($netinfo[$val['devname']]) === false
			|| (bool) $netinfo[$val['devname']]['physical'] === false
			|| $this->_sysnet->netinfo_hwtype_id2name($val['hwtypeid']) === false
			|| (int) $val['hwtypeid'] !== (int) $netinfo[$val['devname']]['hwtypeid'])
				unset($rs[$key]);
		}

		if(empty($rs) === true)
			return(false);

		return($rs);
	}

	function normalize_interface_method($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['method'],$arr['hwtypeid']) === false
		|| ($arr['hwtypeid'] = dwho_uint($arr['hwtypeid'])) !== 1)
			return(false);

		switch($arr['method'])
		{
			case 'dhcp':
				$arr['address'] = null;
				$arr['netmask'] = null;
				$arr['broadcast'] = null;
				$arr['gateway'] = null;
				$arr['mtu'] = null;
				break;
			case 'static':
				if(dwho_has_len($arr,'address') === false
				|| dwho_has_len($arr,'netmask') === false)
					return(false);
				else if(dwho_has_len($arr,'broadcast') === true
				&& dwho_network::ipv4_in_subnet($arr['broadcast'],
								$arr['address'].'/'.$arr['netmask']) === false)
					return(false);
				else if(dwho_has_len($arr,'gateway') === true
				&& dwho_network::ipv4_in_subnet($arr['gateway'],
								$arr['address'].'/'.$arr['netmask']) === false)
					return(false);
				break;
		}

		return($arr);
	}

	function valid_interface($devname,$hwtypeid=null,$address=null,$vlanrawdevice=null,$vlanid=null,$name=null)
	{
		if($hwtypeid !== null && $hwtypeid !== 0 && ($hwtypeid = dwho_uint($hwtypeid)) === 0)
			return(false);
		else if($vlanid !== null && $vlanrawdevice === null)
			return(false);

		if($address !== null)
		{
			$where = array();
			$where['address'] = $address;

			if($name === null)
			{
				if($this->_netiface->exists($where) === true)
					return(false);
			}
			else
			{
				if(($netiface = $this->_netiface->get_primary($where)) !== false
				&& $name !== $netiface)
					return(false);
			}
		}

		if($this->_sysnet->netinfo_interface_is_virtual($devname) === true)
		{
			if($vlanrawdevice !== null)
				return(false);

			$realdevname = $this->_sysnet->netinfo_interface_realname($devname);

			if($this->_sysnet->netinfo_interface_exists($realdevname) === false)
				return(false);

			$where = array();
			$where['devname'] = $realdevname;

			if($hwtypeid !== null)
				$where['hwtypeid'] = $hwtypeid;

			if($this->_netiface->exists($where) !== true)
				return(false);

			if($hwtypeid !== null
			&& (int) $this->_sysnet->netinfo_hwtypeid($realdevname) !== $hwtypeid)
				return(false);

			return(true);
		}
		else if($vlanrawdevice !== null)
		{
			if($vlanid === null
			|| $this->_sysnet->netinfo_interface_is_physical($vlanrawdevice) !== true
			|| $this->_sysnet->netinfo_interface_is_physical($devname) === true
			|| $this->_sysnet->netinfo_interface_is_virtual($devname) === true)
				return(false);

			$where = array();
			$where['devname'] = $vlanrawdevice;

			if($hwtypeid !== null)
				$where['hwtypeid'] = $hwtypeid;

			if($this->_netiface->exists($where) !== true)
				return(false);

			$where = array();
			$where['vlanid'] = $vlanid;

			if($hwtypeid !== null)
				$where['hwtypeid'] = $hwtypeid;

			if($name === null)
			{
				if($this->_netiface->exists($where) === true)
					return(false);
			}
			else
			{
				if(($netiface = $this->_netiface->get_primary($where)) !== false
				&& $name !== $netiface)
					return(false);
			}

			if($hwtypeid !== null)
			{
				if((int) $this->_sysnet->netinfo_hwtypeid($vlanrawdevice) !== $hwtypeid)
					return(false);
				else if($this->_sysnet->netinfo_interface_exists($devname) === true
				&& (int) $this->_sysnet->netinfo_hwtypeid($devname) !== $hwtypeid)
					return(false);
			}

			return(true);
		}
		else if($this->_sysnet->netinfo_interface_exists($devname) === true)
		{
			if($hwtypeid === null)
				return(true);
			else if((int) $this->_sysnet->netinfo_hwtypeid($devname) !== $hwtypeid)
					return(false);
			else if(($info = $this->_netiface->get_where(array('devname' => $devname))) !== false
			&& (int) $info['hwtypeid'] !== $hwtypeid)
					return(false);

			return(true);
		}

		return(false);
	}

	function get_netifaces_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		$rs = $this->_netiface->get_all($disable,true,$order,$limit,$assoc);
		$this->_cnt = $this->_netiface->get_cnt();

		$netinfo = $this->_sysnet->netinfo();

		$r = array();

		if(empty($rs) === false)
		{
			foreach($rs as $val)
			{
				if(($hwtype = $this->_sysnet->netinfo_hwtype_id2name($val['hwtypeid'])) === false)
				{
					$this->_cnt--;
					continue;
				}

				$key = $val['name'].'-'.$val['devname'];

				$r[$key] = array('netinfo'	=> false,
						 'netiface'	=> $val);

				$ref = &$r[$key];
				$ref['netiface']['hwtype'] = $hwtype;
				$ref['netiface']['identity'] = $this->_xobj->identity('netiface',$ref['netiface']);

				$ref['deletable'] = $this->_netiface->exists_link($val['devname']) === false;

				if(isset($netinfo[$val['devname']]) === true
				&& (int) $val['hwtypeid'] === (int) $netinfo[$val['devname']]['hwtypeid'])
				{
					$ref['netinfo'] = $netinfo[$val['devname']];
					unset($netinfo[$val['devname']]);
				}
			}
		}

		if(empty($netinfo) === false)
		{
			foreach($netinfo as $val)
			{
				$key = $val['interface'].'-'.$val['interface'];
				$r[$key] = array('netinfo'	=> $val,
						 'netiface'	=> false,
						 'deletable'	=> false);
				$this->_cnt++;
			}
		}

		if(empty($r) === true)
			return(false);

		dwho::load_class('dwho_sort');
		$ifacesort = new dwho_sort();
		uksort($r,array(&$ifacesort,'str_usort'));

		if($limit !== null)
		{
			if(($limit = dwho_get_limit($limit)) === false)
			{
				$this->_cnt = 0;
				return(false);
			}

			$r = array_slice($r,
					 $limit[0],
					 (isset($limit[1]) === true ? $limit[1] : 0));
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

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

		if(($action !== 'add' && $action !== 'edit') === true)
			return(false);

		$this->set_netiface($arr);

		return(($this->get_errnb() < 1));
	}

	function set_netiface($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('netiface',null);
			$this->_set_error('netiface','invalid data');
			return(false);
		}

		$arr['hwtypeid'] = 1;
		$arr['disable'] = false;

		if(($rs = $this->_netiface->chk_values($arr)) === false)
		{
			$this->_set_result('netiface',$this->_netiface->get_filter_result());
			$this->_set_error('netiface',$this->_netiface->get_filter_error());
			return(false);
		}
		else if(($normalized = $this->normalize_interface_method($rs)) === false)
		{
			$this->_set_result('netiface',$rs);
			$this->_set_error('netiface','interface method not valid');
			return(false);
		}

		$this->_set_result('netiface',$normalized);

		return(true);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->add_netiface() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function add_netiface()
	{
		if(($rs = $this->get_result('netiface')) === null
		|| $rs === false
		|| $this->valid_interface($rs['devname'],
					  $rs['hwtypeid'],
					  $rs['address'],
					  $rs['vlanrawdevice'],
					  $rs['vlanid']) === false)
			return(false);

		$this->_return['netiface'] = $this->_netiface->add($rs);

		return($this->_return['netiface']);
	}

	function edit()
	{
		$r = true;

		$this->_status = 'edit';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_netiface() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_netiface()
	{
		if(dwho_issa('netiface',$this->_info) === false
		|| ($rs = $this->get_result('netiface')) === null
		|| $rs === false
		|| $this->valid_interface($rs['devname'],
					  $rs['hwtypeid'],
					  $rs['address'],
					  $rs['vlanrawdevice'],
					  $rs['vlanid'],
					  $this->_info['netiface']['name']) === false)
			return(false);
		else if(strcmp($rs['devname'],$this->_info['netiface']['devname']) !== 0
		&& $this->_netiface->exists_link($this->_info['netiface']['devname']) !== false)
			return(false);

		$rs['disable'] = $this->_info['netiface']['disable'];

		$this->_return['netiface'] = $this->_netiface->edit($this->_info['netiface']['name'],$rs);

		return($this->_return['netiface']);
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
				case 'netiface':
					if(isset($this->_origin['netiface']) === false
					|| $this->_origin['netiface'] === false)
						$this->delete_netiface();
					else
						$this->_netiface->edit_origin($this->_origin['netiface']);
					break;
			}
		}

		return(true);
	}

	function delete()
	{
		if(isset($this->_info['deletable']) === false)
			return(false);
		else if($this->_info['deletable'] === false)
			return(null);

		$r = true;

		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_netiface() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function delete_netiface()
	{
		if(dwho_issa('netiface',$this->_info) === false)
			return(false);

		$r = $this->_netiface->delete($this->_info['netiface']['name']);

		if($this->_status === 'delete')
			$this->_return['netiface'] = $r;

		return($r);
	}

	function enable()
	{
		$r = true;

		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_netiface() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function disable()
	{
		$r = true;

		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_netiface() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function enable_netiface()
	{
		return($this->_enable_disable_netiface(false));
	}

	function disable_netiface()
	{
		return($this->_enable_disable_netiface(true));
	}

	function _enable_disable_netiface($disable=false)
	{
		if(dwho_issa('netiface',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_netiface->enable($this->_info['netiface']['name']);
		else
			$r = $this->_netiface->disable($this->_info['netiface']['name']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['netiface'] = $r;

		return($r);
	}

	function add_from_json()
	{
		if(($data = $this->_get_data_from_json()) === false
		|| $this->set_add($data) === false
		|| $this->add() === false)
			return(false);

		return(true);
	}

	function edit_from_json()
	{
		if(($data = $this->_get_data_from_json()) === false
		|| $this->set_edit($data) === false
		|| $this->edit() === false)
			return(false);

		return(true);
	}
}

?>
