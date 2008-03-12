<?php

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'helper'.XIVO_SEP_DIR.'table.inc');

class xivo_user_abstract_sql extends xivo_datastorage_helper_table
{
	var $_table	= 'user';
	var $_primary	= array('id' => 0);
	var $_disable	= 'valid';
	var $_unique	= array(array('login','meta'));

	function xivo_user_abstract_sql(&$dso,$param=array())
	{
		parent::_init(&$dso,$param);
	}

	function get($id,$disable=null)
	{
		if(($r = parent::get($id,$disable)) === false)
			return(false);

		$r['obj'] = xivo_unserialize($r['obj']);

		return($r);
	}

	function chk_update($id,$dupdate)
	{
		if(($id = xivo_ulongint($id)) === 0 || ($dupdate = xivo_uint($dupdate)) === 0)
			return(false);

		$this->_dso->new_select($this->_table,'id');
		$this->_dso->where(array('id' => $id));
		$this->_dso->andwherenot(array('dupdate' => $dupdate));

		if(($r = $this->_dso->select_one()) !== false)
			$r = true;

		return($r);
	}

	function acl_edit($id,$arr)
	{
		if(($obj = $this->_get_obj($id)) === false)
			return(false);

		$obj['acl'] = $arr;

		return($this->_save_obj($id,$obj));
	}

	function _save_obj($id,$obj)
	{
		if(($id = xivo_ulongint($id)) === 0 || is_array($obj) === false)
			return(false);

		$r = $this->_dso->update($this->_table,
					 array('obj' => serialize($obj)),
					 array('id' => $id));

		return($r);
	}

	function _get_obj($id)
	{
		if(($id = xivo_ulongint($id)) === 0)
			return(false);

		$this->_dso->new_select($this->_table,'obj');
		$this->_dso->where(array('id' => $id));

		if(($r = $this->_dso->select_one()) === false)
			return(false);

		return(xivo_unserialize($r));
	}
}

?>
