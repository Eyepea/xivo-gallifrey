<?php

class xivo_service_asterisk_queue_abstract_sql
{
	var $_param	= array('table' => 'queue');
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

	function get($id,$disable=null)
	{
		$where = 'name = \''.$this->_dso->escape_string($id).'\'';

		if($disable !== null)
			$where .= ' AND commented = '.intval((bool) $disable);

		$r = $this->_dso->select_one($this->_param['table'],null,$where);

		return($r);
	}

	function add($arr)
	{
		$r = $this->_dso->insert($this->_param['table'],$arr);

		return($r);
	}

	function edit($id,$arr)
	{
		$r = $this->_dso->update($this->_param['table'],$arr,'name = \''.$this->_dso->escape_string($id).'\'');

		return($r);
	}

	function delete($id)
	{
		$r = $this->_dso->delete($this->_param['table'],'name = \''.$this->_dso->escape_string($id).'\'');

		return($r);
	}

	function get_list($disable=null)
	{
		$where = '';

		if($disable !== null)
			$where = ' WHERE commented = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT name FROM '.$this->_param['table'].$where);

		if($rs === false)
			return(false);

		$r = array();

		while($rw = $this->_dso->fetch_assoc($rs))
			$r[] = $rw['name'];

		if($this->_dso->is_free() === true)
			$this->_dso->free($rs);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_where($arr,$disable=null)
	{
		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		if($disable !== null)
			$where .= ' AND commented = '.intval((bool) $disable);

		$r = $this->_dso->select_all($this->_param['table'],null,$where);

		return($r);
	}
}

?>
