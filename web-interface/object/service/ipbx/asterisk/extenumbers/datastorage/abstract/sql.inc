<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_extenumbers_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'extenumbers');
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

	function exists_exten($arr,$context='')
	{
		$context = (string) $context;

		if(($where = $this->_dso->where($arr)) === false)
			return(null);

		if($context !== '')
			$where .= ' AND (context = \''.$this->_dso->escape_string($context).'\' OR context = \'\')';

		if(($r = $this->_dso->select_one($this->_param['table'],'id',$where)) !== false)
			$r = (int) $r['id'];

		return($r);
	}

	function exists_range($val,$context='')
	{
		$val = xivo_uint($val);
		$context = (string) $context;

		if(($where = $this->_dso->between_column($val,'rangebeg','rangeend')) === '')
			return(null);

		if($context !== '')
			$where .= ' AND (context = \''.$this->_dso->escape_string($context).'\' OR context = \'\')';

		if(($r = $this->_dso->select_one($this->_param['table'],'id',$where)) !== false)
			$r = (int) $r['id'];

		return($r);
	}
}

?>
