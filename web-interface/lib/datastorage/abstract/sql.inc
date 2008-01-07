<?php

class xivo_datastorage_abstract_sql
{
	var $_error	= '';
	var $_errno	= 0;
	var $_query	= '';
	var $_quote	= false;
	var $_link	= null;
	var $_result	= false;
	var $_debug	= false;
	var $_free	= false;
	var $_where	= array();
	var $_group	= array();
	var $_order	= array();
	var $_limit	= array();

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

	function set_debug($debug=false)
	{
		$this->_debug = (bool) $debug;
	}

	function get_type()
	{
		return($this->_type);
	}

	function halt($str='')
	{
		$str = (string) $str;

		if(($this->_errno = $this->errno()) === 0)
			$this->_errno = 'unknown';

		if(($this->_error = $this->error()) === '')
			$this->_error = 'unknown';

		if($str !== '')
			$str .= ' - ';

		if($this->_debug === true && empty($this->_query) === false)
			$str .= 'Query: '.$this->_query.' - ';

		trigger_error($str.'Error: '.$this->_error.' - Errno: '.$this->_errno,E_USER_ERROR);
	}

	function is_open()
	{
		return(is_resource($this->_link));
	}

	function is_free()
	{
		return((bool) $this->_free);
	}

	function free()
	{
		return(false);
	}

	function escape($val)
	{
		switch(gettype($val))
		{
			case 'boolean':
				$val = intval($val);
			case 'integer':
			case 'float':
				return($val);
			case 'NULL':
				return('NULL');
		}

		return('\''.$this->escape_string(strval($val)).'\'');
	}

	function select_one($t,$s='',$w='',$o='')
	{
		$t = trim((string) $t);
		$s = trim((string) $s);
		$w = trim((string) $w);
		$o = trim((string) $o);
		$this->limit(1);

		if($s === '')
			$s = '*';

		if($o !== '')
			$o = ' '.$o;

		$o .= $this->_mk_group();
		$o .= $this->_mk_order();
		$o .= $this->_mk_limit();
		$this->limit();

		$q = sprintf('SELECT %s FROM %s WHERE %s%s',$s,$t,($w !== '' ? $w : 1),$o);

		if($this->query($q) === false)
			return(false);

		$r = $this->fetch_assoc();

		if($this->_free === true)
			$this->free();

		return($r);
	}

	function select_all($t,$s='',$w='',$o='',$f=true,$u=false)
	{
		$t = trim((string) $t);
		$s = trim((string) $s);
		$w = trim((string) $w);
		$o = trim((string) $o);
		$f = (bool) $f;
		$u = (bool) $u;

		if($s === '')
			$s = '*';

		if($o !== '')
			$o = ' '.$o;

		$o .= $this->_mk_group();
		$o .= $this->_mk_order();
		$o .= $this->_mk_limit();

		if($u === true && strpos($s,'*') === false)
			$s .= ' AS __UNIQUE__';
		else
			$u = false;

		$q = sprintf('SELECT %s FROM %s WHERE %s%s',$s,$t,($w !== '' ? $w : 1),$o);

		if($this->query($q) === false)
			return(false);

		$r = array();

		if($u === true)
		{
			for($i = 0;$m = $this->fetch_assoc();$i++)
				$r[$i] = $m['__UNIQUE__'];
		}
		else
		{
			for($i = 0;$m = $this->fetch_assoc();$i++)
				$r[$i] = $m;
		}

		if($this->_free === true && $f === true)
			$this->free();

		return($r);
	}

	function select_count($t,$s='',$w='',$o='')
	{
		$t = trim((string) $t);
		$s = trim((string) $s);
		$w = trim((string) $w);
		$o = trim((string) $o);

		if($s === '')
			$s = '*';

		if($o !== '')
			$o = ' '.$o;

		$q = sprintf('SELECT COUNT(%s) FROM %s WHERE %s%s',$s,$t,($w !== '' ? $w : 1),$o);

		if($this->query($q) === false)
			return(false);

		if(($r = $this->fetch_row()) !== false)
			$r = (int) $r[0];

		if($this->_free === true)
			$this->free();

		return($r);
	}

	function insert($t,&$data,$escape=true)
	{
		return($this->_insertupdate('insert',$t,$data,null,$escape));
	}

	function update($t,&$data,$w='',$escape=true,$o='')
	{
		return($this->_insertupdate('update',$t,$data,$w,$escape,true,$o));
	}

	function replace($t,&$data,$escape=true)
	{
		return($this->_insertupdate('replace',$t,$data,null,$escape));
	}

	function delete($t,$w='',$o='')
	{
		$t = trim((string) $t);
		$w = trim((string) $w);
		$o = trim((string) $o);

		if($o !== '')
			$o = ' '.$o;

		$o .= $this->_mk_order();
		$o .= $this->_mk_limit();

		$q = sprintf('DELETE FROM %s WHERE %s%s',$t,($w !== '' ? $w : 1),$o);

		if(($r = $this->query($q)) !== false)
			$r = (bool) $this->affected_rows();

		return($r);
	}

	function join($t,$c='INNER',$o='')
	{
		$t = trim((string) $t);
		$c = strtoupper(trim((string) $c));
		$o = trim((string) $o);

		if($t === '')
			return(false);

		if($o !== '')
			$o = ' ON '.$o;

		switch($c)
		{
			case 'LEFT':
			case 'RIGHT':
			case 'CROSS':
			case 'FULL':
			case 'INNER':
				break;
			case 'NATURAL':
				$o = '';
				break;
			case '':
				$c = 'INNER';
			default:
				return(false);
		}

		return(sprintf('%s JOIN %s%s',$c,$t,$o));
	}

	function new_where($data,$not=false,$table='',$escape=true)
	{
		$this->_where = array();

		return($this->add_where($data,$not,$table,$escape));
	}

	function add_where($data,$not=false,$table='',$escape=true)
	{
		if(($where = $this->where($data,$table,$escape,$not)) === false)
			return(false);

		$this->_where[] = $where;
		return(true);
	}

	function get_where($type='')
	{
		if(strtoupper($type) === 'OR')
			return(implode(' OR ',$this->_where));
		else
			return(implode(' AND ',$this->_where));
	}

	function where($data,$table='',$escape=true,$not=false,$sep='')
	{
		$not = (bool) $not === true ? '!=' : '=';
	
		if(strtoupper($sep) === 'OR')
			$sep = ' OR ';
		else
			$sep = ' AND ';

		if(($where = $this->_separator($data,$sep,$escape,true,false,$not,$table)) === false)
			return(false);

		$r = $where['str'];

		if(trim($r) === '')
			$r = false;

		return($r);
	}

	function where_not($data,$table='',$escape=true,$sep='')
	{
		return($this->where($data,$table,$escape,true,$sep));
	}

	function _insertupdate($p,$t,&$data,$w='',$escape=true,$set=false,$o='',$order=false,$limit=false)
	{
		$p = strtolower((string) $p);
		$t = $this->quote_identifier(trim((string) $t));
		$w = trim((string) $w);
		$set = (bool) $set;
		$o = trim((string) $o);
		$order = (bool) $order;
		$limit = (bool) $limit;

		$r = false;

		if(($values = $this->_separator($data,',',$escape,$set)) === false)
			return($r);

		if($o !== '')
			$o = ' '.$o;

		switch($p)
		{
			case 'insert':
			case 'replace':
				$p = strtoupper($p).' INTO';
				break;
			case 'update':
				$p = 'UPDATE';

				if($order === true)
					$o .= $this->_mk_order();

				if($limit === true)
					$o .= $this->_mk_limit();
				break;
			default:
				return($r);
		}

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

	function _separator($val,$sep=',',$escape=true,$set=false,$ident=true,$oper='=',$table='')
	{
		$r = array('str' => '','keys' => '');

		$val = (array) $val;
		$sep = (string) $sep;
		$escape = (bool) $escape;
		$set = (bool) $set;
		$ident = (bool) $ident;
		$oper = (string) $oper;
		$table = trim((string) $table);

		if($table !== '')
			$ident = false;

		if(($arr = xivo_get_aks($val)) === false)
			return($r);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$data = &$val[$key];

			$type = gettype($data);

			switch($type)
			{
				case 'boolean':
					$data = intval($data);
				case 'integer':
				case 'float':
				case 'NULL':
					if($type === 'NULL')
						$data = 'NULL';

					$f = $set === true ? '%s %s %s'.$sep : '%s'.$sep;
					break;
				default:
					$data = strval($data);
				case 'string':
					$f = $set === true ? '%s %s \'%s\''.$sep : '\'%s\''.$sep;

					if($escape === true)
						$data = $this->escape_string($data);
			}

			if($set === true)
			{
				if($table !== '')
					$key = $this->quote_identifier($table).'.'.$this->quote_identifier($key);
				else if($ident === true)
					$key = $this->quote_identifier($key);

				$r['str'] .= sprintf($f,$key,$oper,$data);
			}
			else
			{
				if($table !== '')
					$key = $this->quote_identifier($table).'.'.$key;
				else if($ident === true)
					$key = $this->quote_identifier($key);

				$r['keys'] .= $key.$sep;
				$r['str'] .= sprintf($f,$data);
			}
		}

		if(isset($r['str']{0},$sep{0}) === false)
			return(false);

		$len = strlen($sep);

		if($set === false)
			$r['keys'] = substr($r['keys'],0,-$len);

		$r['str'] = substr($r['str'],0,-$len);

		return($r);
	}

	function between_value($col,$beg,$end)
	{
		$r = '';
	
		$col = $this->quote_identifier($col);
		$beg = $this->escape_string($beg);
		$end = $this->escape_string($end);

		if(isset($col{0},$beg{0},$end{0}) === false)
			return($r);

		$r = sprintf('%s BETWEEN \'%s\' AND \'%s\'',$col,$beg,$end);

		return($r);
	}

	function between_column($val,$beg,$end)
	{
		$r = '';

		$beg = $this->quote_identifier($beg);
		$end = $this->quote_identifier($end);

		if(isset($beg{0},$end{0}) === false)
			return($r);

		switch(gettype($val))
		{
			case 'integer':
			case 'float':
				break;
			default:
				$val = '\''.$this->escape_string(strval($val)).'\'';
		}

		$r = sprintf('%s BETWEEN %s AND %s',$val,$beg,$end);

		return($r);
	}
	function new_group($col='',$table='')
	{
		$this->_group = array();

		return($this->group($col,$table));
	}

	function reset_group()
	{
		$this->_group = array();
	}

	function group($col='',$table='')
	{
		$table = trim((string) $table);

		if(is_array($col) === true)
		{
			$col = array_values($col);

			if(($nb = count($col)) === 0)
				return(false);

			$group = array();

			for($i = 0;$i < $arr['cnt'];$i++)
			{
				$ref = &$col[$i];

				if(xivo_haslen($ref) === false)
					continue;

				if($table !== '')
					$group[] = $this->quote_identifier($table).'.'.$this->quote_identifier($ref);
				else
					$group[] = $this->quote_identifier($ref);
			}

			if(isset($group[0]) === false)
				return(false);

			$this->_group = array_merge($this->_group,$group);

			return(true);
		}

		$col = trim((string) $col);

		if($col !== '')
		{
			if($table !== '')
				$this->_group[] = $this->quote_identifier($table).'.'.$this->quote_identifier($col);
			else
				$this->_group[] = $this->quote_identifier($col);

			return(true);
		}

		$this->_group = array();
		return(null);
	}

	function _mk_group()
	{
		$r = '';

		if(is_array($this->_group) === false || isset($this->_group[0]) === false)
			return($r);

		if(($r = implode(',',$this->_group)) !== '')
			$r = ' GROUP BY '.$r;

		return($r);
	}

	function new_order($col='',$sort='',$table='')
	{
		$this->_order = array();

		return($this->order($col,$sort,$table));
	}

	function reset_order()
	{
		$this->_order = array();
	}

	function order($col='',$sort='',$table='')
	{
		$table = trim((string) $table);

		if(is_array($col) === true)
		{
			if(($arr = xivo_get_aks($col)) === false)
				return(false);

			$order = array();

			for($i = 0;$i < $arr['cnt'];$i++)
			{
				$key = &$arr['keys'][$i];
				$val = $col[$key];

				if($key === '' || ($val = $this->_chk_sort($val)) === false)
					continue;

				if($table !== '')
					$key = $this->quote_identifier($table).'.'.$this->quote_identifier($key);
				else
					$key = $this->quote_identifier($key);

				$order[] = sprintf('%s %s',$key,$val);
			}

			if(isset($order[0]) === false)
				return(false);

			$this->_order = array_merge($this->_order,$order);

			return(true);
		}

		$col = trim((string) $col);
		$sort = (string) $sort;

		if($col !== '')
		{
			if(($sort = $this->_chk_sort($sort)) === false)
				return(false);

			if($table !== '')
				$col = $this->quote_identifier($table).'.'.$this->quote_identifier($col);
			else
				$col = $this->quote_identifier($col);

			$this->_order[] = sprintf('%s %s',$col,$sort);
			return(true);
		}
		else if($sort === '')
		{
			$this->_order = array();
			return(null);
		}

		return(false);
	}

	function _mk_order()
	{
		$r = '';

		if(is_array($this->_order) === false || isset($this->_order[0]) === false)
			return($r);

		if(($r = implode(',',$this->_order)) !== '')
			$r = ' ORDER BY '.$r;

		return($r);
	}

	function _chk_sort($sort='')
	{
		$sort = strtoupper((string) $sort);

		switch($sort)
		{
			case 'D':
			case 'DESC':
			case SORT_DESC:
				return('DESC');
			case '':
			case 'A':
			case 'ASC':
			case SORT_ASC:
				return('ASC');
		}

		return(false);
	}

	function new_limit($offset='',$line='')
	{
		$this->_limit = array();

		return($this->limit($offset,$line));
	}

	function reset_limit()
	{
		$this->_limit = array();
	}

	function limit($offset='',$line='')
	{
		if(is_array($offset) === true && isset($offset[0]) === true)
		{
			if(isset($offset[1]) === true)
				$line = $offset[1];

			$offset = $offset[0];
		}

		$offset = (string) $offset;
		$line = (string) $line;

		if($offset !== '')
		{
			$this->_limit[0] = xivo_uint($offset);

			if($line !== '')
				$this->_limit[1] = xivo_uint($line);
			
			return(true);
		}

		$this->_limit = array();

		return(($line === '' ? null : false));
	}

	function _mk_limit()
	{
		$r = '';

		if(is_array($this->_limit) === false || isset($this->_limit[0]) === false)
			return($r);

		$r = ' LIMIT '.xivo_uint($this->_limit[0]);

		if(isset($this->_limit[1]) === true)
			$r .= ','.xivo_uint($this->_limit[1]);

		return($r);
	}

	function add_where_in($col,$list,$keys=false,$table='')
	{
		if(($in = $this->_in($col,$list,$keys,false,$table)) === false)
			return(false);

		$this->_where[] = $in;
		return(true);
	}

	function add_where_notin($col,$list,$keys=false,$table='')
	{
		if(($in = $this->_in($col,$list,$keys,true,$table)) === false)
			return(false);

		$this->_where[] = $in;
		return(true);
	}

	function _in($col,&$list,$keys=false,$not=false,$table='')
	{
		$col = trim((string) $col);
		$keys = (bool) $keys;
		$not = (bool) $not;
		$table = trim((string) $table);

		if($col === '' || is_array($list) === false || empty($list) === true)
			return(false);

		if($table !== '')
			$col = $this->quote_identifier($table).'.'.$this->quote_identifier($col);

		$func = $not === true ? 'NOT IN' : 'IN';

		if($keys === true)
			$list = array_keys($list);

		$r = $col.' '.$func.'('.implode(',',array_map(array(&$this,'escape'),$list)).')';

		return($r);
	}

	function in($col,$list,$keys=false,$table='')
	{
		return($this->_in($col,$list,$keys,false,$table));
	}

	function notin($col,$list,$keys=false,$table='')
	{
		return($this->_in($col,$list,$keys,true,$table));
	}

	function _search($col,$pattern,$format='',$not=false,$table='')
	{
		$col = trim((string) $col);
		$format = $format !== '' ? strtolower((string) $format) : 'exact';
		$not = (bool) $not;
		$table = trim((string) $table);

		if(is_scalar($pattern) === true)
			$pattern = array((string) $pattern);
		else if(is_array($pattern) !== true)
			return(false);

		if($col === '')
			return(false);
		else if($table !== '')
			$col = $this->quote_identifier($table).'.'.$this->quote_identifier($col);

		$func = $not === true ? 'NOT LIKE' : 'LIKE';

		$search_format = '%s %s ';

		switch($format)
		{
			case 'begin':
				$search_format .= '\'%s%%\'';
				break;
			case 'contain':
				$search_format .= '\'%%%s%%\'';
				break;
			case 'end':
				$search_format .= '\'%%%s\'';
				break;
			default:
				$func = $not === false ? '=' : '!=';
				$search_format .= '\'%s\'';
		}

		$value = array_values($pattern);

		if(($nb = count($value)) === 0)
			return(false);

		$r = '';

		for($i = 0;$i < $nb;$i++)
		{
			$pattern = $this->escape_string($this->_escape_search($value[$i]));

			if(strlen($pattern) === 0)
				continue;

			if($i > 0)
				$r .= ' AND ';

			$r .= sprintf($search_format,$col,$func,$pattern);
		}

		return($r);
	}

	function _escape_search($str)
	{
		$r = str_replace(array('%','_'),array('\%','\_'),(string) $str);

		return($r);
	}

	function search($col,$pattern,$format='',$table='')
	{
		return($this->_search($col,$pattern,$format,false,$table));
	}

	function search_not($col,$pattern,$format='',$table='')
	{
		return($this->_search($col,$pattern,$format,true,$table));
	}

	function search_exact($col,$pattern,$table='')
	{
		return($this->search($col,$pattern,'exact',$table));
	}

	function search_begin($col,$pattern,$table='')
	{
		return($this->search($col,$pattern,'begin',$table));
	}

	function search_contain($col,$pattern,$table='')
	{
		return($this->search($col,$pattern,'contain',$table));
	}

	function search_end($col,$pattern,$table='')
	{
		return($this->search($col,$pattern,'end',$table));
	}

	function search_not_exact($col,$pattern,$table='')
	{
		return($this->search_not($col,$pattern,'exact',$table));
	}

	function search_not_begin($col,$pattern,$table='')
	{
		return($this->search_not($col,$pattern,'begin',$table));
	}

	function search_not_contain($col,$pattern,$table='')
	{
		return($this->search_not($col,$pattern,'contain',$table));
	}

	function search_not_end($col,$pattern,$table='')
	{
		return($this->search_not($col,$pattern,'end',$table));
	}

	function _cond_typetime($type,$col,$dbeg,$dend='',$format='',$time='')
	{
		$dbeg = (string) $dbeg;
		$dend = (string) $dend;
		$format = $format !== '' ? (string) $format : 'date';
		$lowerformat = strtolower($format);

		if(isset($this->_date[$lowerformat]) === true)
			$format = $this->_date[$lowerformat];

		if($dbeg === '' || ($expr_dbeg = $this->_format_typetime($format,$type,$col,$time)) === false)
			return(false);

		if($dend === '')
			$r = $expr_dbeg.' = \''.$this->escape_string($dbeg).'\'';
		else
			$r = $expr_dbeg.' BETWEEN \''.$this->escape_string($dbeg).'\' AND \''.$this->escape_string($dend).'\'';
		
		return($r);
	}

	function _format_typetime($format,$type='',$col='',$time='')
	{
		return($this->_expr_typetime($type,$col,$time,$format));
	}

	function cond_date($col,$dbeg,$dend='',$format='',$time='')
	{
		return($this->_cond_typetime('date',$col,$dbeg,$dend,$format,$time));
	}

	function format_date($format,$col='',$time='')
	{
		return($this->_format_typetime('date',$col,$time,$format));
	}

	function datetime_from_date($col='',$time='')
	{
		return($this->_expr_typetime('date',$col,$time,$this->_date['datetime']));
	}

	function date_from_date($col='',$time='')
	{
		return($this->_expr_typetime('date',$col,$time,$this->_date['date']));
	}

	function time_from_date($col='',$time='')
	{
		return($this->_expr_typetime('date',$col,$time,$this->_date['time']));
	}

	function cond_unixtime($col,$dbeg,$dend='',$format='',$time='')
	{
		return($this->_cond_typetime('unix',$col,$dbeg,$dend,$format,$time));
	}

	function format_unixtime($format,$col='',$time='')
	{
		return($this->_format_typetime($col,'unix',$time,$format));
	}

	function datetime_from_unixtime($col='',$time='')
	{
		return($this->_expr_typetime('unix',$col,$time,$this->_date['datetime']));
	}

	function date_from_unixtime($col='',$time='')
	{
		return($this->_expr_typetime('unix',$col,$time,$this->_date['date']));
	}

	function time_from_unixtime($col='',$time='')
	{
		return($this->_expr_typetime('unix',$col,$time,$this->_date['time']));
	}
}

?>
