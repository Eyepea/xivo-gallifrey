<?php

require_once(xivo_file::joinpath(XIVO_PATH_OBJECT,'abstract','acl.inc'));

class xivo_accesswebservice_acl extends xivo_acl_abstract
{
	var $_conf	= array();
	var $_dso	= null;
	var $_tree	= null;

	function xivo_accesswebservice_acl(&$dso)
	{
		if(defined('XIVO_WEBSERVICE_ACL_FILE') === true)
			$path = XIVO_WEBSERVICE_ACL_FILE;
		else
			$path = xivo_file::joinpath(XIVO_PATH_OBJECTCONF,'acl','webservice');

		if(($this->_conf = xivo_gat::get_file($path)) === false
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
}

?>
