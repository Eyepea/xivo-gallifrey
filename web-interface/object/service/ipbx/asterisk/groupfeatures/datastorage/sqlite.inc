<?php

class xivo_service_asterisk_groupfeatures_sqlite
{
	var $_param	= array('table' => 'groupfeatures');

	var $_dso	= null;

	function xivo_service_asterisk_groupfeatures_sqlite(&$dso,$param=array())
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
			$disabled = ' AND commented = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT * FROM '.$this->_param['table'].' WHERE id = '.$id.$disabled.' LIMIT 1');

		if($rs !== false && ($rw = $this->_dso->fetch_assoc($rs)) !== false)
			$r = &$rw;

		return($r);
	}

	function get_where($arr)
	{
		$r = false;

		if(($where = $this->_dso->where($arr)) === false)
			return($r);

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

	function delete($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$r = $this->_dso->delete($this->_param['table'],'id = '.$id);

		return($r);
	}

	function get_list($disable=null)
	{
		$r = false;

		$disabled = '';

		if($disable !== null)
			$disabled = ' WHERE commented = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table'].$disabled);

		if($rs !== false)
		{
			$r = array();
			while($rw = $this->_dso->fetch_assoc($rs))
				$r[] = $rw['id'];
		}

		return($r);
	}

	function get_last()
	{
		$r = false;

		$rs = $this->_dso->query('SELECT COUNT(id) AS cnt, MAX(id) AS id FROM '.$this->_param['table'].' LIMIT 1');

		if($rs !== false && ($rw = $this->_dso->fetch_assoc($rs)) !== false)
			$r = $rw['cnt'] > 0 ? $rw['id'] : 0;

		return($r);
	}

	function get_free()
	{
		$r = false;

		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table'].' WHERE commented = 1 ORDER BY id ASC LIMIT 1');

		if($rs !== false && ($rw = $this->_dso->fetch_assoc($rs)) !== false)
			$r = $rw['id'];

		return($r);
	}

	function is_valid($arr)
	{
		$r = false;

		$where = 'id IN(\''.implode('\',\'',array_map(array(&$this->_dso,'escape_string'), $arr)).'\')';

		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table'].' WHERE '.$where.' AND commented = 0');

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
