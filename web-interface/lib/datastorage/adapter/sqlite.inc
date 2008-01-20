<?php

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'adapter'.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'sql.inc');

class xivo_datastorage_adapter_sqlite extends xivo_datastorage_adapter_abstract_sql
{
	var $_param	= array(
				'db'			=> '',
				'persist'		=> false,
				'short_column_names'	=> true,
				'timeout'		=> 150);

	var $_type	= 'sqlite';
	var $_date	= array(
				'datetime'	=> '%Y-%m-%d %H:%M:%S',
				'date'		=> '%Y-%m-%d',
				'time'		=> '%H:%M:%S');

	function xivo_datastorage_adapter_sqlite($param=array())
	{
		$param = (array) $param;
		$open = true;

		if(isset($param['open']) === true)
			$open = false;

		if(isset($param['db']) === true)
			$this->_param['db'] = (string) $param['db'];
		else
			$open = false;

		if(isset($param['persist']) === true)
			$this->_param['persist'] = (bool) $param['persist'];

		if(isset($param['short_column_names']) === true)
			$this->_param['short_column_names'] = (bool) $param['short_column_names'];

		if(isset($param['timeout']) === true && ($timeout = xivo_uint($param['timeout'])) < 60000)
			$this->_param['timeout'] = xivo_uint($param['timeout']);

		if($open === true)
			$this->open();
	}

	function quote_identifier($name)
	{
		if(($r = $this->_chk_quote($name)) !== '')
			$r = '"'.str_replace('"','""',$r).'"';

		return($r);
	}

	function _chk_db()
	{
		if(($db = xivo_file::is_f($this->_param['db'])) !== false)
			return(null);

		if(($pos = strrpos($this->_param['db'],'.')) === false || substr($this->_param['db'],1-$pos) !== '.db')
			$this->_param['db'] .= '.db';

		$this->_param['db'] = XIVO_PATH_DS.XIVO_SEP_DIR.$this->_type.XIVO_SEP_DIR.ltrim($this->_param['db'],XIVO_SEP_DIR);

		if(($db = xivo_file::is_f($this->_param['db'])) === false)
			$this->halt('Can\'t find database');
	}

	function _chk_pragma($key,$value)
	{
		$key = strtolower($key);

		switch($key)
		{
			case 'full_column_names':
			case 'short_column_names':
			case 'count_changes':
			case 'empty_result_callbacks':
				$value = intval((bool) $value);
				break;
			case 'cache_size':
			case 'default_cache_size':
				if(($value = xivo_uint($value)) === 0 || $value > 4000)
					return(false);
				break;
			case 'synchronous':
				if(($value = strtoupper($value)) !== 'FULL'
				&& $value !== 'NORMAL'
				&& $value !== 'OFF'
				&& $value !== '2'
				&& $value !== '1'
				&& $value !== '0')
					return(false);
				break;
			case 'database_list':
			case 'foreign_key_list':
			case 'index_info':
			case 'index_list':
			case 'table_info':
				$value = $this->quote_identifier($value);
				break;
			case 'temp_store':
				if(($value = strtoupper($value)) !== 'DEFAULT'
				&& $value !== 'FILE'
				&& $value !== 'MEMORY'
				&& $value !== '2'
				&& $value !== '1'
				&& $value !== '0')
					return(false);
				break;
			case 'integrity_check':
				if(($value = xivo_uint($value)) === 0 || $value > 200)
					return(false);
				break;
			case 'parser_trace':
			case 'vdbe_trace':
				$value = intval((bool) $value);
				break;
			default:
				return(false);
		}
		return($value);
	}

	function set_pragma($key,$value)
	{
		if(is_resource($this->_link) === false
		|| ($value = $this->_chk_pragma($key,$value)) === false)
			return(false);

		$query = sprintf('PRAGMA %s = %s',strtolower($key),$value);

		return($this->query($query));
	}

	function open()
	{
		if(is_resource($this->_link) === true)
			return($this->_link);

		$this->_chk_db();

		if($this->_param['persist'] === true)
			$this->_link = sqlite_popen($this->_param['db'],0666);
		else
			$this->_link = sqlite_open($this->_param['db'],0666);

		if($this->_link === false)
			$this->halt();

		if(isset($this->_param['short_column_names']) === true)
			$this->set_pragma('short_column_names',$this->_param['short_column_names']);
		
		if(isset($this->_param['timeout']) === true)
			$this->timeout($this->_param['timeout']);

		return($this->_link);
	}

	function query($query,$buffer=false,$o=false)
	{
		$this->_query = (string) $query;
		$buffer = (bool) $buffer;
		$o = (bool) $o;

		if($buffer === true)
			$this->_result = sqlite_query($this->_query,$this->_link);
		else
			$this->_result = sqlite_unbuffered_query($this->_query,$this->_link);

		if($this->_result === false && $o !== false)
			$this->halt();

		return($this->_result);
	}

	function errno()
	{
		$r = 0;

		if(is_resource($this->_link) === true)
			$r = sqlite_last_error($this->_link);

		return($r);
	}

	function error()
	{
		$r = '';

		if(is_resource($this->_link) === true)
			$r = sqlite_error_string($this->_link);

		return($r);
	}

	function timeout($time)
	{
		if(($time = xivo_uint($time)) >= 60000)
			return(false);

		return(sqlite_busy_timeout($this->_link,$time));
	}

	function affected_rows()
	{
		return(sqlite_changes($this->_link));
	}

	function insert_id()
	{
		return(sqlite_last_insert_rowid($this->_link));
	}

	function escape_string($str)
	{
		return(sqlite_escape_string((string) $str));
	}

	function close()
	{
		$r = null;

		if(is_resource($this->_link) === true)
			$r = sqlite_close($this->_link);

		$this->_link = null;

		return($r);
	}

	function num_rows($r=false)
	{
		return(sqlite_num_rows((is_resource($r) === true ? $r : $this->_result)));
	}

	function fetch_row($r=false)
	{
		return(sqlite_fetch_array((is_resource($r) === true ? $r : $this->_result),SQLITE_NUM));
	}

	function fetch_assoc($r=false)
	{
		return(sqlite_fetch_array((is_resource($r) === true ? $r : $this->_result),SQLITE_ASSOC));
	}

	function unix_timestamp($column='',$time='')
	{
		return($this->_expr_typetime(null,$column,$time,'%s'));
	}

	function _expr_typetime($type='',$column='',$time='',$format='',$table='')
	{
		$column = trim((string) $column);
		$time = strtolower((string) $time);
		$format = (string) $format;
		$table = trim((string) $table);

		if($time !== '' && $time !== 'localtime' && $time !== 'utc')
			return(false);
		else if($table === '')
			$column = $this->quote_identifier($column);
		else if(isset($this->_table[$table]) === false || $column === '')
				return(false);
		else
			$column = $this->quote_identifier($table).'.'.$this->quote_identifier($column);

		if($column === '')
			$column = '\'now\'';

		if($format === '' && $time === '')
			$r = 'datetime('.$column.')';
		if($format === '' && $time !== '')
			$r = 'datetime('.$column.',\''.$time.'\')';
		else if($time === '')
			$r = 'strftime(\''.$format.'\',datetime('.$column.'))';
		else
			$r = 'strftime(\''.$format.'\',datetime('.$column.'),\''.$time.'\')';

		return($r);
	}
}

?>
