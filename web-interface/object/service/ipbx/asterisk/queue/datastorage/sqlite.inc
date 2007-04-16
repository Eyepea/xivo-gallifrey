<?php

class xivo_service_asterisk_queue_sqlite
{
	var $_param	= array('table' => 'queue');

	var $_dso	= null;

	function xivo_service_asterisk_queue_sqlite(&$dso,$param=array())
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

		$commented = '';

		if($disable !== null)
			$commented = ' AND commented = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT * FROM '.$this->_param['table'].
		' WHERE name = \''.$this->_dso->escape_string($id).'\''.$commented.' LIMIT 1');

		if($rs !== false && ($rw = $this->_dso->fetch_assoc($rs)) !== false)
			$r = &$rw;

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
		$r = false;

		$commented = '';

		if($disable !== null)
			$commented = ' WHERE commented = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT name FROM '.$this->_param['table'].$commented);

		if($rs !== false)
		{	
			$r = array();
			while($rw = $this->_dso->fetch_assoc($rs))
				$r[] = $rw['name'];
		}

		return($r);
	}
}

?>
