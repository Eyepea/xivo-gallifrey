<?php

class xivo_user_abstract
{
	var $_dso	= null;
	var $_ini	= array();
	var $_info	= array();
	var $_acl	= null;

	function mk_active()
	{
		$r = true;

		if(xivo_user::is_valid() === false)
			return(false);

		$this->_chk_update();

		if(($active = xivo_user::chk_active()) !== null)
			return($active);

		if($this->call_auth('chk',array($_SESSION['_USR']->_login,$_SESSION['_USR']->_pass)) !== false)
			$_SESSION['_USR']->_time = gmmktime();
		else
			$r = false;

		return($r);
	}

	function _chk_update()
	{
		if(isset($_SESSION['_USR']->id,$_SESSION['_USR']->dupdate) === true
		&& $this->_dso->chk_update($_SESSION['_USR']->id,$_SESSION['_USR']->dupdate) === true)
			$this->load_by_id($_SESSION['_USR']->id);
	}

	function get($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->get($id));
	}

	function get_nb($disable=null)
	{
		return($this->_dso->get_nb($disable));
	}

	function get_info($name)
	{
		$r = null;

		$name = (string) $name;

		if(isset($this->_info[$name]) === true && $name{0} !== '_')
			$r = $this->_info[$name];

		return($r);
	}

	function set_info($name,$value)
	{
		$name = (string) $name;

		if($name{0} !== '_')
			$this->_info[$name] = $value;
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
		$r = false;

		if(($data = $this->get_by_id($id)) !== false)
		{
			$r = true;
			if(isset($data['valid']) === true)
				$valid = $data['valid'];
			else
				$valid = false;

			$this->_set_info($valid,$data);
		}

		return($r);
	}

	function load_by_authent($login,$pass)
	{
		$r = false;

		$infos = array();

		if(($type = $this->call_auth('chk',array($login,$pass))) !== false && xivo_user::chk_type($type,true) === true)
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

		$this->_set_info($r,$infos);

		return($r);
	}

	function _set_info($valid,$arr=array())
	{
		$valid = (bool) $valid;

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

		if($valid === true)
			$this->_info['_valid'] = true;
		else
			$this->_info['_valid'] = false;

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

		xivo_i18n::set_locale($lang,$territo);

		$this->_info['language'] = xivo_i18n::attrib_language();
		$this->_info['territory'] = xivo_i18n::attrib_territory();
		$this->_info['locale'] = xivo_i18n::attrib_locale();

		if(defined('XIVO_USER_I18N_LANGUAGE') === false)
			define('XIVO_USER_I18N_LANGUAGE',$this->_info['language']);

		if(defined('XIVO_USER_I18N_TERRITORY') === false)
			define('XIVO_USER_I18N_TERRITORY',$this->_info['territory']);

		if(defined('XIVO_USER_I18N_LOCALE') === false)
			define('XIVO_USER_I18N_LOCALE',$this->_info['locale']);
	}

	function _add_by_login_pass($login,$pass,$type,$valid=true)
	{
		$valid = (bool) $valid;

		if(($meta = xivo_user::type_to_meta($type)) === false)
			return(false);

		$arr = array();
		$arr['login'] = $login;
		$arr['passwd'] = $pass;
		$arr['meta'] = $meta;
		$arr['time'] = gmmktime();
		$arr['valid'] = $valid === false ? false : true;

		return($this->_dso->add($arr));
	}

	function get_by_login_pass($login,$pass)
	{
		$r = $this->_dso->get_by_login_pass($login,$pass);

		if(is_object($r) === true)
			$r = (array) $r;
		else if(is_array($r) === false)
			$r = false;

		return($r);
	}

	function get_by_id($id)
	{
		$r = $this->_dso->get($id);

		if(is_object($r) === true)
			$r = (array) $r;
		else if(is_array($r) === false)
			$r = false;

		return($r);
	}

	function _load_dso()
	{
		if(xivo_gct::is_valid('XIVO_USER_DSO') === true)
		{
			$this->_dso = &xivo_gct::get('XIVO_USER_DSO');
			return(true);
		}

		$ini = xivo_user::load_get_init();
		$datastorage = null;

		if(isset($ini['datastorage']) === true)
			$datastorage = &$ini['datastorage'];

		$dso = &xivo_gct::get('XIVO_DSO');
		$type = $dso->get_type();
		$name = 'xivo_user_'.$type;
		$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.'user'.XIVO_SEP_DIR.'datastorage';

		if(xivo::load_class($name,$dir,$type,false) === false)
		{
			trigger_error('Failed to load user Datastorage',E_USER_ERROR);
			return(false);
		}

		if(isset($ini['datastorage']) === true)
			$datastorage = &$ini['datastorage'];

		$this->_dso = &xivo_gct::set_get(new $name($dso,$datastorage),'XIVO_USER_DSO');

		return(true);
	}

	function _load_filter($name,$user)
	{
		if(xivo_user::chk_meta($user) === false
		|| ($meta = xivo_user::type_to_meta($this->_info['_type'])) === false
		|| ($filter = xivo::load_array(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'user'.XIVO_SEP_DIR.'filter'.XIVO_SEP_DIR.$meta.'.inc')) === false
		|| xivo::load_class('xivo_filter') === false
		|| xivo_issa($user,$filter) === false
		|| xivo_issa($name,$filter[$user]) === false)
			return(false);

		return($filter[$user][$name]);
	}

	function _load_acl()
	{
		$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.'user';

		if(xivo::load_class('xivo_user_acl',$dir,'acl',false) === false)
			trigger_error('Failed to load user acl',E_USER_ERROR);

		$this->_acl = new xivo_user_acl($this->_dso,$this->_param['factory']);
	}

	function call_acl($func,$args=null)
	{
		$func = (string) $func;
		$acl_func = 'acl_'.$func;

		if(is_object($this->_acl) === false)
		{
			if(($this->_acl = $this->load_acl($this->_dso,$this->_param['factory'])) === false)
				trigger_error('Failed to load user acl in '.__CLASS__,E_USER_ERROR);
		}

		if(is_object($this->_acl) === false)
			trigger_error('Invalid user acl object',E_USER_ERROR);

		if(is_callable(array(&$this->_acl,$func)) === false)
			trigger_error('Invalid user acl method',E_USER_ERROR);

		if(method_exists($this,$acl_func) === false)
			$acl = &$this->_acl;
		else
		{
			$acl = &$this;
			$func = $acl_func;
		}

		if($args === null)
			$r = call_user_func(array($acl,$func));
		else
		{
			$args = (array) $args;
			$r = call_user_func_array(array($acl,$func),$args);
		}

		return($r);
	}

	function load_authent()
	{
		$ini = xivo_user::load_get_init();

		if(xivo_issa('authent',$ini) === false)
			return(false);

		$authent = &$ini['authent'];

		if(isset($authent['type']) === false)
			trigger_error('Missing type user authentification in user ini file',E_USER_ERROR);

		$type = $authent['type'];

		if($type !== 'internal')
			return(false);

		$dso = &xivo_gct::get('XIVO_DSO');
		$type = $dso->get_type();
		$name = 'xivo_authent_'.$type;
		$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.'user'.XIVO_SEP_DIR.'authent';

		if(xivo::load_class($name,$dir,$type,false) === false)
			trigger_error('Failed to load user authentification',E_USER_ERROR);

		return(new $name($dso,$authent));
	}

	function call_auth($func,$args=null)
	{
		$func = (string) $func;
		$auth_func = 'auth_'.$func;

		if($this->_param['factory'] === true)
			trigger_error('Unable to call authentification with factory',E_USER_ERROR);

		if(is_object($this->_authent) === false)
		{
			if(($this->_authent = $this->load_authent()) === false)
				trigger_error('Failed to load user authentification in '.__CLASS__,E_USER_ERROR);
		}

		if(is_object($this->_authent) === false)
			trigger_error('Invalid user authentification object',E_USER_ERROR);

		if(is_callable(array(&$this->_authent,$func)) === false)
			trigger_error('Invalid user authentification method',E_USER_ERROR);

		if(method_exists($this,$auth_func) === false)
			$auth = &$this->_authent;
		else
		{
			$auth = &$this;
			$func = $auth_func;
		}

		if($args === null)
			$r = call_user_func(array($auth,$func));
		else
		{
			$args = (array) $args;
			$r = call_user_func_array(array($auth,$func),$args);
		}

		return($r);
	}

	function auth_chk($login,$pass)
	{
		$r = false;

		if(($type = xivo_user::meta_to_type($this->_authent->chk($login,$pass,true))) > 0)
			$r = $type;

		return($r);
	}
}

?>
