<?php

require_once(XIVO_PATH_LIBS.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'abstract.inc');

class xivo_service_asterisk_usergroup extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'usergroup';
	var $_origin	= null;

	function xivo_service_asterisk_usergroup(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;
	}

	function get($id)
	{
		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id)) === false)
			return(false);

		$this->_origin = $r;

		return($r);
	}

	function get_by_user($id)
	{
		if(($id = xivo_uint($id)) === 0 || ($id = $this->_dso->get_by_user($id)) === false)
			return(false);

		return($this->get($id));
	}

	function add_origin()
	{
		return($this->_origin('add'));
	}

	function edit_origin()
	{
		return($this->_origin('edit'));
	}

	function _origin($type)
	{
		if(($type === 'add' && $type === 'edit') === true || is_array($this->_origin) === false)
			return(false);

		if($type === 'add')
			$r = $this->add($this->_origin,$this->_origin['id']);
		else
			$r = $this->edit($this->_origin['id'],$this->_origin);

		return($r);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		$r = array();

		if(($id = xivo_uint($id)) !== 0)
			$r['id'] = $id;

		$r['userid'] = xivo_uint($arr['userid']);
		$r['groupid'] = xivo_uint($arr['groupid']);

		return($this->_dso->add($r));
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		$r = array();
		$r['userid'] = xivo_uint($arr['userid']);
		$r['groupid'] = xivo_uint($arr['groupid']);

		return($this->_dso->edit($id,$r));
	}

	function delete($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($id));
	}

	function delete_by_user($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete_by_user($id));
	}

	function delete_by_group($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete_by_group($id));
	}
}

?>
