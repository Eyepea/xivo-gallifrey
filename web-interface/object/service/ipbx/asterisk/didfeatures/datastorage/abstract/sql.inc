<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_didfeatures_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'didfeatures');
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
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$where = 'id = '.$id;

		if($disable !== null)
			$where .= ' AND commented = '.intval((bool) $disable);

		$r = $this->_dso->select_one($this->_param['table'],null,$where);

		return($r);
	}

	function get_where($arr,$disable=null)
	{
		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		if($disable !== null)
			$where .= ' AND commented = '.intval((bool) $disable);

		if(($r = $this->_dso->select_one($this->_param['table'],'id',$where)) !== false)
			$r = $r['id'];

		return($r);
	}

	function add($arr)
	{
		$r = false;

		if($this->_dso->insert($this->_param['table'],$arr) === true)
			$r = $this->_dso->insert_id();

		return($r);
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$r = $this->_dso->update($this->_param['table'],$arr,'id = '.$id);

		return($r);
	}

	function edit_where($arr,$values)
	{
		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		$r = $this->_dso->update($this->_param['table'],$values,$where);

		return($r);
	}

	function delete($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$r = $this->_dso->delete($this->_param['table'],'id = '.$id);

		return($r);
	}

	function delete_where($arr,$disable=null)
	{
		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		if($disable !== null)
			$where .= ' AND commented = '.intval((bool) $disable);

		$r = $this->_dso->delete($this->_param['table'],$where);

		return($r);
	}

	function get_list($disable=null)
	{
		$where = '';

		if($disable !== null)
			$where = ' WHERE commented = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table'].$where);

		if($rs === false)
			return(false);

		$r = array();

		while($rw = $this->_dso->fetch_assoc($rs))
			$r[] = $rw['id'];

		if($this->_dso->is_free() === true)
			$this->_dso->free($rs);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_list_where($arr,$disable=null)
	{
		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		if($disable !== null)
			$where .= ' AND commented = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table'].' WHERE '.$where);

		if($rs === false)
			return(false);

		$r = array();

		while($rw = $this->_dso->fetch_assoc($rs))
			$r[] = $rw['id'];

		if($this->_dso->is_free() === true)
			$this->_dso->free($rs);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
