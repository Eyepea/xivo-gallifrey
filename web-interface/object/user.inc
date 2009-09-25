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

class xivo_user
{
	var $_ini	= array();
	var $_conf	= array();

	function xivo_user()
	{
		$this->_conf = &dwho_gat::get('user');

		$ini = &xivo_user::load_get_init();

		if(dwho_issa('general',$ini) === false)
			trigger_error('Invalid user init file',E_USER_ERROR);

		$this->_ini = &$ini;

		$this->_chk_ini_general();

		if(isset($_SESSION['_USR']) === false)
			$_SESSION['_USR'] = (object) null;

		if(isset($_SESSION['_USR']->_type) === false
		|| xivo_user::chk_type($_SESSION['_USR']->_type) === false)
			$_SESSION['_USR']->_type = 0;

		$meta = xivo_user::type_to_meta($_SESSION['_USR']->_type);

		if(($class = $this->load_class($meta)) === false)
			trigger_error('Failed to load user class',E_USER_ERROR);

		dwho_gct::set(new $class(false),'_USR');
	}

	function _chk_ini_general()
	{
		$cfg_gen = &$this->_ini['general'];

		if(isset($cfg_gen['ip_check']) === true)
			$this->_conf['option']['ip_check'] = (bool) $cfg_gen['ip_check'];

		if(isset($cfg_gen['time_cache']) === true)
			$this->_conf['option']['time_cache'] = dwho_uint($cfg_gen['time_cache']);
	}

	function &load_get_init()
	{
		static $ini = null;

		$path = XIVO_PATH_CONF.DWHO_SEP_DIR.'user.ini';

		if($ini === null && ($ini = xivo::load_init($path)) === false)
			trigger_error('User init file not found or unreadable',E_USER_ERROR);

		return(($ref = &$ini));
	}

	function get_info($name)
	{
		$name = (string) $name;

		if(isset($_SESSION['_USR'],$_SESSION['_USR']->{$name}) === true && $name{0} !== '_')
			return($_SESSION['_USR']->{$name});

		return(null);
	}

	function is_valid()
	{
		if(isset($_SESSION['_USR'],$_SESSION['_USR']->_valid) === true
		&& $_SESSION['_USR']->_valid === true)
			return(true);

		return(false);
	}

	function chk_active()
	{
		$cfg = dwho_gat::get('user');

		if(xivo_user::is_valid() === false)
			return(false);
		else if($cfg['option']['time_cache'] <= 0
		|| ($cfg['option']['time_cache'] + $_SESSION['_USR']->_time) < gmmktime())
			return(true);

		return(null);
	}

	function chk_type($type,$real=false)
	{
		$cfg = dwho_gat::get('user');

		if(($r = isset($cfg['type'][$type])) === true
		&& (bool) $real === true)
			return(($type > 0));

		return($r);
	}

	function chk_meta($meta,$real=false)
	{
		if(($type = xivo_user::meta_to_type($meta)) !== false
		&& ($meta = xivo_user::type_to_meta($type,$real)) !== false)
			return($meta);

		return(false);
	}

	function type_to_meta($type,$real=false)
	{
		$cfg = dwho_gat::get('user');

		if(xivo_user::chk_type($type,$real) === true
		&& isset($cfg['type'][$type]['meta']) === true)
			return($cfg['type'][$type]['meta']);

		return(false);
	}

	function meta_to_type($meta)
	{
		$meta = strtoupper($meta);

		if(defined('XIVO_USER__'.$meta) === true)
			return(constant('XIVO_USER__'.$meta));

		return(false);
	}

	function chk_authorize($users,$meta='',$real=false)
	{
		$users = (array) $users;

		if(xivo_user::is_valid() === false
		|| empty($users) === true)
			return(false);
		else if($meta === '')
			$meta = $_SESSION['_USR']->meta;

		if(xivo_user::chk_meta($meta) === false)
			return(false);

		foreach($users as $value)
		{
			if(($umeta = xivo_user::chk_meta($value,$real)) === false)
				break;
			else if($meta === $umeta)
				return(true);
		}

		return(false);
	}

	function chk_acl($category='',$subcategory='',$section='')
	{
		xivo::load_class('xivo_user_acl',XIVO_PATH_OBJECT.DWHO_SEP_DIR.'user','acl',false);

		$section = xivo_user_acl::get_section($category,$subcategory,$section);

		return(xivo_user::chk_acl_section($section));
	}

	function chk_acl_section($section)
	{
		if(isset($_SESSION['_ACL']) === false
		|| xivo_user::is_valid() === false
		|| dwho_issa('tree',$_SESSION['_ACL']) === false
		|| isset($_SESSION['_ACL']['user'],
			 $_SESSION['_ACL']['user_section']) === false
		|| is_scalar($section) === false)
			return(false);

		$section = trim(preg_replace('@/+@','/',$section),'/');

		$tree = $_SESSION['_ACL']['tree'];
		$user = $_SESSION['_ACL']['user'];
		$user_section = &$_SESSION['_ACL']['user_section'];

		if(is_array($user_section) === false)
			$user_section = array();
		else if(isset($user_section[$section]) === true)
			return((bool) $user_section[$section]);

		$user_section[$section] = false;
		$r = &$user_section[$section];

		if($user === true)
			$r = true;
		else if(is_array($user) === true)
			$r = false;
		else
			return(false);

		$arr = explode('/',$section);
		$nb = count($arr);

		for($i = 0;$i < $nb;$i++)
		{
			$key = &$arr[$i];
			$grp = $user;

			if(dwho_issa($key,$tree) === false)
				return(($r = false));
			else if($r === false && is_bool(($user = dwho_ak($key,$user,true))) === true)
			{
				if(isset($tree[$key]['value']) === true
				&& (bool) $tree[$key]['value'] === false)
					return(($r = false));

				return(($r = $user));
			}
			else if(isset($tree[$key]['child']) === true)
				$tree = &$tree[$key]['child'];
		}

		if($r === false && isset($key,$grp) === true)
			$r = dwho_ak($key,$grp,true) !== false;

		return($r);
	}

	function logoff()
	{
		session_unset();
	}

	function &factory($meta)
	{
		if(($meta = xivo_user::chk_meta($meta)) === false)
			return(false);
		else if(xivo_user::is_valid($meta) === true)
			return(xivo_user::get($meta));
		else if(($class = xivo_user::load_class($meta)) === false)
			return(false);

		return(($ref = &xivo_user::set_get($meta,new $class())));
	}

	function load_class($meta)
	{
		if(($meta = xivo_user::chk_meta($meta)) === false)
			return(false);

		$class = 'xivo_user_'.$meta;
		$dir = XIVO_PATH_OBJECT.DWHO_SEP_DIR.'user';

		if(xivo::load_class($class,$dir,$meta,false) === false)
			trigger_error('Failed to load user class type: '.$meta,E_USER_ERROR);

		return($class);
	}

	function set($name,&$obj)
	{
		if($name === '' || is_object($obj) === false)
			return(false);

		return(dwho_gct::set($obj,XIVO_USER_CLASS_PRE.$name));
	}

	function &get($name)
	{
		return(($ref = &dwho_gct::get(XIVO_USER_CLASS_PRE.$name)));
	}

	function &set_get($name,&$obj)
	{
		$r = false;

		if($name !== '')
			$r = &dwho_gct::set_get($obj,XIVO_USER_CLASS_PRE.$name);

		return(($ref = &$r));
	}
}

?>
