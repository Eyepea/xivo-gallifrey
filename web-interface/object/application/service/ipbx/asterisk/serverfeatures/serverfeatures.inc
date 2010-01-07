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

class xivo_application_service_asterisk_serverfeatures extends xivo_application_asterisk_abstract
{
	var $_serverfeatures	= false;
	var $_server		= false;
	var $_param		= array('feature'	=> '',
					'type'		=> '');

	function xivo_application_service_asterisk_serverfeatures(&$service,$param=array())
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_serverfeatures = &$this->_service->get_module('serverfeatures')) === false)
			return(false);

		return($this->load_init($param));
	}

	function load_init($param=array())
	{
		if($this->set_params($param) === true
		&& isset($param['feature'],$param['type']) === true
		&& $this->_serverfeatures->chk_element_value('feature',$param['feature']) !== false
		&& $this->_serverfeatures->chk_element_value('type',$param['type']) !== false)
		{
			if($this->_param['type'] === 'xivo'
			&& xivo::load_class('xivo_server',XIVO_PATH_OBJECT,null,false) !== false
			&& ($this->_server = new xivo_server()) !== false)
				return(true);
			else if($this->_param['type'] === 'ldap'
			&& ($this->_server = $this->_service->get_application('ldapfilter')) !== false)
				return(true);
		}

		$this->_param['feature'] = $this->_param['type'] = '';

		return(false);
	}

	function get()
	{
		$this->_info = false;
		$this->_origin = false;

		$where = array();
		$where['feature'] = $this->_param['feature'];
		$where['type'] = $this->_param['type'];

		if(($this->_info = $this->_serverfeatures->get_all_where($where)) === false)
			return(false);

		$this->_origin = $this->_serverfeatures->get_origin_list();

		return($this->_info);
	}

	function get_server_list($disable=null,$order=null,$limit=null)
	{
		if($this->_param['type'] === 'xivo')
			return($this->_server->get_all($disable,true,$order,$limit,true));
		else if($this->_param['type'] === 'ldap')
			return($this->_server->get_ldapfilters_list($disable,$order,$limit,true));

		return(false);
	}

	function get_servers_list($disable=null,$opts=null,$assoc=false)
	{
		if(is_array($opts) === true)
			$opts = array_merge($opts,$this->_param);
		else
			$opts = $this->_param;

		if(($rs = $this->_serverfeatures->get_all_where($opts,$disable)) === false
		|| ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['serverfeatures'] = $rs[$i];

			if(($info['server'] = $this->_server->get($info['serverfeatures']['serverid'],$disable)) !== false)
				$r[$info['serverfeatures']['id']] = $info;
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function get_elements()
	{
		return($this->_serverfeatures->get_element());
	}

	function set($list)
	{
		$this->_reset();

		if(is_array($list) === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === 0)
			return(null);

		$info = $this->_param;

		$tmp = array();

		for($i = $j = $k = 0;$i < $nb;$i++)
		{
			if(($info['serverid'] = $this->_server->get_primary($list[$i])) === false
			|| ($rs = $this->_serverfeatures->chk_values($info)) === false
			|| isset($tmp[$rs['serverid']]) === true)
			{
				$this->_set_error($j++,'wrong server');
				continue;
			}

			$tmp[$rs['serverid']] = 1;
			$this->_set_result($k++,$rs);
		}

		if($this->get_errnb() > 0)
			return(false);

		return(true);
	}

	function add()
	{
		$this->_status = 'add';

		if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);

		$this->_return = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if(($this->_return[] = $this->_serverfeatures->add($rs[$i])) !== false)
				continue;
			else if($this->_status === 'add')
				$this->_reverse_add();
			else if($this->_status === 'edit' || $this->_status === 'save')
				$this->_reverse_edit();

			return(false);
		}

		if(isset($this->_return[0]) === false)
			$this->_return = false;

		return(true);
	}

	function edit()
	{
		$this->_status = 'edit';

		return($this->_edit_save());
	}

	function save()
	{
		$this->_status = 'save';

		return($this->_edit_save());
	}

	function _edit_save()
	{
		if(empty($this->_info) === true)
			return($this->add());

		$this->_return = $this->delete();

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

			if(($this->_return[] = $this->_serverfeatures->add($rs[$i],$aid)) === false)
				return(false);
		}

		if(isset($this->_return[0]) === false)
			$this->_return = false;

		return(true);
	}

	function delete()
	{
		$this->_status = 'delete';

		if(empty($this->_info) === true)
			return(null);

		$where = array();
		$where['feature'] = $this->_param['feature'];
		$where['type'] = $this->_param['type'];

		if(($r = $this->_serverfeatures->delete_where($where)) === false)
			$this->_reverse_delete();

		if($this->_status === 'delete')
			$this->_return = $r;

		return($r);
	}

	function _reverse_add()
	{
		if(is_array($this->_return) === false || ($nb = count($this->_return)) === 0)
			return(null);

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_return[$i] !== false)
				$this->_serverfeatures->delete($this->_return[$i]);
		}

		return(true);
	}

	function _reverse_edit()
	{
		$this->delete();

		return($this->_serverfeatures->add_origin_list());
	}

	function _reverse_delete()
	{
		return($this->_serverfeatures->add_origin_list());
	}
}

?>
