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

class xivo_application_xivo_object_iproute extends xivo_application_xivo_object_abstract
{
	var $_iproute	= false;
	var $_netiface	= false;
	var $_sysconfd	= false;

	function xivo_application_xivo_object_iproute(&$xobj,&$dso)
	{
		$this->xivo_application_xivo_object_abstract(&$xobj);

		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if(($this->_iproute = &$this->_xobj->get_module('iproute')) === false
		|| ($this->_sysconfd = &$this->_xobj->get_module('sysconfd')) === false
		|| ($this->_netiface = &$this->_xobj->get_module('netiface')) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['iproute'] = $this->_iproute->get($id)) === false)
			return(false);

		$this->_origin['iproute'] = $this->_iproute->get_origin();

		return($this->_info);
	}

	function get_elements()
	{
		$r = array();
		$r['iproute'] = $this->_iproute->get_element();

		return($r);
	}

	function get_interfaces_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		if(is_object($this->_netiface) === false)
			return(false);

		return($this->_netiface->get_all_where(
						array('hwtypeid' => 1),
						$disable,
						true,
						$order,
						$limit,
						$assoc));
	}

	function get_iproutes_search($search,$disable=null,$order=null,$limit=null,$assoc=false,$linked=true)
	{
		return($this->_get_iproutes('search',$search,$disable,$order,$limit,$assoc,$linked));
	}

	function get_iproutes_list($disable=null,$order=null,$limit=null,$assoc=false,$linked=true)
	{
		return($this->_get_iproutes('list',null,$disable,$order,$limit,$assoc,$linked));
	}

	function _get_iproutes($action,$search='',$disable=null,$order=null,$limit=null,$assoc=false,$linked=true)
	{
		$search = strval($search);
		$linked = (bool) $linked;

		if($linked === true && is_object($this->_netiface) === false)
		{
			$this->_cnt = 0;
			return(false);
		}

		switch($action)
		{
			case 'search':
				$rs = $this->_dso->search($search,$disable,$order,$limit);
				$this->_cnt = $this->_dso->get_cnt();
				break;
			case 'list':
			default:
				$action = 'list';

				$rs = $this->_iproute->get_all($disable,true,$order,$limit);
				$this->_cnt = $this->_iproute->get_cnt();
		}

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$r[$rs[$i]['id']] = $rs[$i];

			$ref = &$r[$rs[$i]['id']];
			$ref['disable'] = (bool) $ref['disable'];

			if($linked === true)
			{
				$ref['linked'] = $this->_netiface->exists(
								array('name'		=> $rs[$i]['iface'],
								      'hwtypeid'	=> 1));
			}
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));
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

		$this->set_iproute($arr);

		return(($this->get_errnb() < 1));
	}

	function set_iproute($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('iproute',null);
			$this->_set_error('iproute','invalid data');
			return(false);
		}

		$arr['disable'] = false;

		if(($rs = $this->_iproute->chk_values($arr)) === false)
		{
			$this->_set_result('iproute',$this->_iproute->get_filter_result());
			$this->_set_error('iproute',$this->_iproute->get_filter_error());
			return(false);
		}
		else if(($normalize = $this->_normalize_iproute($rs)) === false)
		{
			$this->_set_result('iproute',$rs);
			$this->_set_error('iproute','iproute not valid');
			return(false);
		}

		$this->_set_result('iproute',$normalize);

		return(true);
	}

	function _normalize_iproute($arr)
	{
		if(is_array($arr) === false
		|| dwho_has_len($arr,'netmask') === false
		|| ($calc = dwho_network::ipv4_calc($arr['destination'],
						    $arr['netmask'])) === false)
			return(false);

		$arr['destination'] = $calc['network'];

		return($arr);
	}

	function is_valid_iproute($iface,$destination,$netmask,$id=null)
	{
		if($this->_netiface->exists(array('name'	=> $iface,
						  'hwtypeid'	=> 1)) === false)
			return(false);

		$where = array();
		$where['destination'] = $destination;
		$where['netmask'] = $netmask;

		if($id === null)
			return(($this->_iproute->exists($where) === false));
		else if(($primary = $this->_iproute->get_primary($where)) === false
		|| dwho_ulongint($id) === dwho_ulongint($primary))
			return(true);

		return(false);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->add_iproute() === false
		|| $this->commit_sysconfnet('add') === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function add_iproute()
	{
		if(($rs = $this->get_result('iproute')) === null
		|| $rs === false
		|| $this->is_valid_iproute($rs['iface'],
					   $rs['destination'],
					   $rs['netmask']) === false)
			return(false);

		$this->_return['iproute'] = $this->_iproute->add($rs);

		return($this->_return['iproute']);
	}

	function edit()
	{
		$r = true;

		$this->_status = 'edit';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_iproute() === false
		|| $this->commit_sysconfnet('edit') === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_iproute()
	{
		if(dwho_issa('iproute',$this->_info) === false
		|| ($rs = $this->get_result('iproute')) === null
		|| $rs === false
		|| $this->is_valid_iproute($rs['iface'],
					   $rs['destination'],
					   $rs['netmask'],
					   $this->_info['iproute']['id']) === false)
			return(false);

		$rs['disable'] = $this->_info['iproute']['disable'];

		$this->_return['iproute'] = $this->_iproute->edit($this->_info['iproute']['id'],$rs);

		return($this->_return['iproute']);
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
				case 'iproute':
					if(isset($this->_origin['iproute']) === false
					|| $this->_origin['iproute'] === false)
						$this->delete_iproute();
					else
						$this->_iproute->edit_origin($this->_origin['iproute']);
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

		if($this->delete_iproute() === false
		|| $this->commit_sysconfnet('del') === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function delete_iproute()
	{
		if(dwho_issa('iproute',$this->_info) === false)
			return(false);

		$r = $this->_iproute->delete($this->_info['iproute']['id']);

		if($this->_status === 'delete')
			$this->_return['iproute'] = $r;

		return($r);
	}

	function enable()
	{
		$r = true;

		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_iproute() === false
		|| $this->commit_sysconfnet('del') === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function disable()
	{
		$r = true;

		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_iproute() === false
		|| $this->commit_sysconfnet('del') === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function enable_iproute()
	{
		return($this->_enable_disable_iproute(false));
	}

	function disable_iproute()
	{
		return($this->_enable_disable_iproute(true));
	}

	function commit_sysconfnet($mode)
	{
		if(($list = $this->get_iproutes_list(null,$order,$limit)) === false)
			$list = array();
		
		$curroute = $this->_info['iproute'];
		foreach($list as &$route)
		{
			$route['current'] = ($route['id'] == $curroute['id']);
			// we need system network interface name, not database name
			$iface            = $this->_netiface->get($route['iface']);
			$route['iface']   = $iface['ifname'];
		}

		$ret = (bool) $this->_sysconfd->request_post('/routes', $list);
		return(($this->_return['sysconfnet'] = $ret));
	}

	function _enable_disable_iproute($disable=false)
	{
		if(dwho_issa('iproute',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_iproute->enable($this->_info['iproute']['id']);
		else
			$r = $this->_iproute->disable($this->_info['iproute']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['iproute'] = $r;

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
