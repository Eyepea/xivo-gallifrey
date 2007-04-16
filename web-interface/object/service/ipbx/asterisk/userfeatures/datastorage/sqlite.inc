<?php

class xivo_service_asterisk_userfeatures_sqlite
{
	var $_param	= array('table' => 'userfeatures');

	var $_dso	= null;

	function xivo_service_asterisk_userfeatures_sqlite(&$dso,$param=array())
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
		$r = false;

		if(($id = xivo_uint($id)) === 0)
			return($r);

		$rs = $this->_dso->query('SELECT * FROM '.$this->_param['table'].' WHERE id = '.$id.' LIMIT 1');

		if($rs !== false && ($rw = $this->_dso->fetch_assoc($rs)) !== false)
			$r = &$rw;

		return($r);
	}

	function get_by_protocol($id,$protocol)
	{
		$r = false;

		if(($id = xivo_uint($id)) === 0)
			return($r);

		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table'].
		' WHERE protocolid = '.$id.' AND protocol = \''.$this->_dso->escape_string($protocol).'\' LIMIT 1');

		if($rs !== false && ($rw = $this->_dso->fetch_assoc($rs)) !== false)
			$r = $rw['id'];

		return($r);
	}

	function get_by_provisioning($id)
	{
		$r = false;

		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table'].
		' WHERE provisioningid = \''.$this->_dso->escape_string($id).'\' LIMIT 1');

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

	function get_search($value)
	{
		$r = false;

		$value = $this->_dso->escape_string($value);

		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table'].
		' WHERE firstname LIKE \'%'.$value.'%\' OR lastname LIKE \'%'.$value.'%\''.
		' OR number LIKE \'%'.$value.'%\'');

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
