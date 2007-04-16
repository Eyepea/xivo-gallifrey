<?php

class xivo_user_sqlite
{
	var $_param		= array('table'	=> 'user');

	var $_dso		= null;

	function xivo_user_sqlite(&$dso,$param=array())
	{
		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		$param = (array) $param;

		if(isset($param['table']) === true)
			$this->_param['table'] = (string) $param['table'];
	}

	function add($arr)
	{
		$r = false;

		$arr['dcreate'] = time();
		$arr['dupdate'] = time();

		if($this->_dso->insert($this->_param['table'],$arr) === true)
			$r = $this->_dso->insert_id();

		return($r);
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$arr['dupdate'] = time();

		$r = $this->_dso->update($this->_param['table'],$arr,'id = '.$id);

		return($r);
	}

	function get($id)
	{
		$r = false;

		if(($id = xivo_uint($id)) === 0)
			return($r);

		$rs = $this->_dso->query('SELECT * FROM '.$this->_param['table'].' WHERE id = '.$id.' LIMIT 1');

		if($rs !== false && ($r = $this->_dso->fetch_assoc($rs)) !== false)
			$r['obj'] = xivo_unserialize($r['obj']);

		return($r);
	}

	function get_by_login_pass($login,$pass)
	{
		$r = false;

		$rs = $this->_dso->query('SELECT * FROM '.$this->_param['table'].' '.
					'WHERE login = \''.$this->_dso->escape_string($login).'\' '.
					'AND passwd = \''.$this->_dso->escape_string($pass).'\' LIMIT 1');

		if($rs !== false && ($r = $this->_dso->fetch_assoc($rs)) !== false)
			$r['obj'] = xivo_unserialize($r);

		return($r);
	}

	function get_list($arr=array())
	{
		$r = false;
		$arr = (array) $arr;
		$where = '';

		if(empty($arr) === false)
			$where .= 'meta IN(\''.implode('\',\'',array_map(array(&$this->_dso,'escape_string'), $arr)).'\')';

		if($where !== '')
			$where = ' WHERE '.$where;

		$rs = $this->_dso->query('SELECT * FROM '.$this->_param['table'].$where);

		if($rs !== false)
		{
			$r = array();

			while($rw = $this->_dso->fetch_assoc($rs))
			{
				$rw['obj'] = xivo_unserialize($rw['obj']);
				$r[] = $rw;
			}
		}

		return($r);
	}

	function chk_update($id,$dupdate)
	{
		$r = false;

		if(($id = xivo_uint($id)) === 0 || ($dupdate = xivo_uint($dupdate)) === 0)
			return($r);

		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table'].' WHERE id = '.$id.' AND dupdate != '.$dupdate.' LIMIT 1');

		if($rs !== false && $this->_dso->fetch_assoc($rs) !== false)
			$r = true;

		return($r);
	}

	function policy_edit($id,$arr)
	{
		$r = false;

		if(($obj = $this->_get_obj($id)) === false)
			return($r);

		$obj['policy'] = $arr;

		$r = $this->_save_obj($id,$obj);

		return($r);
	}

	function _save_obj($id,$obj)
	{
		$r = false;

		if(($id = xivo_uint($id)) === 0 || is_array($obj) === false)
			return($r);

		$arr = array('obj' => serialize($obj));

		$r = $this->_dso->update($this->_param['table'],$arr,'id = '.$id);

		return($r);
	}

	function _get_obj($id)
	{
		$r = false;

		if(($id = xivo_uint($id)) === 0)
			return($r);

		$rs = $this->_dso->query('SELECT obj FROM '.$this->_param['table'].' WHERE id = '.$id.' LIMIT 1');

		if($rs !== false && ($rw = $this->_dso->fetch_assoc($rs)) !== false)
			$r = xivo_unserialize($rw['obj']);

		return($r);
	}
}

?>
