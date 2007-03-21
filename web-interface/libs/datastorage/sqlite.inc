<?php

class xivo_datastorage_sqlite
{
	var $_param	= array(
			'db'		=> '',
			'persist'	=> false);

	var $_date	= array();
	var $_error	= '';
	var $_errno	= 0;
	var $_link	= null;
	var $_query	= '';
	var $_quote	= false;
	var $_result	= false;
	var $_type	= 'sqlite';

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

		$this->_quote = (bool) get_magic_quotes_runtime();

		if($open === true)
			$this->open();
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
		$name = str_replace('\'','\'\'',$name);
		return('\''.$name.'\'');
	}

	function halt($str='')
	{
		$str = (string) $str;

		if(($this->_errno = $this->errno()) === false)
			$this->_errno = 'unknown';

		if(($this->_error = $this->error()) === false)
			$this->_error = 'unknown';

		if($str !== '')
			$str .= ' - ';

		if(empty($this->_query) === false)
			$str .= 'Query: '.$this->_query.' - ';

		trigger_error($str.'Error: '.$this->_error.' - Errno: '.$this->_errno,E_USER_ERROR);
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

	function is_open()
	{
		return(is_resource($this->_link));
	}

	function query($q,$b=false,$o=false)
	{
		$this->_query = (string) $q;
		$b = (bool) $b;
		$o = (bool) $o;

		if($b === true)
		{
			if(($this->_result = sqlite_query($this->_query,$this->_link)) === false && $o !== false)
				$this->halt();
		}
		else
		{
			if(($this->_result = sqlite_unbuffered_query($this->_query,$this->_link)) === false && $o !== false)
				$this->halt();
		}

		return($this->_result);
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
			{
				if($m === false)
					break;

				$r[$i] = $m;
			}
		}

		return($r);
	}

	function insert($t,&$a,$q=true)
	{
		return($this->_insertupdate('INSERT INTO',$t,$a,null,$q));
	}

	function update($t,&$a,$w='',$q=true,$o='')
	{
		return($this->_insertupdate('UPDATE',$t,$a,$w,$q,true,$o));
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
	
		$q = sprintf('DELETE FROM %s WHERE %s%s',$t,($w !== '' ? $w : 1),$o);

		return(($this->query($q) !== false ? true : false));
	}

	function where($a,$q=true)
	{
		$r = false;
	
		if(($where = $this->_separator($a,$sep=' AND ',$q,true,false)) !== false)
			$r = $where['str'];

		return($r);	
	}

	function _separator($a,$sep=',',$q=true,$set=false,$ident=true)
	{
		$r = array('str' => '','keys' => '');

		$a = (array) $a;
		$sep = (string) $sep;
		$q = (bool) $q;
		$set = (bool) $set;
		$ident = (bool) $ident;

		if(($arr = xivo_get_aks($a)) === false)
			return($r);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = &$arr['keys'][$i];

			if(is_array($a[$k]) === true && isset($a[$k]['type'],$a[$k]['data']) === true)
			{
				$type = (string) $a[$k]['type'];
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
				case 'NULL':
					if($type === 'NULL')
						$data = 'NULL';

					$f = $set === true ? '%s = %s'.$sep : '%s'.$sep;
					break;
				case 'string':
				default:
					if(is_string($data) === false)
						$data = strval($data);

					$f = $set === true ? '%s = \'%s\''.$sep : '\'%s\''.$sep;

					if($q === true)
						$data = $this->escape_string($data);
			}

			if($set === true)
				$r['str'] .= sprintf($f,($ident === true ? $this->quote_identifier($k) : $k),$data);
			else
			{
				$r['keys'] .= ($ident === true ? $this->quote_identifier($k) : $k).$sep;
				$r['str'] .= sprintf($f,$data);
			}
		}

		if(isset($r['str']{0},$sep{0}) === false)
			return(false);

		$len = strlen($sep);

		if($set === false)
			$r['keys'] = substr($r['keys'],0,-$len);

		$r['str'] = substr($r['str'],0,-$len);
/*
		if($set === true)
			$r['keys'] = implode($sep,$r['keys']);

		$r['str'] = implode($sep,$r['str']);
*/
		return($r);
	}

	function _insertupdate($p,$t,&$a,$w='',$q=true,$set=false,$o='')
	{
		$p = (string) $p;
		$t = (string) $t;
		$w = (string) $w;
		$set = (bool) $set;
		$o = (string) $o;

		$r = false;

		if(($values = $this->_separator($a,',',$q,$set)) === false)
			return($r);

		if($set === true)
			$q = sprintf('%s %s SET %s',$p,$t,$values['str']);
		else
			$q = sprintf('%s %s (%s) VALUES(%s)',$p,$t,$values['keys'],$values['str']);

		if($w !== '')
			$q .= sprintf(' WHERE %s',$w);

		if($o !== '')
			$q .= $o;

		if($this->query($q) !== false)
			$r = true;

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

	function errno()
	{
		$r = false;

		if((bool) $this->_link !== false)
			$r = sqlite_last_error($this->_link);

		return($r);
	}

	function error()
	{
		$r = false;

		if((bool) $this->_link !== false)
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
		$str = (string) $str;

		if($this->_quote === true)
			return($str);
		else
			return(sqlite_escape_string($str));
	}

	function close()
	{
		if(is_resource($this->_link) === true)
			return(sqlite_close($this->_link));
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
}

?>
