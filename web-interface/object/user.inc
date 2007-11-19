<?php

class xivo_user
{
	var $_ini	= array();
	var $_conf	= array();

	function xivo_user()
	{
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

	function get_info($name)
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
		$cfg = xivo_gat::get('user');

		if(xivo_user::is_valid() === false)
			return(false);

		if($cfg['option']['time_cache'] <= 0
		|| ($cfg['option']['time_cache'] + $_SESSION['_USR']->_time) < gmmktime())
			$r = true;
		else
			$r = null;

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
		$users = (array) $users;

		if(xivo_user::is_valid() === false)
			return(false);

		if($meta === '')
			$meta = $_SESSION['_USR']->meta;

		if(xivo_user::chk_meta($meta) === false || ($arr = xivo_get_aks($users)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = &$arr['keys'][$i];
			$v = &$users[$k];

			if(($umeta = xivo_user::chk_meta($v,$real)) === false)
				break;

			if($meta === $umeta)
				return(true);
		}

		return(false);
	}

	function acl_chk($section)
	{
		if(isset($_SESSION['_ACL']) === false
		|| xivo_user::is_valid() === false
		|| xivo_issa('tree',$_SESSION['_ACL']) === false
		|| isset($_SESSION['_ACL']['user']) === false)
			return(false);
			
		$section = trim(preg_replace('@/+@','/',(string) $section),'/');

		$tree = $_SESSION['_ACL']['tree'];
		$user = $_SESSION['_ACL']['user'];

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
			$k = &$arr[$i];
			$grp = $user;

			if(xivo_issa($k,$tree) === false)
				return(false);

			if($r === false && is_bool(($user = xivo_ak($k,$user,true))) === true)
				return($user);

			if(isset($tree[$k]['child']) === true)
				$tree = &$tree[$k]['child'];
		}

		if($r === false && xivo_ak($k,$grp,true) !== false)
			$r = true;

		return($r);
	}

	function logoff()
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

		return(($ref = &xivo_user::set_get($meta,new $class())));
	}

	function load_class($meta)
	{
		if(($meta = xivo_user::chk_meta($meta)) === false)
			return(false);

		$class = 'xivo_user_'.$meta;
		$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.'user';

		if(xivo::load_class($class,$dir,$meta,false) === false)
			trigger_error('Failed to load user class type: '.$meta,E_USER_ERROR);

		return($class);
	}

	function set($name,&$obj)
	{
		if($name === '' || is_object($obj) === false)
			return(false);

		return(xivo_gct::set($obj,XIVO_USER_CLASS_PRE.$name));
	}

	function &get($name)
	{
		return(($ref = &xivo_gct::get(XIVO_USER_CLASS_PRE.$name)));
	}

	function &set_get($name,&$obj)
	{
		$r = false;

		if($name !== '')
			$r = &xivo_gct::set_get($obj,XIVO_USER_CLASS_PRE.$name);

		return(($ref = &$r));
	}
}

?>
