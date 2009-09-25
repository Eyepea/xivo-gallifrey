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

class xivo_user_abstract extends xivo_libdatastorage_abstract
{
	var $_conf	= array();
	var $_dso	= null;
	var $_ini	= array();
	var $_info	= array();
	var $_acl	= null;

	function get_info($name)
	{
		$name = (string) $name;

		if(isset($this->_info[$name]) === true && $name{0} !== '_')
			return($this->_info[$name]);

		return(null);
	}

	function set_info($name,$value)
	{
		$name = (string) $name;

		if($name{0} !== '_')
			$this->_info[$name] = $value;
	}

	function _prepare($data)
	{
		$data['valid'] = (bool) $data['valid'];

		if(is_array($data['obj']) === false)
			$data['obj'] = dwho_unserialize($data['obj']);

		return($data);
	}

	function add($arr)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['login']) === true)
			$arr['login'] = strtolower($arr['login']);

		if(isset($arr['meta']) === true)
			$arr['meta'] = strtolower($arr['meta']);

		$arr['dcreate'] = gmmktime();
		$arr['dupdate'] = gmmktime();

		return(parent::add($arr));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['login']) === true)
			$arr['login'] = strtolower($arr['login']);

		if(isset($arr['meta']) === true)
			$arr['meta'] = strtolower($arr['meta']);

		$arr['dupdate'] = gmmktime();

		return(parent::edit($id,$arr,$disable));
	}

	function mk_active()
	{
		if(xivo_user::is_valid() === false)
			return(false);

		$this->_chk_update();

		if(($active = xivo_user::chk_active()) !== null)
			return($active);
		else if($this->call_auth('chk',array($_SESSION['_USR']->_login,
						     $_SESSION['_USR']->_pass)) === false)
			return(false);

		$_SESSION['_USR']->_time = gmmktime();

		return(true);
	}

	function _chk_update()
	{
		if(isset($_SESSION['_USR']->id,
			 $_SESSION['_USR']->dupdate) === true
		&& $this->_dso->chk_update($_SESSION['_USR']->id,
					   $_SESSION['_USR']->dupdate) === true)
			$this->load_by_id($_SESSION['_USR']->id);
	}

	function load_by_session()
	{
		if(($r = $this->mk_active()) === true)
			$this->_info = (array) $_SESSION['_USR'];

		$this->_set_info($r,$this->_info);

		return($r);
	}

	function load_by_id($id)
	{
		if(($data = $this->get($id)) === false)
			return(false);
		else if(isset($data['valid']) === true)
			$valid = $data['valid'];
		else
			$valid = false;

		$this->_set_info($valid,$data);

		return(true);
	}

	function load_by_authent($login,$pass,$lang='')
	{
		$r = false;

		$infos = array();

		if(($type = $this->call_auth('chk',array($login,$pass))) !== false
		&& xivo_user::chk_type($type,true) === true)
		{
			if(($data = $this->get_by_login_pass($login,$pass)) === false)
			{
				if(($r = $this->_add_by_login_pass($login,$pass,$type)) !== false)
				{
					$this->_info = array();
					$this->_info['id'] = $r;
					$r = true;
				}
			}
			else
			{
				$this->_info = &$data;
				$r = true;
			}

			if($r === true)
			{
				$this->_info['login'] = $login;
				$this->_info['passwd'] = $pass;
				$this->_info['type'] = $type;
				$infos = $this->_info;
			}
		}

		if(dwho_has_len($lang) === true)
			$infos['language'] = $lang;

		$this->_set_info($r,$infos);

		return($r);
	}

	function _set_info($valid,$arr=array())
	{
		if(is_array($arr) === true)
		{
			$this->_info = &$arr;

			if(isset($arr['type']) === false && isset($arr['meta']) === true)
				$arr['type'] = xivo_user::meta_to_type($arr['meta']);

			if(isset($arr['login'],$arr['passwd'],$arr['type']) === true)
			{
				$this->_info['_login'] = (string) $arr['login'];
				$this->_info['_pass'] = (string) $arr['passwd'];
				$this->_info['_type'] = (int) $arr['type'];
				$this->_info['_time'] = gmmktime();
			}
			else
				$this->_info['_valid'] = $valid = false;
		}

		$this->_info['_valid'] = (bool) $valid;

		$this->set_i18n();
		$_SESSION['_USR'] = (object) $this->_info;
	}

	function set_i18n()
	{
		$lang = $territo = false;

		if(isset($this->_info['language']) === true)
			$lang = $this->_info['language'];

		if(isset($this->_info['territory']) === true)
			$territo = $this->_info['territory'];

		dwho_i18n::set_locale($lang,$territo);

		$this->_info['language'] = dwho_i18n::attrib_language();
		$this->_info['territory'] = dwho_i18n::attrib_territory();
		$this->_info['locale'] = dwho_i18n::attrib_locale();

		if(isset($this->_info['_valid']) === true && $this->_info['_valid'] === true)
		{
			if(defined('DWHO_USER_I18N_LANGUAGE') === false)
				define('DWHO_USER_I18N_LANGUAGE',$this->_info['language']);

			if(defined('DWHO_USER_I18N_TERRITORY') === false)
				define('DWHO_USER_I18N_TERRITORY',$this->_info['territory']);

			if(defined('DWHO_USER_I18N_LOCALE') === false)
				define('DWHO_USER_I18N_LOCALE',$this->_info['locale']);
		}
	}

	function _add_by_login_pass($login,$pass,$type,$valid=true)
	{
		if(($meta = xivo_user::type_to_meta($type)) === false)
			return(false);

		$arr = array();
		$arr['login'] = $login;
		$arr['passwd'] = $pass;
		$arr['meta'] = $meta;
		$arr['time'] = gmmktime();
		$arr['valid'] = (bool) $valid;

		return($this->add($arr));
	}

	function get_by_login_pass($login,$pass)
	{
		$arr = array();
		$arr['login'] = strval($login);
		$arr['passwd'] = strval($pass);

		return($this->get_where($arr));
	}

	function _load_dso()
	{
		if(dwho_gct::is_valid('XIVO_USER_DSO') === true)
		{
			$this->_dso = &dwho_gct::get('XIVO_USER_DSO');
			return(true);
		}

		$ini = xivo_user::load_get_init();
		$datastorage = null;

		if(isset($ini['datastorage']) === true)
			$datastorage = &$ini['datastorage'];

		$dso = &dwho_gct::get('DWHO_DSO');
		$type = $dso->get_type();
		$name = 'xivo_user_'.$type;
		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,'user','datastorage');

		if(xivo::load_class($name,$dir,$type,false) === false)
		{
			trigger_error('Failed to load user Datastorage',E_USER_ERROR);
			return(false);
		}

		if(isset($ini['datastorage']) === true)
			$datastorage = &$ini['datastorage'];

		$this->_dso = &dwho_gct::set_get(new $name($dso,$datastorage),'XIVO_USER_DSO');

		return(true);
	}

	function _load_filter($name,$user)
	{
		if(xivo_user::chk_meta($user) === false
		|| ($meta = xivo_user::type_to_meta($this->_info['_type'])) === false
		|| ($filter = dwho_gat::get_file(dwho_file::joinpath(XIVO_PATH_OBJECT,
								     'user',
								     'filter',
								     $meta))) === false
		|| dwho::load_class('dwho_filter') === false
		|| dwho_issa($user,$filter) === false
		|| dwho_issa($name,$filter[$user]) === false)
			return(false);

		return($filter[$user][$name]);
	}

	function _load_acl()
	{
		if(is_object($this->_acl) === true)
			return(true);
		else if(xivo::load_class('xivo_user_acl',
					 XIVO_PATH_OBJECT.DWHO_SEP_DIR.'user',
					 'acl',
					 false) === false)
			trigger_error('Failed to load user ACL',E_USER_ERROR);

		$this->_acl = new xivo_user_acl($this->_dso,$this->_param['factory']);
	}

	function get_acl()
	{
		$this->_load_acl();

		return($this->_acl);
	}

	function load_authent()
	{
		$ini = xivo_user::load_get_init();

		if(dwho_issa('authent',$ini) === false)
			return(false);

		$authent = &$ini['authent'];

		if(isset($authent['type']) === false)
		{
			trigger_error('Missing type user authentication in user init file',E_USER_ERROR);
			return(false);
		}

		$type = $authent['type'];

		if($type !== 'internal')
			return(false);

		$dso = &dwho_gct::get('DWHO_DSO');
		$type = $dso->get_type();
		$name = 'xivo_authent_'.$type;
		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,'user','authent');

		if(xivo::load_class($name,$dir,$type,false) === false)
		{
			trigger_error('Failed to load user authentication',E_USER_ERROR);
			return(false);
		}

		return(new $name($dso,$authent));
	}

	function call_auth($func,$args=null)
	{
		$func = (string) $func;
		$auth_func = 'auth_'.$func;

		if($this->_param['factory'] === true)
		{
			trigger_error('Unable to call authentication with factory',E_USER_ERROR);
			return(false);
		}
		else if(is_object($this->_authent) === false
		&& ($this->_authent = $this->load_authent()) === false)
		{
			trigger_error('Failed to load user authentication in '.__CLASS__,E_USER_ERROR);
			return(false);
		}
		else if(is_object($this->_authent) === false)
		{
			trigger_error('Invalid user authentication object',E_USER_ERROR);
			return(false);
		}
		else if(is_callable(array(&$this->_authent,$func)) === false)
		{
			trigger_error('Invalid user authentication method',E_USER_ERROR);
			return(false);
		}
		else if(method_exists($this,$auth_func) === false)
			$auth = &$this->_authent;
		else
		{
			$auth = &$this;
			$func = $auth_func;
		}

		if($args === null)
			return(call_user_func(array($auth,$func)));

		return(call_user_func_array(array($auth,$func),(array) $args));
	}

	function auth_chk($login,$pass)
	{
		if(($type = xivo_user::meta_to_type($this->_authent->chk($login,$pass,true))) > 0)
			return($type);

		return(false);
	}
}

?>
