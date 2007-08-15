<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_groupfeatures_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'groupfeatures');
	var $_dso	= null;

	function _init(&$dso,$param=array())
	{
		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		$param = (array) $param;

		if(isset($param['table']) === true)
			$this->_param['table'] = (string) $param['table'];
	}

	function get_last()
	{
		$r = $this->_dso->select_one($this->_param['table'],'COUNT(*) AS cnt, MAX(id) AS id');

		if($r !== false)	
			$r = $r['cnt'] > 0 ? $r['id'] : 0;

		return($r);
	}

	function get_free()
	{
		$r = false;

		$this->_dso->order(array('id' => SORT_ASC));

		$r = $this->_dso->select_one($this->_param['table'],'id','deleted = 1');

		$this->_dso->order();

		if($r !== false)
			$r = $r['id'];

		return($r);
	}
}

?>
