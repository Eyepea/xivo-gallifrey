<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_generaloutcall_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'generaloutcall');
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

	function exists_range($val)
	{
		$val = xivo_uint($val);

		if(($where = $this->_dso->between_column($val,'rangebeg','rangeend')) === '')
			return(null);

		if(($r = $this->_dso->select_one($this->_param['table'],'id',$where)) !== false)
			$r = $r['id'];

		return($r);
	}
}

?>
