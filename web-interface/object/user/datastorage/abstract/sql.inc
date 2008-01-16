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

		$r = $this->_dso->update($this->_param['table'],$arr,array('id' => $id));

		return($r);
	}

	function get($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$this->_dso->new_select($this->_param['table']);
		$this->_dso->where(array('id' => $id));

		if(($r = $this->_dso->select_single()) === false)
			return(false);

		$r['valid'] = (bool) $r['valid'];
		$r['obj'] = xivo_unserialize($r['obj']);

		return($r);
	}

	function get_nb($valid=null)
	{
		$this->_dso->new_select($this->_param['table']);

		if($valid !== null)
			$this->_dso->where(array('valid' => intval((bool) $valid)));

		return((int) $this->_dso->select_count());
	}

	function get_by_login_pass($login,$pass)
	{
		$this->_dso->new_select($this->_param['table']);
		$this->_dso->where(array('login' => $login,
					 'passwd' => $pass));

		if(($r = $this->_dso->select_single()) === false)
			return(false);

		$r['valid'] = (bool) $r['valid'];
		$r['obj'] = xivo_unserialize($r['obj']);

		return($r);
	}

	function get_list($arr=array())
	{
		$arr = (array) $arr;

		$this->_dso->new_select($this->_param['table']);

		if(empty($arr) === false)
			$this->_dso->wherein('meta',$arr);

		$r = $this->_dso->select_all();

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function chk_update($id,$dupdate)
	{
		if(($id = xivo_uint($id)) === 0 || ($dupdate = xivo_uint($dupdate)) === 0)
			return(false);

		$this->_dso->new_select($this->_param['table'],'id');
		$this->_dso->where(array('id' => $id,'dupdate' => $dupdate));

		if(($r = $this->_dso->select_one()) !== false)
			$r = true;

		return($r);
	}

	function acl_edit($id,$arr)
	{
		if(($obj = $this->_get_obj($id)) === false)
			return(false);

		$obj['acl'] = $arr;

		return($this->_save_obj($id,$obj));
	}

	function _save_obj($id,$obj)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($obj) === false)
			return(false);

		$r = $this->_dso->update($this->_param['table'],
					 array('obj' => serialize($obj)),
					 array('id' => $id));

		return($r);
	}

	function _get_obj($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$this->_dso->new_select($this->_param['table'],'obj');
		$this->_dso->where(array('id' => $id));

		if(($r = $this->_dso->select_one()) === false)
			return(false);

		return(xivo_unserialize($r));
	}
}

?>
