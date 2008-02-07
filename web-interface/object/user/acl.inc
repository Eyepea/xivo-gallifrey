<?php

class xivo_user_acl
{
	var $_conf	= array();
	var $_dso	= null;
	var $_param	= array('factory' => true);

	function xivo_user_acl(&$dso,$factory=true)
	{
		if(defined('XIVO_USER_ACL_PATH') === true)
			$path = XIVO_USER_ACL_PATH;
		else
			$path = XIVO_PATH_OBJECTCONF;

		if(($this->_conf = xivo_gat::load_get('acl',$path)) === false)
			trigger_error('Failed to load user Array',E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

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
		if(is_array($a) === false || empty($a) === true)
			return(false);

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

		foreach($a as $k => $v)
		{
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

/*
		if($section === '')
			return($r);

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
		else if(isset($user['obj']['acl']) === true)
			$r = &$user['obj']['acl'];
		else if(isset($this->_conf['group'][$user['meta']]) === true)
			$r = $this->_conf['group'][$user['meta']];

		return($r);
	}

	function edit($arr)
	{
		if(is_array($arr) === false || isset($arr['id']) === false)
			return(false);
		else if(isset($arr['tree']) === false)
			$arr['tree'] = array();

		return($this->_dso->acl_edit($arr['id'],$this->_chk_tree($arr['tree'])));
	}

	function get_user($id)
	{
		if(($user = $this->get($id)) === null)
			return(false);

		return($this->_mk_user_tree($this->get_tree(),$user));
	}

	function _mk_user_tree($tree,$user)
	{
		if(is_array($tree) === false || empty($tree) === true)
			return($tree);

		reset($tree);

		while(list($key) = each($tree))
		{
			$ref = &$tree[$key];
			$ref['access'] = $this->chk($ref['path'],$user);

			if(isset($ref['child']) === true)
				$ref['child'] = $this->_mk_user_tree($ref['child'],$user);
		}

		return($tree);
	}

	function chk($section,$user)
	{
		if($user === null || is_bool($user) === true)
			return((bool) $user);

		$section = trim(preg_replace('@/+@','/',$section),'/');
		$tree = $this->get_tree();

		$arr = explode('/',$section);
		$nb = count($arr);

		for($i = 0;$i < $nb;$i++)
		{
			$k = &$arr[$i];
			$grp = $user;

			if(xivo_issa($k,$tree) === false || ($user = xivo_ak($k,$user,true)) === false)
				return(false);
			else if($user === true)
				return(true);
			else if(isset($tree[$k]['child']) === true)
				$tree = &$tree[$k]['child'];
		}

		if(xivo_ak($k,$grp,true) !== false)
			return(true);
			
		return(false);
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

	function _chk_path($path,$tree)
	{
		if(is_array($tree) === false)
			return(false);

		$path = (string) $path;

		$arr = explode('/',$path);
		$arr_path = $arr;

		if(($nb = count($arr)) === 0)
			return(false);

		for($i = 0;$i < $nb;$i++)
		{
			$k = &$arr[$i];

			if(xivo_ak($k,$tree) === false)
				return(false);

			$tree = &$tree[$k];
		}

		if(is_array($tree) === false)
			$r = &$arr_path;
		else
			$r = false;

		return($r);
	}
}

?>
