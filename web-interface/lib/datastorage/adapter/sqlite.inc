<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

require_once(xivo_file::joinpath(XIVO_PATH_LIB,'datastorage','adapter','abstract','sql.inc'));

class xivo_datastorage_adapter_sqlite extends xivo_datastorage_adapter_abstract_sql
{
	var $_param			= array(
						'db'			=> '',
						'persist'		=> false,
						'short_column_names'	=> true,
						'timeout'		=> 150);

	var $_type			= 'sqlite';
	var $_date			= array(
						'datetime'	=> '%Y-%m-%d %H:%M:%S',
						'date'		=> '%Y-%m-%d',
						'time'		=> '%H:%M:%S');
	var $_fix_leftjoin_column	= false;

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
		if(($r = $this->_get_scalar($name,true)) !== '')
			$r = '"'.str_replace('"','""',$r).'"';

		return($r);
	}

	function _chk_db()
	{
		if(($db = xivo_file::is_f($this->_param['db'])) !== false)
			return(null);
		else if(($pos = strrpos($this->_param['db'],'.')) === false || substr($this->_param['db'],1-$pos) !== '.db')
			$this->_param['db'] .= '.db';

		$this->_param['db'] = XIVO_PATH_DS.XIVO_SEP_DIR.$this->_type.XIVO_SEP_DIR.ltrim($this->_param['db'],XIVO_SEP_DIR);

		if(($db = xivo_file::is_f($this->_param['db'])) === false)
			$this->halt('Can\'t find database SQLite');
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

		if((bool) $this->_param['persist'] === false)
			$this->_link = sqlite_open($this->_param['db'],0666);
		else
			$this->_link = sqlite_popen($this->_param['db'],0666);

		if($this->_link === false)
			$this->halt();

		if(isset($this->_param['short_column_names']) === true)
			$this->set_pragma('short_column_names',$this->_param['short_column_names']);

		if(isset($this->_param['timeout']) === true)
			$this->timeout($this->_param['timeout']);

		return($this->_link);
	}

	function query($query,$buffer=false)
	{
		if(is_string($query) === false)
			return(false);

		$this->_query = $query;

		if((bool) $buffer === false)
			$this->_result = sqlite_unbuffered_query($this->_query,$this->_link);
		else
			$this->_result = sqlite_query($this->_query,$this->_link);

		if($this->_result === false && (bool) $this->_debug === true)
			$this->halt();

		return($this->_result);
	}

	function errno()
	{
		if(is_resource($this->_link) === true)
			return(sqlite_last_error($this->_link));

		return(0);
	}

	function error()
	{
		if(is_resource($this->_link) === true)
			return(sqlite_error_string($this->_link));

		return('');
	}

	function timeout($time)
	{
		if(($time = xivo_uint($time)) < 60000 && is_resource($this->_link) === true)
			return(sqlite_busy_timeout($this->_link,$time));

		return(false);
	}

	function affected_rows()
	{
		if(is_resource($this->_link) === true)
			return(sqlite_changes($this->_link));

		return(false);
	}

	function insert_id()
	{
		if(is_resource($this->_link) === true)
			return(sqlite_last_insert_rowid($this->_link));

		return(false);
	}

	function escape_string($str)
	{
		return(sqlite_escape_string($str));
	}

	function close()
	{
		$r = null;

		if(is_resource($this->_link) === true)
			$r = sqlite_close($this->_link);

		$this->_link = null;

		return($r);
	}

	function num_rows()
	{
		if(is_resource($this->_result) === true)
			return(sqlite_num_rows($this->_result));

		return(false);
	}

	function fetch_row()
	{
		if(is_resource($this->_result) === true)
			return(sqlite_fetch_array($this->_result,SQLITE_NUM));

		return(false);
	}

	function fetch_assoc()
	{
		if(is_resource($this->_result) === true)
			return(sqlite_fetch_array($this->_result,SQLITE_ASSOC));

		return(false);
	}

	function unix_timestamp($column='',$time='')
	{
		return($this->_expr_typetime(null,$column,$time,'%s'));
	}

	function _escape_like($str)
	{
		return($str);
	}

	function _fix_leftjoin_condition($val)
	{
		if($this->is_escaped($val) === false
		&& is_int($val) === false
		&& is_float($val) === false
		&& is_bool($val) === false)
			return($val);
		else if(xivo_haslen($this->_fix_leftjoin_column) === false)
			return($val);

		return(sprintf('SUBSTR(%s,0,0)||%s',$this->_fix_leftjoin_column,$val));
	}

	function _expr_typetime($type='',$column='',$time='',$format='',$table='')
	{
		$column = trim($column);
		$time = strtolower($time);
		$format = (string) $format;
		$table = trim($table);

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
