<?php

class xivo_session_sqlite
{
	var $_param		= array('table'	=> 'session');

	var $_attrib		= array('user_id' => 0);

	var $_dso		= null;
	var $_time		= XIVO_SESS_TIME;
	var $_insert		= false;
	var $_crc		= '';

	function xivo_session_sqlite(&$dso,$param=array())
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

	function read($k)
	{
		$r = '';

		$this->_insert = true;

		$rs = $this->_dso->query('SELECT user_id, data, expire FROM '.$this->_param['table'].
			' WHERE sesskey = \''.$this->_dso->escape_string($k).'\' AND expire >= strftime(\'%s\',datetime(\'now\',\'utc\'))',true);

		if($rs !== false && $this->_dso->num_rows($rs) === 1)
		{
			$this->_insert = false;
			$rw = $this->_dso->fetch_assoc($rs);

			$r = rawurldecode($rw['data']);
			$this->_attrib['user_id'] = $rw['user_id'];

			if(empty($r) === true)
				$this->_dso->delete($this->_param['table'],'sesskey = \''.$this->_dso->escape_string($k).'\'');
		}

		$this->_crc = strlen($r.$this->_attrib['user_id']).crc32($r.$this->_attrib['user_id']);

		return($r);
	}

	function write($k,$val)
	{
		//$_USR = &xivo_gct::get('xivo_user');

		//if(isset($_USR->id) === true && $_USR->id > 0)
		//	$this->u = $_USR->id;

		$now = gmmktime();
		$expire = $now + $this->_time;

		if($this->_insert === false && empty($this->_crc) === false
		&& $this->_crc === strlen($val.$this->_attrib['user_id']).crc32($val.$this->_attrib['user_id']))
			return(true);

		$val = rawurlencode($val);

		if($this->_insert === false)
		{
			$rs = $this->_dso->query('SELECT COUNT(*) AS cnt FROM '.$this->_param['table'].
				' WHERE sesskey = \''.$this->_dso->escape_string($k).'\'');

			if($rs !== false)
			{
				$rw = $this->_dso->fetch_assoc($rs);

				if($rw['cnt'] === 0)
					$this->_insert = true;
				else
				{

					$arr = array(
						'user_id'	=> $this->_attrib['user_id'],
						'expire'	=> $expire,
						'data'		=> $val);

					$rs = $this->_dso->update($this->_param['table'],$arr,'sesskey = \''.$this->_dso->escape_string($k).'\'');

					if($rs !== false)
					{
						if($this->_dso->affected_rows() === 0)
							$this->_insert = true;
					}

				}
			}
		}

		if($this->_insert === true)
		{
			$arr = array(
				'user_id'	=> $this->_attrib['user_id'],
				'sesskey'	=> $k,
				'start'		=> $now,
				'expire'	=> $expire,
				'data'		=> $val);

			$this->_dso->replace($this->_param['table'],$arr);
		}

		return(true);
	}

	function destroy($k)
	{
		$this->_dso->delete($this->_param['table'],'sesskey = \''.$this->_dso->escape_string($k).'\'');
		return(true);
	}

	function gc($t=0)
	{
		$this->_dso->delete($this->_param['table'],'expire < strftime(\'%s\',datetime(\'now\',\'utc\'))');
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
