<?php

xivo_file::required(array(XIVO_PATH_LIB,'session','abstract','sql.inc'),true);

class xivo_session_mysql extends xivo_session_abstract_sql
{
	function xivo_session_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}

	function read($key)
	{
		$r = '';

		$this->_insert = true;

		$rs = $this->_dso->query('SELECT user_id, data, expire FROM '.$this->_param['table'].
					 ' WHERE sesskey = \''.$this->_dso->escape_string($key).'\''.
					 ' AND expire >= UNIX_TIMESTAMP(UTC_TIMESTAMP())');

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

	function gc($t=0)
	{
		$this->_dso->delete($this->_param['table'],'expire < UNIX_TIMESTAMP(UTC_TIMESTAMP())');
		return(true);
	}
}

?>
