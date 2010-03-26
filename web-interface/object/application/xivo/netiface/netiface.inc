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
	var $_sysconfd	= false;
	var $_sysnet	= false;

	function xivo_application_xivo_object_netiface(&$xobj)
	{
		$this->xivo_application_xivo_object_abstract(&$xobj);

		if(($this->_netiface = &$this->_xobj->get_module('netiface')) === false
		|| ($this->_sysconfd = &$this->_xobj->get_module('sysconfd')) === false)
			return(false);

		$sysinfo = new dwho_sysinfo();
		$this->_sysnet = $sysinfo->net();

		return(true);
	}

	function get_unremovable_interfaces()
	{
		if(isset($_SERVER['SERVER_ADDR']) === false)
			return(null);
		else if(($res = $this->_sysconfd->request_get('/discover_netifaces')) === false
		|| is_array($res) === false)
			return(false);

		$phyname = null;

		$r = array();

		foreach($res as $ifname => $iface)
		{
			unset($res[$iface['name']]);

			if(dwho_ak('family',$iface,true) !== 'inet'
			|| dwho_ak('type',$iface,true) !== 'eth'
			|| dwho_has_len($iface,'address') === false
			|| dwho_has_len($iface,'netmask') === false
			|| dwho_network::ipv4_in_subnet($_SERVER['SERVER_ADDR'],
							$iface['address'].'/'.$iface['netmask']) === false)
				continue;
			else if($iface['physicalif'] === true)
				$phyname = $iface['name'];
			else if($iface['vlanif'] === true)
				$phyname = $iface['vlan-raw-device'];
			else if($iface['aliasif'] === true)
				$phyname = $iface['alias-raw-device'];
			else
				continue;

			$r[$phyname] = false;

			if($iface['address'] === $_SERVER['SERVER_ADDR'])
				$r[$iface['name']] = false;

			unset($res[$phyname]);
		}

		if($phyname === null)
			return(null);

		return($r);
	}

	function discover_current_interface()
	{
		if(isset($_SERVER['SERVER_ADDR']) === false)
			return(false);
		else if($_SERVER['SERVER_ADDR'] === '127.0.0.1')
			return(null);
		else if(($res = $this->_sysconfd->request_get('/discover_netifaces')) === false
		|| is_array($res) === false)
			return(false);

		$r = array();

		foreach($res as $ifname => $iface)
		{
			if(dwho_ak('family',$iface,true) !== 'inet'
			|| dwho_ak('type',$iface,true) !== 'eth'
			|| dwho_has_len($iface,'address') === false
			|| dwho_has_len($iface,'netmask') === false
			|| dwho_network::ipv4_in_subnet($_SERVER['SERVER_ADDR'],
							$iface['address'].'/'.$iface['netmask']) === false)
				continue;
			else if($_SERVER['SERVER_ADDR'] !== $iface['address'])
				array_push($r,$iface);
			else
			{
				array_unshift($r,$iface);
				break;
			}
		}

		return(array_shift($r));
	}

	function get_redirect_uri($devname=null,$prevdevname=null)
	{
		if(dwho_has_len($devname) === true
		&& dwho_has_len($prevdevname) === true
		&& $devname !== $prevdevname)
			return(false);
		else if(isset($_SERVER['SERVER_ADDR'],$_SERVER['SERVER_PORT']) === false
		|| ($iface = $this->discover_current_interface()) === false
		|| is_array($iface) === false
		|| dwho_has_len($iface,'address') === false)
			return(false);

		$uri = $iface['address'].':'.$_SERVER['SERVER_PORT'];

		$scheme = 'http';

		if(isset($_SERVER['HTTPS']) === true
		&& (bool) $_SERVER['HTTPS'] === true)
			$scheme .= 's';

		return($scheme.'://'.$uri);
	}

	function get($name,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['netiface'] = $this->_netiface->get($name)) === false)
			return(false);

		$this->_origin['netiface'] = $this->_netiface->get_origin();

		if($this->_netiface->exists_link($this->_info['netiface']['name']) !== false)
			$this->_info['deletable'] = false;
		else if(($unremovables = $this->get_unremovable_interfaces()) !== false
		&& isset($unremovables[$this->_info['netiface']['ifname']]) === true)
			$this->_info['deletable'] = false;
		else
			$this->_info['deletable'] = true;

		return($this->_info);
	}

	function get_elements()
	{
		$r = array();
		$r['netiface'] = $this->_netiface->get_element();

		return($r);
	}

	function is_valid_interface_method($method)
	{
		$methods = $this->_netiface->get_element_value('method');

		if($method === null || is_array($methods) === false)
			return(true);
		else if(dwho_has_len($method) === false)
			return(false);

		return(in_array($method,$methods));
	}

	function normalize_interface_method($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['method'],$arr['hwtypeid'],$arr['ifname']) === false
		|| ($arr['hwtypeid'] = dwho_uint($arr['hwtypeid'])) !== 1)
			return(false);

		switch($arr['method'])
		{
			case 'dhcp':
				if($this->_sysnet->is_alias_if($arr['ifname']) === true)
					return(false);

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
				&& dwho_network::valid_ipv4_broadcast($arr['broadcast'],
								      $arr['address'],
								      $arr['netmask']) === false)
					return(false);
				else if(dwho_has_len($arr,'gateway') === true
				&& dwho_network::ipv4_in_subnet($arr['gateway'],
								$arr['address'].'/'.$arr['netmask']) === false)
					return(false);
				break;
			default:
				return(false);
		}

		return($arr);
	}

	function clean_interface_options($arr,$outputline=false)
	{
		$outputline = (bool) $outputline;

		if(is_array($arr) === false)
			return(false);

		$r = array();

		foreach($arr as $value)
		{
			if(is_array($value) === false
			|| isset($value[0],$value[1]) === false
			|| is_scalar($value[0]) === false
			|| is_scalar($value[1]) === false)
				continue;

			switch(strtolower($value[0]))
			{
				case 'iface':
				case 'ifname':
				case 'mapping':
				case 'auto':
				case 'allow-auto':
				case 'name':
				case 'family':
				case 'method':
				case 'address':
				case 'netmask':
				case 'broadcast':
				case 'gateway':
				case 'vlan-id':
				case 'vlan-raw-device':
				case 'mtu':
					continue;
				default:
					if($outputline === false)
						$r[] = array($value[0],$value[1]);
					else
						$r[] = $value[0].' '.$value[1];
			}
		}

		if($outputline === false)
			return($r);

		return(implode(DWHO_EOL,$r));
	}

	function normalize_interface_options($options,$outputline=false)
	{
		if(is_string($options) === false)
			return(null);

		$options = explode("\n",$options);

		$nb = count($options);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(preg_match('/^\s*([^\s]+)\s+([^#]+)/',$options[$i],$match) === 1)
				$r[] = array($match[1],trim($match[2]));
		}

		if(($optionscleaned = $this->clean_interface_options($r,$outputline)) !== false)
			return($optionscleaned);

		return(null);
	}

	function get_sysconf_netiface_info($ifname,$hwtypeid=null,$method=true,$optionsoutputline=false)
	{
		if(dwho_has_len($ifname) === false)
			return(false);

		if($hwtypeid !== null && $hwtypeid !== 0)
			$hwtypeid = dwho_uint($hwtypeid,null);

		$info = $this->_sysconfd->request_get('/netiface',array(array('ifname',$ifname)));

		if(is_array($info) === false)
			return(null);
		else if($hwtypeid !== null
		&& isset($info['hwtypeid']) === true
		&& $hwtypeid !== dwho_uint($info['hwtypeid']))
			return(false);

		if((bool) $method === true
		&& dwho_has_len($info,'method') === true
		&& $this->is_valid_interface_method($info['method']) === false)
			return(false);

		return($info);
	}

	function get_physical_interfaces_for_vlan($disable=null,$except=null)
	{
		if(($rs = $this->_netiface->get_all($disable,true)) === false
		|| empty($rs) === true)
			return(false);

		$netinfo = $this->_sysconfd->request_get('/discover_netifaces');

		if(is_array($netinfo) === false)
			$netinfo = null;

		if(dwho_has_len($except) === true
		&& isset($netinfo[$except]) === true)
		{
			if(isset($netinfo[$except]['physicalif']) === true
			&& (bool) $netinfo[$except]['physicalif'] === true)
				return(false);

			unset($netinfo[$except]);
		}

		reset($rs);

		while(list($key,$val) = each($rs))
		{
			if(isset($netinfo[$val['ifname']],$netinfo[$val['ifname']]['physicalif']) === false
			|| (bool) $netinfo[$val['ifname']]['physicalif'] === false
			|| $this->_sysnet->netinfo_hwtype_id2name($val['hwtypeid']) === false
			|| dwho_uint($val['hwtypeid']) !== dwho_uint($netinfo[$val['ifname']]['hwtypeid']))
				unset($rs[$key]);
		}

		if(empty($rs) === true)
			return(false);

		return($rs);
	}

	function valid_interface($ifname,$hwtypeid=null,$address=null,$vlanrawdevice=null,$vlanid=null,$name=null)
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

		if($this->_sysnet->is_alias_if($ifname) === true)
		{
			if($vlanrawdevice !== null
			|| ($phyifname = $this->_sysnet->physical_ifname_from_alias_if($ifname)) === false)
				return(false);

			$phyiface = $this->_sysconfd->request_get('/netiface',
								  array(array('ifname',$phyifname)));
            
			if(is_array($phyiface) === false)
				return(false);

			$where = array();
			$where['ifname'] = $phyifname;

			if($hwtypeid !== null)
				$where['hwtypeid'] = $hwtypeid;

			if($this->_netiface->exists($where) !== true)
				return(false);

			if($hwtypeid !== null
			&& isset($phyiface['hwtypeid']) === true
			&& dwho_uint($phyiface['hwtypeid']) !== $hwtypeid)
				return(false);

			return(true);
		}
		else if($vlanrawdevice !== null)
		{
			if($this->_sysnet->is_vlan_if($ifname) === false
			|| $vlanid === null)
				return(false);

			$vconfig = $this->_sysnet->vlan_info_from_ifname($ifname);

			if(isset($vconfig['vlan-id']) === false
			|| $vconfig['vlan-id'] !== dwho_uint($vlanid))
				return(false);

			if(isset($vconfig['vlan-raw-device']) === true
			&& $vlanrawdevice !== $vconfig['vlan-raw-device'])
				return(false);

			$rawdevice = $this->_sysconfd->request_get('/netiface',
								   array(array('ifname',$vlanrawdevice)));

			if(is_array($rawdevice) === false
			|| isset($rawdevice['physicalif']) === false
			|| (bool) $rawdevice['physicalif'] === false)
				return(false);

			$devinfo = $this->_sysconfd->request_get('/netiface',
								 array(array('ifname',$ifname)));

			if(is_array($devinfo) === true
			&& isset($devinfo['physicalif']) === true
			&& (bool) $devinfo['physicalif'] === true)
				return(false);

			if($this->_sysnet->is_alias_if($ifname) === true)
				return(false);

			$where = array();
			$where['ifname'] = $vlanrawdevice;

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
				if(dwho_uint($rawdevice['hwtypeid']) !== $hwtypeid)
					return(false);
				else if(is_array($devinfo) === true
				&& isset($devinfo['hwtypeid']) === true
				&& dwho_uint($devinfo['hwtypeid']) !== $hwtypeid)
					return(false);
			}

			return(true);
		}

		$devinfo = $this->_sysconfd->request_get('/netiface',
							 array(array('ifname',$ifname)));
        
		if(is_array($devinfo) === true)
		{
			if($hwtypeid === null)
				return(true);
			else if(isset($devinfo['hwtypeid']) === true
			&& dwho_uint($devinfo['hwtypeid']) !== $hwtypeid)
				return(false);
			else if(($info = $this->_netiface->get_where(array('ifname' => $ifname))) !== false
			&& dwho_uint($info['hwtypeid']) !== $hwtypeid)
				return(false);

			return(true);
		}

		return(false);
	}

	function netiface_type_to_hwtype($type)
	{
		switch($type)
		{
			case 'eth':
				return('ether');
			case 'loopback':
				return('loopback');
			case 'tun':
				return('tunnel');
			case 'other':
				return('void');
		}

		return('none');
	}

	function get_netifaces_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		$rs = $this->_netiface->get_all($disable,true,$order,$limit,$assoc);
		$this->_cnt = $this->_netiface->get_cnt();

		$netinfo = $this->_sysconfd->request_get('/discover_netifaces');
		$unremovables = $this->get_unremovable_interfaces();

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

				$key = $val['name'].'-'.$val['ifname'];

				$r[$key] = array('netinfo'	=> false,
						 'netiface'	=> $val);

				$ref = &$r[$key];
				$ref['netiface']['hwtype'] = $hwtype;
				$ref['netiface']['identity'] = $this->_xobj->identity('netiface',$ref['netiface']);

				if($this->_netiface->exists_link($val['ifname']) !== false)
					$ref['deletable'] = false;
				else if(($unremovables = $this->get_unremovable_interfaces()) !== false
				&& isset($unremovables[$val['ifname']]) === true)
					$ref['deletable'] = false;
				else
					$ref['deletable'] = true;

				$ref['actionable'] = $this->is_valid_interface_method($val['method']);

				if(isset($netinfo[$val['ifname']]) === true
				&& (int) $val['hwtypeid'] === dwho_uint($netinfo[$val['ifname']]['hwtypeid']))
				{
					$ref['netinfo'] = $netinfo[$val['ifname']];
					$ref['netinfo']['interface'] = $ref['netinfo']['name'];

					if($ref['netinfo']['hwtypeid'] !== null)
						$ref['netinfo']['hwtype'] = $this->_sysnet->netinfo_hwtype_id2name($ref['netinfo']['hwtypeid']);
					else
						$ref['netinfo']['hwtype'] = $this->netiface_type_to_hwtype($ref['netinfo']['type']);

					$ref['netinfo']['flags'] = $this->_sysnet->netinfo_iff_to_dict($ref['netinfo']['flags']);

					unset($netinfo[$val['ifname']]);
				}
			}
		}

		if(empty($netinfo) === false)
		{
			foreach($netinfo as $val)
			{
				if(is_array($val) === false || isset($val['type']) === false)
					continue;

				$actionable		= true;
				$val['interface']	= $val['name'];

				if(isset($val['method']) === true)
					$actionable = $this->is_valid_interface_method($val['method']);

				if($val['hwtypeid'] !== null)
					$val['hwtype'] = $this->_sysnet->netinfo_hwtype_id2name($val['hwtypeid']);
				else
					$val['hwtype'] = $this->netiface_type_to_hwtype($val['type']);

				$val['flags'] = $this->_sysnet->netinfo_iff_to_dict($val['flags']);

				$key = $val['name'].'-'.$val['name'];

				$r[$key] = array('netinfo'	=> $val,
						 'netiface'	=> false,
						 'deletable'	=> false,
						 'actionable'	=> $actionable);
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

		if(is_array($arr) === false)
			return(false);

		$devname = null;

		switch($action)
		{
			case 'add':
				if(isset($arr['devname']) === true
				&& dwho_has_len($arr['devname']) === true)
					$devname = $arr['devname'];
				break;
			case 'edit':
				if(dwho_issa('netiface',$this->_info) === true
				&& isset($this->_info['netiface']['ifname']) === true)
					$devname = $this->_info['netiface']['ifname'];
				break;
			default:
				return(false);
		}

		$this->set_netiface($arr);
		$this->set_sysconfnet($devname);
		
		return($this->get_errnb() < 1);
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

		if(is_array($this->_info) === true
		&& isset($this->_info['deletable']) === true
		&& $this->_info['deletable'] === false)
			$arr['disable'] = false;

		if(isset($arr['options']) === true)
			$arr['options'] = $this->normalize_interface_options($arr['options'],true);

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

	function set_sysconfnet($devname=null)
	{
		if(($arr = $this->get_result('netiface')) === false
		|| is_array($arr) === false)
			return(false);

		if($this->_sysnet->is_alias_if($arr['ifname']) === true
		|| $this->_sysnet->is_vlan_if($arr['ifname']) === true)
		{
			$rs = dwho_array_intersect_key(array('ifname'		=> true,
						             'method'		=> true,
							     'vlanid'		=> true,
							     'vlanrawdevice'	=> true,
							     'address'		=> true,
							     'netmask'		=> true,
							     'broadcast'	=> true,
							     'gateway'		=> true,
							     'mtu'		=> true,
							     'options'		=> true),
						       $arr);

			if(isset($arr['disable']) === true)
				$rs['up'] = $rs['auto'] = (bool) $arr['disable'] === false;

			$rs['virtualif'] = true;
			$rs['devname'] = $devname;
		}
		else
		{
			$rs = dwho_array_intersect_key(array('ifname'		=> true,
							     'method'		=> true,
						       	     'address'		=> true,
							     'netmask'		=> true,
							     'broadcast'	=> true,
							     'gateway'		=> true,
							     'mtu'		=> true,
							     'options'		=> true),
						       $arr);

			if(isset($arr['disable']) === true)
				$rs['up'] = $rs['auto'] = (bool) $arr['disable'] === false;

			$rs['virtualif'] = false;
			$rs['devname'] = $devname;
		}

		if(isset($rs['vlanid']) === true)
			$rs['vlanid'] = dwho_uint($rs['vlanid']);

		if(isset($rs['mtu']) === true)
			$rs['mtu'] = dwho_uint($rs['mtu']);

		if(isset($rs['options']) === true)
			$rs['options'] = $this->normalize_interface_options($rs['options'],false);

		$this->_set_result('sysconfnet',$rs);

		return(true);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$netifacename = null;

		if($this->get_errnb() > 0
		|| ($netifacename = $this->add_netiface()) === false
		|| $this->commit_sysconfnet() === false)
		{
			$r = false;
			if($netifacename !== null)
				$this->_reverse_add();
		}

		$this->_status = '';
        if($r === false)
            return false;

        return $this->update_commonconf();
	}

	function add_netiface()
	{
		if(($rs = $this->get_result('netiface')) === null
		|| $rs === false
		|| $this->valid_interface($rs['ifname'],
					  $rs['hwtypeid'],
					  $rs['address'],
					  $rs['vlanrawdevice'],
					  $rs['vlanid']) === false)
			return(false);

		if($this->_netiface->add($rs) !== false)
			$this->_return['netiface'] = $rs['name'];
		else
			$this->_return['netiface'] = false;

		return($this->_return['netiface']);
	}

	function commit_sysconfnet()
	{
		if(($rs = $this->get_result('sysconfnet')) === null
		|| $rs === false)
			return(false);

		$virtualif = (bool) $rs['virtualif'];
		unset($rs['virtualif']);

		if($virtualif === true)
		{
			if(dwho_has_len($rs['devname']) === true)
				$ifname = $rs['devname'];
			else
				$ifname = $rs['ifname'];

			unset($rs['devname']);

			$ret = (bool) $this->_sysconfd->request_post('/replace_virtual_eth_ipv4',
								     $rs,
								     array(array('ifname',$ifname)));
		}
		else
		{
			$ifname = $rs['ifname'];
			unset($rs['ifname'],$rs['devname']);

			$ret = (bool) $this->_sysconfd->request_post('/modify_physical_eth_ipv4',
								     $rs,
								     array(array('ifname',$ifname)));
		}

		return(($this->_return['sysconfnet'] = $ret));
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
				case 'netiface':
					$this->_netiface->delete($val);
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
		|| $this->edit_netiface() === false
		|| $this->commit_sysconfnet() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';
        if($r === false)
            return false;

        return $this->update_commonconf();
	}

	function edit_netiface()
	{
		if(dwho_issa('netiface',$this->_info) === false
		|| ($rs = $this->get_result('netiface')) === null
		|| $rs === false
		|| $this->valid_interface($rs['ifname'],
					  $rs['hwtypeid'],
					  $rs['address'],
					  $rs['vlanrawdevice'],
					  $rs['vlanid'],
					  $this->_info['netiface']['name']) === false)
			return(false);
		else if(strcmp($rs['ifname'],$this->_info['netiface']['ifname']) !== 0
		&& $this->_netiface->exists_link($this->_info['netiface']['ifname']) !== false)
			return(false);

		if($this->_netiface->edit($this->_info['netiface']['name'],$rs) !== false)
			$this->_return['netiface'] = $rs['name'];
		else
			$this->_return['netiface'] = false;

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
					$this->_netiface->delete($val);

					if(isset($this->_origin['netiface']) === true
					&& $this->_origin['netiface'] !== false)
						$this->_netiface->add_origin($this->_origin['netiface']);
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

		if($this->delete_netiface() === false
		|| $this->delete_sysconfnet() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';
        if($r === false)
            return false;
    
        return $this->update_commonconf();
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

	function delete_sysconfnet()
	{
		if(dwho_issa('netiface',$this->_info) === false)
			return(false);

		$r = (bool) $this->_sysconfd->request_get('/delete_eth_ipv4',
							   array(array('ifname',$this->_info['netiface']['ifname'])));

		if($this->_sysconfd->last_status_code() === 404)
			$r = null;

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['sysconfnet'] = $r;

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
				case 'netiface':
					if(isset($this->_origin['netiface']) === true
					&& $this->_origin['netiface'] !== false)
						$this->_netiface->add_origin($this->_origin['netiface']);
					break;
			}
		}

		return(true);
	}

	function enable()
	{
		if(isset($this->_info['deletable']) === false)
			return(false);
		else if($this->_info['deletable'] === false)
			return(null);

		$r = true;

		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_netiface() === false)
			$r = false;
		else if($this->enable_sysconfnet() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';
    
        if($r === false)
            return false;
    
        return $this->update_commonconf();
	}

	function disable()
	{
		if(isset($this->_info['deletable']) === false)
			return(false);
		else if($this->_info['deletable'] === false)
			return(null);

		$r = true;

		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_netiface() === false)
			$r = false;
		else if($this->disable_sysconfnet() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

        if($r === false)
            return false;
    
        return $this->update_commonconf();
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

	function enable_sysconfnet()
	{
		return($this->_enable_disable_sysconfnet(false));
	}

	function disable_sysconfnet()
	{
		return($this->_enable_disable_sysconfnet(true));
	}

	function _enable_disable_sysconfnet($disable=false)
	{
		if(dwho_issa('netiface',$this->_info) === false)
			return(false);

		$r = (bool) $this->_sysconfd->request_post('/change_state_eth_ipv4',
							   array('state' => ($disable === false)),
							   array(array('ifname',$this->_info['netiface']['ifname'])));

		if($this->_sysconfd->last_status_code() === 404)
			$r = null;

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['sysconfnet'] = $r;

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
				case 'netiface':
					$this->_enable_disable_netiface($disable);
					break;
			}
		}

		return(true);
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
	
	/**
	    Send device network configuration IFF device is voip type
	    
	    Return true if sysconfd store values & regenerate common.conf correctly,
	    false otherwise
	*/
	function update_commonconf()
    {
        $list = $this->get_netifaces_list();
        
        // we filter network devices list, keeping only 'voip' interfaces:
        //  1. 1st vlan
        //  2. 1st voip iface
        //  3. voip ifaces list
        $vlanid     = 0;        //0 means no vlan
        $ifaces     = "";
        $cidr_id    = -1;
        
        for($i = 0; $i < count($list); $i++)
        {
            $item = $list[$i];
            if(!$item['actionable']
            || $item['netiface']['disable']
            || strcmp($item['netiface']['networktype'], 'voip') != 0)
                continue;
                
            $ifaces .= $item['netiface']['ifname'] . ' ';

            if($vlanid == 0 && $item['netinfo']['vlanif'])
                $vlanid = intval($item['netiface']['vlanid']);
                
            if($cidr_id < 0 && strcmp($item['netinfo']['method'], 'static') == 0)
                $cidr_id = $i;
        }

        $ifaces = trim($ifaces);
        
##        var_dump($list);
#        var_dump($ifaces);
#        var_dump($vlanid);
#        var_dump($cidr_id);
        
        $subnet = null;
        if($cidr_id >= 0)
        {
            $dotted_ip = split('\.', $list[$cidr_id]['netiface']['address']);
            $dotted_nm = split('\.', $list[$cidr_id]['netiface']['netmask']);
        
            $subnet = sprintf("%d.%d.%d.%d",
                (((int)$dotted_ip[0]) & ((int)$dotted_nm[0])),
                (((int)$dotted_ip[1]) & ((int)$dotted_nm[1])),
                (((int)$dotted_ip[2]) & ((int)$dotted_nm[2])),
                (((int)$dotted_ip[3]) & ((int)$dotted_nm[3]))
            );
        }
        
        $conf = array(
            'xivo.voip.ifaces'      => $ifaces,
            'xivo.voip.vlan.id'     => $vlanid,
            
            'xivo.net4.ip'          => ($cidr_id < 0)?null:$list[$cidr_id]['netiface']['address'],
            'xivo.net4.netmask'     => $cidr_id < 0?null:$list[$cidr_id]['netiface']['netmask'],
            'xivo.net4.broadcast'   => $cidr_id < 0?null:$list[$cidr_id]['netiface']['broadcast'],
            'xivo.net4.subnet'      => $subnet,
        );
        
#        var_dump($conf);
        if($this->_sysconfd->request_post('/commonconf_set', $conf) === false)
            return false;
            
        return $this->_sysconfd->request_get('/commonconf_generate');
    }
}

    ?>
