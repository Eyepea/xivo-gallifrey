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

	function get_search($exten,$extenhash,$context='')
	{
		$where = array();

		if(($rs = $this->_dso->search_contain('exten',$exten)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_exact('extenhash',$extenhash)) !== false)
			$where[] = $rs;

		if(xivo_haslen($context) !== false
		&& ($rs = $this->_dso->search_exact('context',$context)) !== false)
			$where[] = $rs;

		if(($where = implode(' OR ',$where)) === '')
			return(false);

		$r = $this->_dso->select_all($this->_param['table'],null,$where);

		return($r);
	}
}

?>
