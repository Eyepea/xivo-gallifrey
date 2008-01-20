<?php

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'adapter'.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'sql.inc');

class xivo_datastorage_adapter_mysql extends xivo_datastorage_adapter_abstract_sql
{
	var $_param	= array(
				'host'		=> 'localhost',
				'user'		=> '',
				'pass'		=> '',
				'db'		=> '',
				'port'		=> 3306,
				'socket'	=> '',
				'persist'	=> false,
				'compress'	=> false,
				'ignore_space'	=> false,
				'interactive'	=> false,
				'ssl'		=> false);

	var $_flag	= 0;
	var $_type	= 'mysql';
	var $_free	= true;
	var $_escape	= false;
	var $_date	= array(
				'datetime'	=> '%Y-%m-%d %H:%i:%s',
				'date'		=> '%Y-%m-%d',
				'time'		=> '%H:%i:%s');

	function xivo_datastorage_adapter_mysql($param=array())
	{
		$param = (array) $param;

		$open = true;

		if(isset($param['open']) === true)
			$open = false;

		if(isset($param['host']) === true)
			$this->_param['host'] = (string) $param['host'];

		if(isset($param['user']) === true)
			$this->_param['user'] = (string) $param['user'];

		if(isset($param['pass']) === true)
			$this->_param['pass'] = (string) $param['pass'];

		if(isset($param['db']) === true)
			$this->_param['db'] = (string) $param['db'];
		else
			$open = false;

		if(isset($param['port']) === true)
			$this->_param['port'] = xivo_uint($param['port']);

		if(isset($param['socket']) === true)
			$this->_param['socket'] = (string) $param['socket'];

		if(isset($param['persist']) === true)
			$this->_param['persist'] = (bool) $param['persist'];

		if(isset($param['compress']) === true)
			$this->_param['compress'] = (bool) $param['compress'];

		if(isset($param['ignore_space']) === true)
			$this->_param['ignore_space'] = (bool) $param['ignore_space'];

		if(isset($param['interactive']) === true)
			$this->_param['interactive'] = (bool) $param['interactive'];

		if(isset($param['ssl']) === true)
			$this->_param['ssl'] = (bool) $param['ssl'];

		$this->_escape = (bool) function_exists('mysql_real_escape_string'); 

		if($open === true && $this->open() !== false)
			$this->select_db();
	}

	function quote_identifier($name)
	{
		if(($r = $this->_chk_quote($name)) !== '')
			$r = '`'.str_replace('`','``',$name).'`';

		return($r);
	}

	function _set_flag()
	{
		if($this->_param['compress'] === true)
			$this->_flag |= MYSQL_CLIENT_COMPRESS;

		if($this->_param['ignore_space'] === true)
			$this->_flag |= MYSQL_CLIENT_IGNORE_SPACE;

		if($this->_param['interactive'] === true)
			$this->_flag |= MYSQL_CLIENT_INTERACTIVE;

		if($this->_param['ssl'] === true)
			$this->_flag |= MYSQL_CLIENT_SSL;
	}

	function open()
	{
		if(is_resource($this->_link) === true)
			return($this->_link);

		if($this->_param['host'] !== '')
			$host = $this->_param['host'];
		else
			$host = 'localhost';

		if($this->_param['socket'] !== '')
			$host .= ':'.$this->_param['socket'];
		else if($this->_param['port'] !== 0)
			$host .= ':'.$this->_param['port'];

		$this->_set_flag();

		if($this->_param['persist'] === true)
			$this->_link = mysql_pconnect($host,$this->_param['user'],$this->_param['pass'],false,$this->_flag);
		else
			$this->_link = mysql_connect($host,$this->_param['user'],$this->_param['pass'],false,$this->_flag);

		if($this->_link === false)
			$this->halt('Can\'t connect to mysql server');

		return($this->_link);
	}

	function select_db()
	{
		if(($r = mysql_select_db($this->_param['db'], $this->_link)) === false)
			$this->halt('Can\'t select database: '.$this->_param['db']);

		return($r);
	}

	function query($query,$buffer=false,$o=false)
	{
		$this->_query = (string) $query;
		$o = (bool) $o;

		if(($this->_result = mysql_query($this->_query,$this->_link)) === false && $o !== false)
			$this->halt();

		return($this->_result);
	}

	function lock($t)
	{
		$query = sprintf('LOCK TABLES %s WRITE',$t);
		return($this->query($query));
	}

	function unlock()
	{
		$query = 'UNLOCK TABLES';
		return($this->query($query));
	}

	function errno()
	{
		$r = 0;

		if(is_resource($this->_link) === true)
			$r = mysql_errno($this->_link);

		return($r);
	}

	function error()
	{
		$r = '';

		if(is_resource($this->_link) === true)
			$r = mysql_error($this->_link);

		return($r);
	}

	function _chk_join($type)
	{
		$type = strtoupper((string) $type);

		if($type === 'FULL')
			return(false);

		return(parent::_chk_join($type));
	}

	function affected_rows()
	{
		return(mysql_affected_rows($this->_link));
	}

	function insert_id()
	{
		return(mysql_insert_id($this->_link));
	}

	function escape_string($str)
	{
		$str = (string) $str;

		if($this->_escape === true && is_resource($this->_link) === true)
			$str = mysql_real_escape_string($str,$this->_link);
		else
			$str = mysql_escape_string($str);

		return($str);
	}

	function close()
	{
		$r = null;
	
		if(is_resource($this->_link) === true)
			$r = mysql_close($this->_link);

		$this->_link = null;

		return($r);
	}

	function free($r=false)
	{
		return(mysql_free_result((is_resource($r) === true ? $r : $this->_result)));
	}

	function num_rows($r=false)
	{
		return(mysql_num_rows((is_resource($r) === true ? $r : $this->_result)));
	}

	function fetch_row($r=false)
	{
		return(mysql_fetch_row((is_resource($r) === true ? $r : $this->_result)));
	}

	function fetch_assoc($r=false)
	{
		return(mysql_fetch_assoc((is_resource($r) === true ? $r : $this->_result)));
	}

	function unix_timestamp($column='',$time='')
	{
		if(($r = $this->_expr_typetime('date',$column,$time,$this->_date['datetime'])) !== false)
			$r = 'UNIX_TIMESTAMP('.$r.')';

		return($r);
	}

	function _expr_typetime($type='',$column='',$time='',$format='',$table='')
	{
		$column = trim((string) $column);
		$type = $type !== '' ? strtolower((string) $type) : 'date';
		$time = strtolower((string) $time);
		$format = (string) $format;
		$table = trim((string) $table);

		if(($type !== 'unix' && $type !== 'date') === true
		|| ($time !== '' && $time !== 'localtime' && $time !== 'utc') === true)
			return(false);
		else if($table === '')
			$column = $this->quote_identifier($column);
		else if(isset($this->_table[$table]) === false || $column === '')
				return(false);
		else
			$column = $this->quote_identifier($table).'.'.$this->quote_identifier($column);

		if($type === 'unix')
			$func = 'FROM_UNIXTIME';
		else
			$func = 'DATE_FORMAT';

		if($column === '')
			$column = ($time === 'utc' ? 'UTC' : 'CURRENT').'_TIMESTAMP()';
		else if($time === 'utc')
			$column = 'CONVERT_TZ('.$func.'('.$column.',\'%Y-%m-%d %H:%i:%s\'),@@session.time_zone,\'+00:00\')';
		else if($time === 'localtime')
			$column = 'CONVERT_TZ('.$func.'('.$column.',\'%Y-%m-%d %H:%i:%s\'),\'+00:00\',@@session.time_zone)';
		
		if($type === 'unix')
			$column = 'UNIX_TIMESTAMP('.$column.')';

		$r = $func.'('.$column.($format !== '' ? ',\''.$format.'\'' : '').')';

		return($r);
	}
}

?>
