<?php

class xivo_session_abstract_sql
{
	var $_param		= array('table'	=> 'session');
	var $_attrib		= array('user_id' => 0);
	var $_dso		= null;
	var $_time		= XIVO_SESS_TIME;
	var $_insert		= false;
	var $_crc		= '';

	function _init(&$dso,$param=array())
	{
		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		$param = (array) $param;

		if(isset($param['table']) === true)
			$this->_param['table'] = (string) $param['table'];
	}

	function set_attrib($name,$value)
	{
		$name = (string) $name;
		$this->_attrib[$name] = $value;
	}

	function get_attrib($name)
	{
		$name = (string) $name;

		if(isset($this->_attrib[$name]) === true)
			return($this->_attrib[$name]);
		else
			return(null);
	}

	function open($path,$name)
	{
		return(true);
	}

	function close()
	{
		return(true);
	}

	function read($key)
	{
		$r = '';

		$this->_insert = true;

		$rs = $this->_dso->query('SELECT user_id, data, expire FROM '.$this->_param['table'].
					 ' WHERE sesskey = \''.$this->_dso->escape_string($key).'\''.
					 ' AND expire >= '.gmmktime(),true);

		if($rs !== false)
		{
			if($this->_dso->num_rows($rs) === 1)
			{
				$this->_insert = false;
				$rw = $this->_dso->fetch_assoc($rs);

				$r = rawurldecode($rw['data']);
				$this->_attrib['user_id'] = $rw['user_id'];

				if(empty($r) === true)
					$this->_dso->delete($this->_param['table'],'sesskey = \''.$this->_dso->escape_string($key).'\'');
			}

			if($this->_dso->is_free() === true)
				$this->_dso->free($rs);
		}

		$this->_crc = strlen($r.$this->_attrib['user_id']).crc32($r.$this->_attrib['user_id']);

		return($r);
	}

	function write($key,$val)
	{
		/*
		$_USR = &xivo_gct::get('xivo_user');

		if(isset($_USR->id) === true && $_USR->id > 0)
			$this->u = $_USR->id;
		*/

		$now = gmmktime();
		$expire = $now + $this->_time;
		$crc = strlen($val.$this->_attrib['user_id']).crc32($val.$this->_attrib['user_id']);

		if($this->_insert === false && empty($this->_crc) === false && $this->_crc === $crc)
			return(true);

		$val = rawurlencode($val);

		if($this->_insert === false)
		{
			$where = 'sesskey = \''.$this->_dso->escape_string($key).'\'';

			if(($r = $this->_dso->select_count($this->_param['table'],null,$where)) !== false)
			{
				if($r > 0)
				{
					$arr = array();
					$arr['user_id'] = $this->_attrib['user_id'];
					$arr['expire'] = $expire;
					$arr['data'] = $val;

					$rs = $this->_dso->update($this->_param['table'],$arr,$where);

					if($rs !== false && $this->_dso->affected_rows() === 0)
						$this->_insert = true;
				}
				else
					$this->_insert = true;
			}
		}

		if($this->_insert !== true)
			return(true);

		$arr = array();
		$arr['user_id'] = $this->_attrib['user_id'];
		$arr['sesskey'] = $key;
		$arr['start'] = $now;
		$arr['expire'] = $expire;
		$arr['data'] = $val;

		$this->_dso->replace($this->_param['table'],$arr);

		return(true);
	}

	function destroy($key)
	{
		$this->_dso->delete($this->_param['table'],'sesskey = \''.$this->_dso->escape_string($key).'\'');
		return(true);
	}

	function gc($t=0)
	{
		$this->_dso->delete($this->_param['table'],'expire < '.gmmktime());
		return(true);
	}

	function write_close()
	{
		$this->gc();
		session_write_close();

		return(true);
	}
}

?>
