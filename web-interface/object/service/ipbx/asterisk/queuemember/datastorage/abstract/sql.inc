<?php

class xivo_service_asterisk_queuemember_abstract_sql
{
	var $_param	= array('table' => 'queuemember');
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

	function get($ids,$disable=null)
	{
		if(is_array($ids) === false)
			return(false);

		$where = 'queue_name = \''.$this->_dso->escape_string($ids['queue_name']).'\''.
			 ' AND interface = \''.$this->_dso->escape_string($ids['interface']).'\'';

		if($disable !== null)
			$where .= ' AND commented = '.intval((bool) $disable);

		$r = $this->_dso->select_one($this->_param['table'],null,$where);

		return($r);
	}

	function get_where($arr)
	{
		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		if(($r = $this->_dso->select_one($this->_param['table'],'queue_name',$where)) !== false)
			$r = $r['queue_name'];

		return($r);
	}

	function add($arr)
	{
		$r = $this->_dso->insert($this->_param['table'],$arr);

		return($r);
	}

	function edit($ids,$arr)
	{
		if(is_array($ids) === false)
			return(false);

		$where = 'queue_name = \''.$this->_dso->escape_string($ids['queue_name']).'\''.
			 ' AND interface = \''.$this->_dso->escape_string($ids['interface']).'\'';

		$r = $this->_dso->update($this->_param['table'],$arr,$where);

		return($r);
	}

	function edit_where($arr,$values)
	{
		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		$r = $this->_dso->update($this->_param['table'],$values,$where);

		return($r);
	}

	function delete($ids)
	{
		if(is_array($ids) === false)
			return(false);

		$where = 'queue_name = \''.$this->_dso->escape_string($ids['queue_name']).'\''.
			 ' AND interface = \''.$this->_dso->escape_string($ids['interface']).'\'';

		$r = $this->_dso->delete($this->_param['table'],$where);

		return($r);
	}

	function delete_where($arr)
	{
		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		$r = $this->_dso->delete($this->_param['table'],$where);

		return($r);
	}

	function get_list($disable=null)
	{
		$where = '';

		if($disable !== null)
			$where = ' WHERE commented = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT queue_name, interface FROM '.$this->_param['table'].$where);

		if($rs === false)
			return(false);

		$r = array();

		while($rw = $this->_dso->fetch_assoc($rs))
			$r[] = $rw;

		if($this->_dso->is_free() === true)
			$this->_dso->free($rs);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_nb_by_name($name,$disable=null)
	{
		$where = 'queue_name = \''.$this->_dso->escape_string($name).'\'';

		if($disable !== null)
			$where .= ' AND commented = '.intval((bool) $disable);

		$r = $this->_dso->select_one($this->_param['table'],'COUNT(*) AS cnt',$where);

		if($r !== false)
			$r = (int) $r['cnt'];

		return($r);
	}

	function get_all($disable=null)
	{
		$where = '';

		if($disable !== null)
			$where = 'commented = '.intval((bool) $disable);

		$r = $this->_dso->select_all($this->_param['table'],null,$where);

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
