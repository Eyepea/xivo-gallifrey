<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_autoprov_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'phone');
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

	function get($macaddr)
	{
		$where = 'macaddr = \''.$this->_dso->escape_string($macaddr).'\'';

		$r = $this->_dso->select_one($this->_param['table'],null,$where);

		return($r);
	}

	function get_where($arr)
	{
		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		if(($r = $this->_dso->select_one($this->_param['table'],'macaddr',$where)) !== false)
			$r = $r['macaddr'];

		return($r);
	}

	function get_list()
	{
		$rs = $this->_dso->query('SELECT macaddr FROM '.$this->_param['table']);

		if($rs === false)
			return(false);

		$r = array();

		while($rw = $this->_dso->fetch_assoc($rs))
			$r[] = $rw['macaddr'];

		if($this->_dso->is_free() === true)
			$this->_dso->free($rs);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
