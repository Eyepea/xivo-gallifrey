<?php

class xivo_service_asterisk_uservoicemail_sqlite
{
	var $_param	= array('table' => 'uservoicemail');

	var $_dso	= null;

	function xivo_service_asterisk_uservoicemail_sqlite(&$dso,$param=array())
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

		$commented = '';

		if($disable !== null)
			$commented = ' AND commented = '.intval((bool) $disable);

		$rs = $this->_dso->query('SELECT * FROM '.$this->_param['table'].' WHERE id = '.$id.$commented.' LIMIT 1');

		if($rs !== false && ($rw = $this->_dso->fetch_assoc($rs)) !== false)
			$r = &$rw;

		return($r);
	}

	function get_by_mailbox($mailbox)
	{
		$r = false;

		if(($mailbox = xivo_uint($mailbox)) === 0)
			return($r);

		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table'].' WHERE mailbox = '.$mailbox.' LIMIT 1');

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
}

?>
