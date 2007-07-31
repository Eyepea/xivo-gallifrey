<?php

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'sql.inc');

class xivo_datastorage_mysql extends xivo_datastorage_abstract_sql
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
	var $_date	= array('datetime'	=> '%Y-%m-%d %H:%i:%s',
				'date'		=> '%Y-%m-%d',
				'time'		=> '%H:%i:%s');

	function xivo_datastorage_mysql($param=array())
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
			$this->_param['port'] = (int) $param['port'];
		else if(isset($param['socket']) === true)
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

		$this->_quote = (bool) get_magic_quotes_runtime();

		if($open === true && $this->open() !== false)
			$this->select_db();
	}

	function quote_identifier($name)
	{
		$r = '';

		$name = (string) $name;

		if(isset($name{0}) === true)
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

		if($this->_param['port'] !== 0 && xivo_uint($this->_param['port']) !== 0)
			$host .= ':'.$this->_param['port'];
		else if($this->_param['socket'] !== '')
			$host .= ':'.$this->_param['socket'];

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

	function query($q,$o=false)
	{
		$this->_query = (string) $q;
		$o = (bool) $o;

		if(($this->_result = mysql_query($this->_query,$this->_link)) === false && $o !== false)
			$this->halt();

		return($this->_result);
	}

	function lock($t)
	{
		$q = sprintf('LOCK TABLES %s WRITE',$t);
		return($this->query($q));
	}

	function unlock()
	{
		$q = 'UNLOCK TABLES';
		return($this->query($q));
	}

	function errno()
	{
		$r = false;

		if((bool) $this->_link !== false)
			$r = mysql_errno($this->_link);

		return($r);
	}

	function error()
	{
		$r = false;

		if((bool) $this->_link !== false)
			$r = mysql_error($this->_link);

		return($r);
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

		if($this->_quote === true)
			return($str);
		else
		{
			if(function_exists('mysql_real_escape_string') === true)
				$str = mysql_real_escape_string($str);
			else
				$str = mysql_escape_string($str);
		}
		
		return($str);
	}

	function close()
	{
		if(is_resource($this->_link) === true)
			mysql_close($this->_link);
	}

	function free($r=false)
	{
		return(mysql_free_result(($r !== false ? $r : $this->_result)));
	}

	function num_rows($r=false)
	{
		return(mysql_num_rows(($r !== false ? $r : $this->_result)));
	}

	function fetch_row($r=false)
	{
		return(mysql_fetch_row(($r !== false ? $r : $this->_result)));
	}

	function fetch_assoc($r=false)
	{
		return(mysql_fetch_assoc(($r !== false ? $r : $this->_result)));
	}

	function _expr_typetime($type='',$col='',$time='localtime',$format='')
	{
		$type = $type !== '' ? strtolower((string) $type) : 'date';
		$col = $this->quote_identifier($col);
		$time = $time !== '' ? strtolower((string) $time) : 'localtime';
		$format = (string) $format;

		if(($type !== 'unix' && $type !== 'date') === true
		|| ($time !== 'localtime' && $time !== 'utc') === true)
			return(false);

		if($type === 'unix')
			$func = 'UNIX_TIMESTAMP';
		else
			$func = 'DATE_FORMAT';

		if($col === '')
		{
			$col = ($time === 'utc' ? 'UTC_' : 'LOCAL').'TIMESTAMP()';

			if($type === 'unix')
				$col = 'UNIX_TIMESTAMP('.$col.')';
		}
		else if($time === 'utc')
		{
			$col = 'CONVERT_TZ('.$func.'('.$col.',\'%Y-%m-%d %H:%i:%s\'),@@session.time_zone,\'+00:00\')';

			if($type === 'unix')
				$col = 'UNIX_TIMESTAMP('.$col.')';
		}

		$r = $func.'('.$col.($format !== '' ? ',\''.$format.'\'' : '').')';

		return($r);
	}
}

?>
