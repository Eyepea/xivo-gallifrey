<?php

class xivo_service_asterisk_staticconf_sqlite
{
	var $_dso	= null;

	function get($id,$disable=null)
	{
		$r = false;

		if(($id = xivo_uint($id)) === 0)
			return($r);

		$commented = '';

		if($disable !== null)
			$commented = ' AND commented = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT * FROM '.$this->_param['table'].
		' WHERE id = '.$id.$commented.' LIMIT 1');

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
			$where .= ' AND commented = '.intval((bool) $disable);

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

	function edit_where($arr,$where,$disable=null)
	{
		if(($where = $this->_dso->where($where)) === false)
			return($r);

		if($disable !== null)
			$where .= ' AND commented = '.intval((bool) $disable);

		$r = $this->_dso->update($this->_param['table'],$arr,$where);

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
			return($r);

		if($disable !== null)
			$where .= ' AND commented = '.intval((bool) $disable);

		$r = $this->_dso->delete($this->_param['table'],$where);

		return($r);
	}

	function get_list($disable=null)
	{
		$r = false;

		$commented = '';

		if($disable !== null)
			$commented = ' WHERE commented = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table'].$commented);

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
			$where .= ' AND commented = '.intval((bool) $disable);

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
