<?php

class xivo_user
{
	var $_ini	= array();
	var $_conf	= array();

	function xivo_user()
	{
		$r = false;

		$this->_conf = &xivo_gat::get('user');

		$ini = &xivo_user::load_get_init();

		if(xivo_issa('general',$ini) === false)
			trigger_error('Invalid user ini file',E_USER_ERROR);

		$this->_ini = &$ini;

		$this->_chk_ini_general();

		if(isset($_SESSION['_USR']) === false)
			$_SESSION['_USR'] = (object) null;

		if(isset($_SESSION['_USR']->_type) === false || xivo_user::chk_type($_SESSION['_USR']->_type) === false)
			$_SESSION['_USR']->_type = 0;

		$meta = xivo_user::type_to_meta($_SESSION['_USR']->_type);

		if(($class = $this->load_class($meta)) === false)
			trigger_error('Failed to load user class',E_USER_ERROR);

		xivo_gct::set(new $class(false),'_USR');
	}

	function _chk_ini_general()
	{
		$cfg_gen = &$this->_ini['general'];

		if(isset($cfg_gen['ip_check']) === true)
			$this->_conf['option']['ip_check'] = (bool) $cfg_gen['ip_check'];

		if(isset($cfg_gen['time_cache']) === true)
			$this->_conf['option']['time_cache'] = xivo_uint($cfg_gen['time_cache']);
	}

	function &load_get_init()
	{
		static $ini = null;

		if($ini === null)
		{
			if(($ini = xivo::load_init(XIVO_PATH_CONF.XIVO_SEP_DIR.'user.ini')) === false)
				trigger_error('User ini file not found or unreadable',E_USER_ERROR);
		}
		
		return(($ref = &$ini));
	}

	function get_infos($name)
	{
		$r = null;

		$name = (string) $name;

		if(isset($_SESSION['_USR'],$_SESSION['_USR']->{$name}) === true && $name{0} !== '_')
			$r = $_SESSION['_USR']->{$name};

		return($r);
	}

	function is_valid()
	{
		$r = false;

		if(isset($_SESSION['_USR'],$_SESSION['_USR']->_valid) === true && $_SESSION['_USR']->_valid === true)
			$r = true;

		return($r);
	}

	function chk_active()
	{
		$r = false;

		$cfg = xivo_gat::get('user');

		if(xivo_user::is_valid() === true)
		{
			$r = null;

			if($cfg['option']['time_cache'] > 0)
			{
				if(($cfg['option']['time_cache'] + $_SESSION['_USR']->_time) < gmmktime())
					$r = true;
			}
			else $r = true;
		}

		return($r);
	}

	function chk_type($type,$real=false)
	{
		$cfg = xivo_gat::get('user');
		$real = (bool) $real;

		$r = isset($cfg['type'][$type]);

		if($r === true && $real === true)
			return(($type > 0 ? true : false));

		return($r);
	}

	function chk_meta($meta,$real=false)
	{
		$r = false;

		if(($type = xivo_user::meta_to_type($meta)) !== false && ($meta = xivo_user::type_to_meta($type,$real)) !== false)
			$r = $meta;

		return($r);
	}

	function type_to_meta($type,$real=false)
	{
		$r = false;

		$cfg = xivo_gat::get('user');

		if(xivo_user::chk_type($type,$real) === true && isset($cfg['type'][$type]['meta']) === true)
			$r = $cfg['type'][$type]['meta'];

		return($r);
	}

	function meta_to_type($meta)
	{
		$r = false;

		$meta = strtoupper((string) $meta);

		if(defined('XIVO_USER__'.$meta) === true)
			$r = constant('XIVO_USER__'.$meta);

		return($r);
	}

	function chk_authorize($users,$meta='',$real=false)
	{
		$r = false;

		$users = (array) $users;

		if(xivo_user::is_valid() === false)
			return($r);

		if($meta === '')
			$meta = $_SESSION['_USR']->meta;

		if(xivo_user::chk_meta($meta) === false || ($arr = xivo_get_aks($users)) === false)
			return($r);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = &$arr['keys'][$i];
			$v = &$users[$k];

			if(($umeta = xivo_user::chk_meta($v,$real)) === false)
				break;

			if($meta === $umeta)
				return(true);
		}

		return($r);
	}

	function policy_chk($section)
	{
		$r = false;
	
		if(isset($_SESSION['_PLY']) === false || xivo_user::is_valid() === false)
			return($r);
			
		if(xivo_issa('tree',$_SESSION['_PLY']) === false || xivo_issa('user',$_SESSION['_PLY']) === false)
			return($r);

		$section = trim(preg_replace('@/+@','/',(string) $section),'/');

		$tree = $_SESSION['_PLY']['tree'];
		$user = $_SESSION['_PLY']['user'];

		if($user === false || $user === true)
			return($user);

		$arr = explode('/',$section);
		$nb = count($arr);

		for($i = 0;$i < $nb;$i++)
		{
			$k = &$arr[$i];
			$grp = $user;

			if(xivo_issa($k,$tree) === false || ($user = xivo_ak($k,$user,true)) === false)
				return($r);

			if($user === true)
				return(true);

			if(isset($tree[$k]['child']) === true)
				$tree = &$tree[$k]['child'];
		}

		if(xivo_ak($k,$grp,true) !== false)
			$r = true;

		return($r);
	}

	function logout()
	{
		session_unset();
	}

	function &factory($meta)
	{
		$r = false;

		if(($meta = xivo_user::chk_meta($meta)) === false)
			return($r);

		if(xivo_user::is_valid($meta) === true)
			return(xivo_user::get($meta));

		if(($class = xivo_user::load_class($meta)) === false)
			return($r);

		return(xivo_user::set_get($meta,new $class()));
	}

	function load_class($meta)
	{
		$r = false;

		if(($meta = xivo_user::chk_meta($meta)) === false)
			return($r);

		$class = 'xivo_user_'.$meta;

		if(xivo::load_class($class,'user',$meta) === false)
			trigger_error('Failed to load user class type: '.$meta,E_USER_ERROR);

		return($class);
	}

	function set($name,&$obj)
	{
		$r = false;

		if($name === '' || is_object($obj) === false)
			return($r);

		return(xivo_gct::set($obj,XIVO_USER_CLASS_PRE.$name));
	}

	function &get($name)
	{
		return(xivo_gct::get(XIVO_USER_CLASS_PRE.$name));
	}

	function &set_get($name,&$obj)
	{
		$r = false;

		if($name === '')
			return($r);

		return(xivo_gct::set_get($obj,XIVO_USER_CLASS_PRE.$name));
	}
/*
	function is_valid($name)
	{
		return(xivo_gct::is_valid(XIVO_USER_CLASS_PRE.$name));
	}
*/
}

?>
