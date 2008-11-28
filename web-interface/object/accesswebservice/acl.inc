<?php

require_once(xivo_file::joinpath(XIVO_PATH_OBJECT,'abstract','acl.inc'));

class xivo_accesswebservice_acl extends xivo_acl_abstract
{
	var $_conf	= array();
	var $_dso	= null;
	var $_tree	= null;

	function xivo_accesswebservice_acl(&$dso)
	{
		if(defined('XIVO_WEBSERVICE_ACL_PATH') === true)
			$path = XIVO_WEBSERVICE_ACL_PATH;
		else
			$path = XIVO_PATH_OBJECTCONF.XIVO_SEP_DIR.'acl';

		if(($this->_conf = xivo_gat::load_get('webservice',$path,'xivo_webservice_acl')) === false
		|| isset($this->_conf['tree']) === false)
			trigger_error('Failed to load web services ACL Array',E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_acl_tree = $this->_conf['tree'];
		$this->_dso = &$dso;
	}

	function get($id)
	{
		if(($webservice = $this->_dso->get($id)) === false)
			return(null);
		else if(isset($webservice['obj']['acl']) === true)
			return($webservice['obj']['acl']);

		return(array());
	}

	function edit($arr)
	{
		if(is_array($arr) === false || isset($arr['id']) === false)
			return(false);
		else if(isset($arr['tree']) === false)
			$arr['tree'] = array();

		return($this->_dso->acl_edit($arr['id'],$this->build_tree_by_path($arr['tree'])));
	}

	function get_webservice_tree($id)
	{
		if(($webservice = $this->get($id)) === null)
			return(false);

		return($this->_mk_webservice_tree($this->get_full_tree(),$webservice));
	}

	function _mk_webservice_tree($tree,$webservice)
	{
		if(is_array($tree) === false || empty($tree) === true)
			return($tree);

		reset($tree);

		while(list($key) = each($tree))
		{
			$ref = &$tree[$key];
			$ref['access'] = $this->has_access($ref['path'],$webservice);

			if(isset($ref['child']) === true)
				$ref['child'] = $this->_mk_webservice_tree($ref['child'],$webservice);
		}

		return($tree);
	}

	function _chk_section($section,$id)
	{
		if(is_scalar($section) === false
		|| xivo_ulongint($id) === false)
			return(false);

		$section = trim(preg_replace('@/+@','/',$section),'/');

		$tree = $this->get_full_tree();
		$webservice = $this->get($id);

		$r = false;

		if($webservice === true)
			$r = true;
		else if(is_array($webservice) === true)
			$r = false;
		else
			return(false);

		$arr = explode('/',$section);
		$nb = count($arr);

		for($i = 0;$i < $nb;$i++)
		{
			$key = &$arr[$i];
			$grp = $webservice;

			if(xivo_issa($key,$tree) === false)
				return(false);
			else if($r === false
			&& is_bool(($webservice = xivo_ak($key,$webservice,true))) === true)
			{
				if(isset($tree[$key]['value']) === true
				&& (bool) $tree[$key]['value'] === false)
					return(false);

				return($webservice);
			}
			else if(isset($tree[$key]['child']) === true)
				$tree = &$tree[$key]['child'];
		}

		if($r === false && isset($key,$grp) === true)
			$r = xivo_ak($key,$grp,true) !== false;

		return($r);
	}
}

?>
