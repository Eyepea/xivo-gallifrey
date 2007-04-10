<?php

class xivo_service_asterisk_didfeatures_sqlite
{
	var $_param	= array('table' => 'didfeatures');

	var $_dso	= null;

	function xivo_service_asterisk_didfeatures_sqlite(&$dso,$param=array())
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
		$r = false;

		if(($id = xivo_uint($id)) === 0)
			return($r);

		$disabled = '';

		if($disable !== null)
			$disabled = ' AND disable = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT * FROM '.$this->_param['table'].' WHERE id = '.$id.$disabled.' LIMIT 1');

		if($rs !== false && ($rw = $this->_dso->fetch_assoc($rs)) !== false)
			$r = &$rw;

		return($r);
	}

	function get_where($arr,$disable=null)
	{
		$r = false;

		if(($where = $this->_dso->where($arr)) === false)
			return($r);

		if($disable !== null)
			$where .= ' AND disable = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table'].' WHERE '.$where.' LIMIT 1');

		if($rs !== false && ($rw = $this->_dso->fetch_assoc($rs)) !== false)
			$r = $rw['id'];

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
			$where .= ' AND disable = '.intval((bool) $disable);

		$r = $this->_dso->delete($this->_param['table'],$where);

		return($r);
	}

	function get_list()
	{
		$r = false;

		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table']);

		if($rs !== false)
		{
			$r = array();
			while($rw = $this->_dso->fetch_assoc($rs))
				$r[] = $rw['id'];
		}

		return($r);
	}

	function get_list_where($arr,$disable=null)
	{
		$r = false;

		if(($where = $this->_dso->where($arr)) === false)
			return($r);

		if($disable !== null)
			$where .= ' AND disable = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table'].' WHERE '.$where);

		if($rs !== false)
		{
			$r = array();
			while($rw = $this->_dso->fetch_assoc($rs))
				$r[] = $rw['id'];
		}

		return($r);
	}
}

?>
