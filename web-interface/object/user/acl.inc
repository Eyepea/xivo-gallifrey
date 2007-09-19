<?php

class xivo_user_acl
{
	var $_conf	= array();
	var $_dso	= null;
	var $_param	= array('factory' => true);

	function xivo_user_acl(&$dso,$factory=true)
	{
		if(($this->_conf = xivo_gat::load_get('acl',XIVO_PATH_CONF)) === false)
			trigger_error('Failed to load user Array',E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_dso = &$dso;

		$factory = (bool) $factory;

		$this->_param['factory'] = $factory;

		if($factory === false)
			$this->load_in_session();
	}

	function load_in_session()
	{
		if(($r = xivo_user::is_valid()) === false)
			return($r);

		$hash = md5(serialize($this->_conf));

		//unset($_SESSION['_ACL']);
		
		if(xivo_issa('_ACL',$_SESSION) === false)
			$_SESSION['_ACL'] = array();

		if(isset($_SESSION['_ACL']['tree']) === false)
			$_SESSION['_ACL']['tree'] = array();

		if(xivo_issa('group',$this->_conf) === false)
			trigger_error('Missing group array in acl',E_USER_ERROR);
		else
			$_SESSION['_ACL']['group'] = $this->_conf['group'];

		if(($user = $this->get(xivo_user::get_info('id'))) === null)
			trigger_error('Missing or invalid user array in acl',E_USER_ERROR); 
		else
			$_SESSION['_ACL']['user'] = $user;

		if(isset($_SESSION['_ACL']['hash']) === false || $hash !== $_SESSION['_ACL']['hash'])
		{
			if(($tree = $this->_mk_tree($this->_conf['tree'],null)) === false)
				trigger_error('Invalid tree array in acl',E_USER_ERROR);
			else
			{
				$_SESSION['_ACL']['tree'] = $tree;
				$_SESSION['_ACL']['hash'] = $hash;
			}
		}
	}

	function _mk_tree($a,$sess)
	{
		$r = false;

		if(($arr = xivo_get_aks($a)) === false)
			return($r);

		$id = $path = '';
		$level = 1;
		$parent = false;

		if(xivo_issa('child',$sess) === true)
		{
			$level = $sess['level']+1;
			$id = $sess['id'];
			$path = $sess['path'];
			$parent = $sess;
			unset($parent['child'],$parent['parent']);
			$sess = &$sess['child'];
		}

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = $arr['keys'][$i];
			$v = &$a[$k];

			if(isset($sess[$k]) === false)
				$sess[$k] = array();

			$sess[$k]['name'] = $k;
			$sess[$k]['id'] = ($id !== '' ? $id.'-' : '').$k;
			$sess[$k]['path'] = ($path !== '' ? $path.'/' : '').$k; 
			$sess[$k]['level'] = $level;

			if($parent !== false)
				$sess[$k]['parent'] = $parent;

			if(is_array($v) === true)
			{
				$sess[$k]['child'] = array();
				$sess[$k]['child'] = $this->_mk_tree($v,$sess[$k]);
			}
			else
				$sess[$k]['value'] = $v;
		}

		return($sess);
	}

	function get_tree($section='',$cat='',$func='')
	{
		$section = (string) $section;
		$cat = (string) $cat;
		$func = (string) $func;

		$r = &$_SESSION['_ACL']['tree'];

		if($section === '')
			return($r);
/*
		if(isset($r[$section]) === true)
			$r = &$r[$section];
		else
			return(false);

		if(isset($r[$cat]) === true)
			$r = &$r[$cat];
		else
			return($r);

		if(isset($r[$func]) === true)
			$r = &$r[$func];
		else
			return($r);
*/
		return($r);
	}

	function get($id)
	{
		$r = null;

		if(($user = $this->_dso->get($id)) === false)
			return($r);

		if(isset($user['obj']['acl']) === true)
			$r = &$user['obj']['acl'];
		else if(isset($this->_conf['group'][$user['meta']]) === true)
			$r = $this->_conf['group'][$user['meta']];

		return($r);
	}

	function edit($arr)
	{
		$r = false;

		if(is_array($arr) === false || isset($arr['id']) === false)
			return($r);

		if(isset($arr['tree']) === false)
			$arr['tree'] = array();

		$r = $this->_dso->acl_edit($arr['id'],$this->_chk_tree($arr['tree']));

		return($r);
	}

	function get_user($id)
	{
		$r = false;

		if(($user = $this->get($id)) === null)
			return($r);

		$r = $this->_mk_user_tree($this->get_tree(),$user);

		return($r);
	}

	function _mk_user_tree($tree,$user)
	{
		if(($arr = xivo_get_aks($tree)) !== false)
		{
			for($i = 0;$i < $arr['cnt'];$i++)
			{
				$k = &$arr['keys'][$i];
				$v = &$tree[$k];

				$v['access'] = $this->chk($v['path'],$user);

				if(isset($v['child']) === true)
					$v['child'] = $this->_mk_user_tree($v['child'],$user);
			}
		}

		return($tree);
	}

	function chk($section,$user)
	{
		$r = false;

		if($user === null)
			return($r);

		if($user === false || $user === true)
			return($user);

		$section = trim(preg_replace('@/+@','/',(string) $section),'/');
		$tree = $this->get_tree();

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

	function _chk_tree($arr)
	{
		$r = array();

		$arr = array_values((array) $arr);

		$nb = count($arr);
		
		for($i = 0;$i < $nb;$i++)
		{
			if(($arr_path = $this->_chk_path($arr[$i],$this->_conf['tree'])) === false)
				continue;

			$cnt = count($arr_path)-1;

			$ref = &$r;

			for($j = 0;$j < $cnt;$j++)
			{
				if(isset($ref[$arr_path[$j]]) === false)
					$ref[$arr_path[$j]] = array();

				$ref = &$ref[$arr_path[$j]];
			}

			$ref[$arr_path[$j]] = true;
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}
/*
	function chk($section,$user='')
	{
		$r = false;

		$section = trim(preg_replace('@/+@','/',$section),'/');
		$user = (string) $user;
		
		if(isset($_SESSION['_ACL']) === false || xivo_user::is_valid() === false)
			return($r);

		if(xivo_issa('tree',$_SESSION['_ACL']) === false || xivo_issa('group',$_SESSION['_ACL']) === false)
			return($r);

		if($user === '')
			$user = xivo_user::type_to_meta($_SESSION['_USR']->_type);

		if(xivo_user::chk_meta($user) === false)
			return($r);
		
		if(xivo_issa('tree',$_SESSION['_ACL']) === false || xivo_issa('group',$_SESSION['_ACL']) === false)
			return($r);

		$tree = &$_SESSION['_ACL']['tree'];
		$group = &$_SESSION['_ACL']['group'];

		if(($ugroup = xivo_ak($user,$group,true)) === false)
			return($r);

		if($ugroup === true)
			return(true);

		$arr = explode('/',$section);
		$nb = count($arr);

		for($i = 0;$i < $nb;$i++)
		{
			$k = &$arr[$i];
			$grp = $ugroup;

			if(xivo_issa($k,$tree) === false || ($ugroup = xivo_ak($k,$ugroup,true)) === false)
				return($r);

			if($ugroup === true)
				return(true);

			if(isset($tree[$k]['child']) === true)
				$tree = &$tree[$k]['child'];
		}

		if(xivo_ak($k,$grp,true) !== false)
			return(true);
/*
		if(isset($tree[$cat]) === false)
			return($r);

		if(($ugroup = xivo_ak($cat,$ugroup,true)) === false)
			return($r);

		if($ugroup === true)
			return(true);

		$tree = &$tree[$cat]['child'];

		if(isset($tree[$func]) === false)
			return($r);

		if(($ugroup = xivo_ak($func,$ugroup,true)) === false)
			return($r);

		if($ugroup === true)
			return(true);
		return($r);
	}
*/
	function _chk_path($path,$tree)
	{
		$r = false;

		if(is_array($tree) === false)
			return($r);

		$path = (string) $path;

		$arr = explode('/',$path);
		$arr_path = $arr;

		if(($nb = count($arr)) === 0)
			return($r);

		for($i = 0;$i < $nb;$i++)
		{
			$k = &$arr[$i];

			if(xivo_ak($k,$tree) === false)
				return($r);

			$tree = &$tree[$k];
		}

		if(is_array($tree) === false)
			$r = &$arr_path;

		return($r);
	}
}

?>
