<?php

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'sql.inc');

class xivo_datastorage_sqlite extends xivo_datastorage_abstract_sql
{
	var $_param	= array(
			'db'		=> '',
			'persist'	=> false);

	var $_type	= 'sqlite';
	var $_date	= array('datetime'	=> '%Y-%m-%d %H:%M:%S',
				'date'		=> '%Y-%m-%d',
				'time'		=> '%H:%M:%S');

	function xivo_datastorage_sqlite($param=array())
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

		if($open === true)
			$this->open();
	}

	function quote_identifier($name)
	{
		$r = '';

		$name = (string) $name;

		if(isset($name{0}) === true)
			$r = '"'.str_replace('"','""',$name).'"';
		
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

		return($this->_link);
	}

	function query($q,$b=false,$o=false)
	{
		$this->_query = (string) $q;
		$b = (bool) $b;
		$o = (bool) $o;

		if($b === true)
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
		return(sqlite_num_rows(($r !== false ? $r : $this->_result)));
	}

	function fetch_row($r=false)
	{
		return(sqlite_fetch_array(($r !== false ? $r : $this->_result),SQLITE_NUM));
	}

	function fetch_assoc($r=false)
	{
		return(sqlite_fetch_array(($r !== false ? $r : $this->_result),SQLITE_ASSOC));
	}

	function unix_timestamp($col='',$time='localtime')
	{
		return($this->_expr_typetime(null,$col,$time,'%s'));
	}

	function _expr_typetime($type='',$col='',$time='',$format='')
	{
		$col = $this->quote_identifier($col);
		$time = $time !== '' ? strtolower((string) $time) : 'localtime';
		$format = (string) $format;

		if($time !== 'localtime' && $time !== 'utc')
			return(false);

		if($col === '')
			$col = '\'now\'';

		$col = 'datetime('.$col.',\''.$time.'\')';
		
		if($format === '')
			$r = $col;
		else
			$r = 'strftime(\''.$format.'\','.$col.')';

		return($r);
	}
}

?>
