<?php

class xivo_service_asterisk_meetmefeatures_abstract_sql
{
	var $_param	= array('table' => 'meetmefeatures');
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

	function get($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$r = $this->_dso->select_one($this->_param['table'],null,'id = '.$id);

		return($r);
	}

	function get_by_meetme($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		if(($r = $this->_dso->select_one($this->_param['table'],'id','meetmeid = '.$id)) !== false)
			$r = $r['id'];

		return($r);
	}

	function get_where($arr)
	{
		if(($where = $this->_dso->where($arr)) === false)
			return(false);

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

	function get_list()
	{
		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table']);

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
