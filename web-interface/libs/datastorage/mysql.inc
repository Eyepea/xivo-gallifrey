<?php

class xivo_datastorage_mysql
{
	var $_param	= array(
			'host'		=> '',
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

	var $_date	= array();
	var $_error	= '';
	var $_errno	= 0;
	var $_flag	= 0;
	var $_link	= null;
	var $_query	= '';
	var $_quote	= false;
	var $_result	= false;
	var $_type	= 'mysql';

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

	function set_param($name,$value)
	{
		$name = (string) $name;

		$this->_param[$name] = $value;	
	}

	function get_param($name)
	{
		$name = (string) $name;

		if(isset($this->_param[$name]) === true)
			return($this->_param[$name]);
		else
			return(null);
	}

	function get_type()
	{
		return($this->_type);
	}

	function quote_identifier($name)
	{
		$name = str_replace('`','``',$name);
		return('`'.$name.'`');
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

	function halt($str='')
	{
		$str = (string) $str;

		$this->_errno = $this->errno() === false ? 'unknown' : $this->errno();
		$this->_error = $this->error() === false ? 'unknown' : $this->error();

		if($str !== '')
			$str .= ' - ';

		trigger_error($str.'Error: '.$this->_error.' - Errno: '.$this->_errno,E_USER_ERROR);
	}

	function open()
	{
		if($this->_link === null)
		{
			$host = '';

			if($this->_param['host'] !== '')
				$host .= ':'.$this->_param['host'];

			if($this->_param['port'] !== 0 && xivo_uint($this->_param['port']) !== 0)
				$host .= ':'.$this->_param['port'];
			else if($this->_param['socket'] !== '')
				$host .= ':'.$this->_param['socket'];

			if($host !== '')
				$host = substr($host,1);

			$this->_set_flag();

			if($this->_param['persist'] === true)
				$this->_link = mysql_pconnect($host,$this->_param['user'],$this->_param['pass'],false,$this->_flag);
			else
				$this->_link = mysql_connect($host,$this->_param['user'],$this->_param['pass'],false,$this->_flag);

			if($this->_link === false)
				$this->halt('Can\'t connect to mysql server');
		}

		return($this->_link);
	}

	function select_db()
	{
		if(($r = mysql_select_db($this->_db, $this->_link)) === false)
			$this->halt('Can\'t select database: '.$this->_db);

		return($r);
	}

	function query($q,$o=false)
	{
		$this->_query = (string) $q;
		$o = (bool) $o;

		if(($this->_result = mysql_query($this->_query,$this->_link)) === false && $o !== false)
			$this->halt();

		return($this->r);
	}

	function select_hash($t,$w='',$o='')
	{
		$t = (string) $t;
		$w = (string) $w;
		$o = (string) $o;

		$r = false;
		$q = sprintf('SELECT * FROM %s WHERE %s%s',$t,($w !== '' ? $w : 1),$o);

		if($this->query($q) !== false)
		{
			$r = array();

			for($i = 0;$m = $this->fetch_assoc();$i++)
				$r[$i] = $m;

			$this->free_result();
		}

		return($r);
	}

	function insert($t,&$a,$q=true)
	{
		return($this->_insertupdate('INSERT INTO',$t,$a,null,$q));
	}

	function update($t,&$a,$w='',$q=true,$o='')
	{
		return($this->_insertupdate('UPDATE',$t,$a,$w,$q,$o));
	}

	function replace($t,&$a,$q=true)
	{
		return($this->_insertupdate('REPLACE INTO',$t,$a,null,$q));
	}

	function delete($t,$w='',$o='')
	{
		$t = (string) $t;
		$w = (string) $w;
		$o = (string) $o;

		return($this->query(sprintf('DELETE FROM %s WHERE %s%s',$t,($w !== '' ? $w : 1),$o)));
	}

	function _insertupdate($p,$t,&$a,$w='',$q=true,$o='')
	{
		$p = (string) $p;
		$t = (string) $t;
		$a = (array) $a;
		$w = (string) $w;
		$q = (bool) $q;
		$o = (string) $o;

		$r = false;
		$str = '';

		if(($arr = xivo_get_aks($a)) === false)
			return($r);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = &$arr['keys'][$i];

			if(is_array($a[$k]) === true && isset($a[$k]['type'],$a[$k]['data']) === true)
			{
				$type = $a[$k]['type'];
				$data = &$a[$k]['data'];
			}
			else
			{
				$type = gettype($a[$k]);
				$data = &$a[$k];
			}

			switch($type)
			{
				case 'boolean':
					$data = intval($data);
				case 'func':
				case 'integer':
				case 'float':
					$f = '%s = %s,';
					$data = (string) $data;
					break;
				default:
				case 'string':
					$f = '%s = \'%s\',';

					if($q === true)
						$data = $this->escape_string($data);
			}
				
			$str .= sprintf($f,$this->quote_identifier($k),$data);
		}

		if($str === '')
			return($r);

		$str = substr($str,0,-1);
		$q = sprintf('%s %s SET %s',$p,$t,$str);

		if($w !== '')
			$q .= sprintf(' WHERE %s',$w);

		if($o !== '')
			$q .= $o;

		$r = $this->query($q);

		return($r);
	}

	function date($k,$date='',$format=0,$utc=false)
	{
		$r = false;

		$k = (string) $k;
		$date = (array) $date;
		$format = xivo_uint($format);
		$utc = (bool) $utc;

		if($format !== 2)
			$utc = false;


		if(isset($date['beg'],$date['sql']) === false)
			return($r);

		$col = $col_beg = $k;

		if(is_array($date['beg']) === true)
		{
			$k .= '-'.$date['beg']['int'];
			if(is_array($date['end']) === true)
				$k .= '-'.$date['end']['int'];
		}

		if($utc === true)
			$k .= '-utc';

		if(isset($this->_date[$k]) === false)
		{
			$arr = array(1 => '%Y',2 => '%Y%m',3 => '%Y%m%d');
			$this->_date[$k] = array('date' => '','unix' => '');

			$f = $arr[$date['cnt']];

			$dbeg = $date['beg']['int'];

			if(is_array($date['end']) === true)
				$dend = $date['end']['int'];
			else
				$dend = null;

			if($utc === true)
			{
				$tz_beg = strftime('%z',$date['beg']['unix']);
				$tz_beg_sql = sprintf('%s%02d:%02d',$tz_beg{0},(int) substr($tz_beg,1,2),(int) substr($tz_beg,-2));

				$col = 'FROM_UNIXTIME('.$col.',\'%Y-%m-%d %H.%i.%s\')';

				$col_beg = 'CONVERT_TZ('.$col.',\'+00:00\',\''.$tz_beg_sql.'\')';

				if($dend !== null)
				{
					if(($tz_end = strftime('%z',$date['end']['unix'])) === $tz_beg)
						$this->_date[$k]['unix'] = 'DATE_FORMAT('.$col_beg.',\''.$f.'\') BETWEEN '.$dbeg.' AND '.$dend;
					else
					{
						$tz_end_sql = sprintf('%s%02d:%02d',$tz_end{0},substr($tz_end,1,2),substr($tz_end,-2));
						$col_end = 'CONVERT_TZ('.$col.',\'+00:00\',\''.$tz_end_sql.'\')';

						$this->_date[$k]['unix'] = 'DATE_FORMAT('.$col_beg.',\''.$f.'\') >= '.$dbeg.' AND '.
									  $dend.' <= DATE_FORMAT('.$col_end.',\''.$f.'\')';
					}
				}
				else
					$this->_date[$k]['unix'] = 'DATE_FORMAT('.$col_beg.',\''.$f.'\') = '.$dbeg;
			}
			else if($dend === null)
			{
				$this->_date[$k]['date'] = 'DATE_FORMAT('.$col_beg.',\''.$f.'\') = '.$dbeg;
				$this->_date[$k]['unix'] = 'FROM_UNIXTIME('.$col_beg.',\''.$f.'\') = '.$dbeg;
			}
			else
			{
				$this->_date[$k]['date'] = 'DATE_FORMAT('.$col_beg.',\''.$f.'\') BETWEEN '.$dbeg.' AND '.$dend;
				$this->_date[$k]['unix'] = 'FROM_UNIXTIME('.$col_beg.',\''.$f.'\') BETWEEN '.$dbeg.' AND '.$dend;
			}
		}

		switch($format)
		{
			case 1:
				$r = &$this->_date[$k]['date'];
				break;
			case 2:
				$r = &$this->_date[$k]['unix'];
				break;
			default:
				$r = &$this->_date[$k];
		}

		return($r);
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
		mysql_close($this->_link);
	}

	function free_result($r=false)
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
}

?>
