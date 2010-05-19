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

require_once(dwho_file::joinpath(XIVO_PATH_OBJECT,'abstract','acl.inc'));

class xivo_user_acl extends xivo_acl_abstract
{
	var $_conf	= array();
	var $_dso	= null;
	var $_param	= array('factory' => true);

	function xivo_user_acl(&$dso,$factory=true)
	{

		if(defined('XIVO_USER_ACL_FILE') === true)
			$path = XIVO_USER_ACL_FILE;
		else
			$path = dwho_file::joinpath(XIVO_PATH_OBJECTCONF,'acl','user');

		if(($this->_conf = dwho_gat::get_file($path)) === false
		|| isset($this->_conf['tree']) === false)
			trigger_error('Failed to load user ACL Array',E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_acl_tree = $this->_conf['tree'];
		$this->_dso = &$dso;

		$this->_param['factory'] = (bool) $factory;

		if($this->_param['factory'] === false)
			$this->load_in_session();
	}

	function load_in_session()
	{
		if(xivo_user::is_valid() === false)
			return(false);

		$hash = md5(serialize($this->_conf));

		//unset($_SESSION['_ACL']);

		if(isset($_SESSION['_ACL']) === false
		|| is_array($_SESSION['_ACL']) === false)
			$_SESSION['_ACL'] = array();

		if(isset($_SESSION['_ACL']['tree']) === false)
			$_SESSION['_ACL']['tree'] = array();

		if(dwho_issa('group',$this->_conf) === false)
			trigger_error('Missing group array in acl',E_USER_ERROR);
		else
			$_SESSION['_ACL']['group'] = $this->_conf['group'];

		if(($user = $this->get(xivo_user::get_info('id'))) === null)
			trigger_error('Missing or invalid user array in acl',E_USER_ERROR);
		else
		{
			$_SESSION['_ACL']['user'] = $user;
			$user_hash = md5(serialize($user));

			if(isset($_SESSION['_ACL']['user_hash']) === false
			|| $user_hash !== $_SESSION['_ACL']['user_hash'])
			{
				$_SESSION['_ACL']['user_section'] = array();
				$_SESSION['_ACL']['user_hash'] = $user_hash;
			}
		}

		if(isset($_SESSION['_ACL']['hash']) === false || $hash !== $_SESSION['_ACL']['hash'])
		{
			if(($tree = parent::get_full_tree()) === false)
				trigger_error('Invalid tree array in acl',E_USER_ERROR);
			else
			{
				$_SESSION['_ACL']['tree'] = $tree;
				$_SESSION['_ACL']['hash'] = $hash;
			}
		}
	}

	function get_full_tree()
	{
		if(is_array($_SESSION['_ACL']) === false
		|| isset($_SESSION['_ACL']['tree']) === false)
			return(false);

		return($_SESSION['_ACL']['tree']);
	}

	function get($id)
	{
		if(($user = $this->_dso->get($id)) === false)
			return(null);
		else if(isset($user['obj']['acl']) === true)
			return($user['obj']['acl']);
		else if(isset($this->_conf['group'][$user['meta']]) === true)
			return($this->_conf['group'][$user['meta']]);

		return(null);
	}

	function edit($arr)
	{
		if(is_array($arr) === false || isset($arr['id']) === false)
			return(false);
		else if(isset($arr['tree']) === false)
			$arr['tree'] = array();

		return($this->_dso->acl_edit($arr['id'],$this->build_tree_by_path($arr['tree'])));
	}
}

?>
