<?php

class xivo_service_asterisk_usergroup_sqlite
{
	var $_param	= array('table' => 'usergroup');

	var $_dso	= null;

	function xivo_service_asterisk_usergroup_sqlite(&$dso,$param=array())
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

	function get_by_user($id)
	{
		$r = false;

		if(($id = xivo_uint($id)) === 0)
			return($r);

		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table'].' WHERE userid = '.$id.' LIMIT 1');

		if($rs !== false && ($rw = $this->_dso->fetch_assoc($rs)) !== false)
			$r = $rw['id'];

		return($r);
	}

	function add($arr)
	{
		$r = $this->_dso->insert($this->_param['table'],$arr);

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

	function delete_by_user($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$r = $this->_dso->delete($this->_param['table'],'userid = '.$id);

		return($r);
	}

	function delete_by_group($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$r = $this->_dso->delete($this->_param['table'],'groupid = '.$id);

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
}

?>
