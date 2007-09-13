<?php

class xivo_user_abstract_sql
{
	var $_param		= array('table'	=> 'user');
	var $_dso		= null;

	function _init(&$dso,$param=array())
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
		if(($id = xivo_uint($id)) === 0)
			return(false);

		if(($r = $this->_dso->select_one($this->_param['table'],null,'id = '.$id)) !== false)
		{
			$r['valid'] = (bool) $r['valid'];
			$r['obj'] = xivo_unserialize($r['obj']);
		}

		return($r);
	}

	function get_by_login_pass($login,$pass)
	{
		$where = 'login = \''.$this->_dso->escape_string($login).'\''.
			 ' AND passwd = \''.$this->_dso->escape_string($pass).'\'';

		if(($r = $this->_dso->select_one($this->_param['table'],null,$where)) !== false)
		{
			$r['valid'] = (bool) $r['valid'];
			$r['obj'] = xivo_unserialize($r['obj']);
		}

		return($r);
	}

	function get_list($arr=array())
	{
		$arr = (array) $arr;
		$where = '';

		if(empty($arr) === false)
			$where = ' WHERE meta IN(\''.implode('\',\'',array_map(array(&$this->_dso,'escape_string'), $arr)).'\')';

		$rs = $this->_dso->query('SELECT * FROM '.$this->_param['table'].$where);

		if($rs === false)
			return(false);

		$r = array();

		while($rw = $this->_dso->fetch_assoc($rs))
		{
			$rw['valid'] = (bool) $rw['valid'];
			$rw['obj'] = xivo_unserialize($rw['obj']);
			$r[] = $rw;
		}

		if($this->_dso->is_free() === true)
			$this->_dso->free($rs);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function chk_update($id,$dupdate)
	{
		if(($id = xivo_uint($id)) === 0 || ($dupdate = xivo_uint($dupdate)) === 0)
			return(false);

		$where = 'id = '.$id.' AND dupdate != '.$dupdate;

		if(($r = $this->_dso->select_one($this->_param['table'],'id',$where)) !== false)
			$r = true;

		return($r);
	}

	function acl_edit($id,$arr)
	{
		if(($obj = $this->_get_obj($id)) === false)
			return(false);

		$obj['acl'] = $arr;

		$r = $this->_save_obj($id,$obj);

		return($r);
	}

	function _save_obj($id,$obj)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($obj) === false)
			return(false);

		$arr = array('obj' => serialize($obj));

		$r = $this->_dso->update($this->_param['table'],$arr,'id = '.$id);

		return($r);
	}

	function _get_obj($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		if(($r = $this->_dso->select_one($this->_param['table'],'obj','id = '.$id)) !== false)
			$r = xivo_unserialize($r['obj']);

		return($r);
	}
}

?>
