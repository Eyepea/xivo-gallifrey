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

require_once(dwho_file::joinpath(XIVO_PATH_OBJECT,'abstract','libdatastorage.inc'));

class xivo_accesswebservice extends xivo_libdatastorage_abstract
{
	var $_conf		= array();
	var $_dso		= null;
	var $_acl		= null;
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_accesswebservice()
	{
		$this->_load_config(dirname(__FILE__).DWHO_SEP_DIR.'accesswebservice');
		$this->_load_dso();
		dwho::load_class('dwho_filter');
	}

	function _prepare($data)
	{
		$data['disable'] = (bool) $data['disable'];

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['obj']) === false)
			$arr['obj'] = '';

		return(parent::add($arr,$id));
	}

	function _load_dso()
	{
		$dso = &dwho_gct::get('DWHO_DSO');
		$type = $dso->get_type();
		$class = 'xivo_accesswebservice_'.$type;
		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,'accesswebservice','datastorage');

		if(xivo::load_class($class,$dir,$type,false) === false)
		{
			trigger_error('Failed to load datastorage '.$type.' for accesswebservice',E_USER_ERROR);
			return(false);
		}

		$this->_dso = new $class($dso);
		return(true);
	}

	function chk_http_access($category='',$subcategory='',$section='')
	{
		if(isset($_SERVER['REMOTE_ADDR']) === true)
			$host = $_SERVER['REMOTE_ADDR'];
		else
			$host = null;

		if(isset($_SERVER['PHP_AUTH_USER'],$_SERVER['PHP_AUTH_PW']) === true)
			$id = $this->chk_user_access($_SERVER['PHP_AUTH_USER'],
						     $_SERVER['PHP_AUTH_PW'],
						     $host);
		else
			$id = $this->chk_host_access($host);

		if($id === false)
			return(null);

		return($this->_chk_acl($id,$category,$subcategory,$section));
	}

	function chk_access($user=null,$passwd=null,$host=null,$category='',$subcategory='',$section='')
	{
		if($user !== null && ($id = $this->chk_user_access($user,$passwd,$host)) === false)
			return(false);
		else if(($id = $this->chk_host_access($host)) === false)
			return(false);

		return($this->_chk_acl($id,$category,$subcategory,$section));
	}

	function chk_user_access($user,$passwd,$host=null)
	{
		if(dwho_has_len($user) === false
		|| dwho_has_len($passwd) === false)
			return(false);

		$where = array();
		$where['user'] = $user;
		$where['passwd'] = $passwd;

		if(($rs = $this->get_all_where($where,false,false)) === false
		|| ($nb = count($rs)) === 0)
			return(false);

		for($i = 0;$i < $nb;$i++)
		{
			if($user !== $rs[$i]['login'] || $passwd !== $rs[$i]['passwd'])
				continue;
			else if(dwho_has_len($rs[$i]['host']) === false
			|| dwho_filter::chk_ipv4_in_lhost($host,$rs[$i]['host']) === true)
				return($rs[$i]['id']);
		}

		return(false);
	}

	function chk_host_access($host)
	{
		if(dwho_has_len($host) === false
		|| ($rs = $this->get_all(false,false)) === false
		|| ($nb = count($rs)) === 0)
			return(false);

		for($i = 0;$i < $nb;$i++)
		{
			if(dwho_filter::chk_ipv4_in_lhost($host,$rs[$i]['host']) === true
			&& dwho_has_len($rs[$i]['login']) === false
			&& dwho_has_len($rs[$i]['passwd']) === false)
				return($rs[$i]['id']);
		}

		return(false);
	}

	function _chk_acl($id,$category='',$subcategory='',$section='')
	{
		$this->_load_acl();

		$section = $this->_acl->get_section($category,$subcategory,$section);

		return($this->_acl->_chk_section($section,$id));
	}

	function _load_acl()
	{
		if(is_object($this->_acl) === true)
			return(true);
		else if(xivo::load_class('xivo_accesswebservice_acl',
					 XIVO_PATH_OBJECT.DWHO_SEP_DIR.'accesswebservice',
					 'acl',
					 false) === false)
			trigger_error('Failed to load web services ACL',E_USER_ERROR);

		$this->_acl = new xivo_accesswebservice_acl($this->_dso);
	}

	function get_acl()
	{
		$this->_load_acl();

		return($this->_acl);
	}
}

?>
