<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_userfeatures_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'userfeatures');
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

	function get_search($value)
	{
		$where = array();

		if(($rs = $this->_dso->search_contain('firstname',$value)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_contain('lastname',$value)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_contain('number',$value)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_contain('name',$value)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_contain('context',$value)) !== false)
			$where[] = $rs;

		if(($where = implode(' OR ',$where)) === '')
			return(false);

		$r = $this->_dso->select_all($this->_param['table'],null,$where);

		return($r);
	}
}

?>
