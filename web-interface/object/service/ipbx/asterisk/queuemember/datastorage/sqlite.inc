<?php

class xivo_service_asterisk_queuemember_sqlite
{
	var $_param	= array('table' => 'queuemember');

	var $_dso	= null;

	function xivo_service_asterisk_queuemember_sqlite(&$dso,$param=array())
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
		$r = false;

		if(is_array($ids) === false)
			return($r);

		$commented = '';

		if($disable !== null)
			$commented = ' AND commented = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT * FROM '.$this->_param['table'].
		' WHERE queue_name = \''.$this->_dso->escape_string($ids['queue_name']).'\''.
		' AND interface = \''.$this->_dso->escape_string($ids['interface']).'\''.$commented.' LIMIT 1');

		if($rs !== false && ($rw = $this->_dso->fetch_assoc($rs)) !== false)
			$r = &$rw;

		return($r);
	}

	function get_list_by_interface($interface)
	{
		$r = false;

		$rs = $this->_dso->query('SELECT queue_name, interface FROM '.$this->_param['table'].
		' WHERE interface = \''.$this->_dso->escape_string($interface).'\'');

		if($rs !== false)
		{
			$r = array();

			while($rw = $this->_dso->fetch_assoc($rs))
				$r[] = $rw;
		}

		return($r);
	}

	function get_nb_by_name($name,$disable=null)
	{
		$r = false;

		$commented = '';

		if($disable !== null)
			$commented = ' AND commented = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT COUNT(queue_name) AS cnt FROM '.$this->_param['table'].
		' WHERE queue_name = \''.$this->_dso->escape_string($name).'\''.$commented);

		if($rs !== false && ($rw = $this->_dso->fetch_assoc($rs)) !== false)
			$r = (int) $rw['cnt'];

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

		$r = $this->_dso->update($this->_param['table'],$arr,'queue_name = \''.$this->_dso->escape_string($ids['queue_name']).'\''.
		' AND interface = \''.$this->_dso->escape_string($ids['interface']).'\'');

		return($r);
	}

	function edit_where($arr,$values)
	{
		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		$r = $this->_dso->update($this->_param['table'],$values,$where);
	}

	function delete($ids)
	{
		if(is_array($ids) === false)
			return(false);

		$r = $this->_dso->delete($this->_param['table'],'queue_name = \''.$this->_dso->escape_string($ids['queue_name']).'\''.
		' AND interface = \''.$this->_dso->escape_string($ids['interface']).'\'');

		return($r);
	}

	function delete_by_name($name)
	{
		$r = $this->_dso->delete($this->_param['table'],'name = \''.$this->_dso->escape_string($name).'\'');

		return($r);
	}

	function delete_by_interface($interface)
	{
		$r = $this->_dso->delete($this->_param['table'],'interface = \''.$this->_dso->escape_string($interface).'\'');

		return($r);
	}

	function get_list($disable=null)
	{
		$r = false;

		$commented = '';

		if($disable !== null)
			$commented = ' WHERE commented = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT queue_name, interface FROM '.$this->_param['table'].$commented);

		if($rs !== false)
		{	
			$r = array();
			while($rw = $this->_dso->fetch_assoc($rs))
				$r[] = array('queue_name' => $rw['queue_name'],'interface' => $rw['interface']);
		}

		return($r);
	}
}

?>
