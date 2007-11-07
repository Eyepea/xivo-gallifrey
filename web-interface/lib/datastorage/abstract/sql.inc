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

	function select_all($t,$s='',$w='',$o='')
	{
		$t = trim((string) $t);
		$s = trim((string) $s);
		$w = trim((string) $w);
		$o = trim((string) $o);

		if($s === '')
			$s = '*';

		if($o !== '')
			$o = ' '.$o;

		$o .= $this->_mk_order();
		$o .= $this->_mk_limit();

		$q = sprintf('SELECT %s FROM %s WHERE %s%s',$s,$t,($w !== '' ? $w : 1),$o);

		if($this->query($q) === false)
			return(false);

		$r = array();

		for($i = 0;$m = $this->fetch_assoc();$i++)
			$r[$i] = $m;

		if($this->_free === true)
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

	function where($data,$escape=true)
	{
		$r = false;
	
		if(($where = $this->_separator($data,$sep=' AND ',$escape,true,false)) === false)
			return($r);

		$r = $where['str'];

		if(trim($r) === '')
			$r = false;

		return($r);
	}

	function where_not($data,$escape=true)
	{
		$r = false;

		if(($where = $this->_separator($data,$sep=' AND ',$escape,true,false,'!=')) === false)
			return($r);

		$r = $where['str'];

		if(trim($r) === '')
			$r = false;

		return($r);	
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

	function _separator($val,$sep=',',$escape=true,$set=false,$ident=true,$oper='=')
	{
		$r = array('str' => '','keys' => '');

		$val = (array) $val;
		$sep = (string) $sep;
		$escape = (bool) $escape;
		$set = (bool) $set;
		$ident = (bool) $ident;
		$oper = (string) $oper;

		if(($arr = xivo_get_aks($val)) === false)
			return($r);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];

			$type = gettype($val[$key]);
			$data = &$val[$key];

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
				case 'string':
				default:
					if(is_string($data) === false)
						$data = strval($data);

					$f = $set === true ? '%s %s \'%s\''.$sep : '\'%s\''.$sep;

					if($escape === true)
						$data = $this->escape_string($data);
			}

			if($set === true)
				$r['str'] .= sprintf($f,($ident === true ? $this->quote_identifier($key) : $key),$oper,$data);
			else
			{
				$r['keys'] .= ($ident === true ? $this->quote_identifier($key) : $key).$sep;
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

	function order($col='',$sort='')
	{
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

				$order[] = sprintf('%s %s',$this->quote_identifier($key),$val);
			}

			if(isset($order[0]) === false)
				return(false);

			$this->_order = array_merge($this->_order,$order);

			return(true);
		}

		$col = (string) $col;
		$sort = (string) $sort;

		if($col !== '')
		{
			if(($sort = $this->_chk_sort($sort)) === false)
				return(false);

			$this->_order[] = sprintf('%s %s',$this->quote_identifier($col),$sort);
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

	function _in($col,&$list,$keys=false,$not=false)
	{
		$col = $this->quote_identifier($col);
		$keys = (bool) $keys;
		$not = (bool) $not;

		if($col === '' || is_array($list) === false || empty($list) === true)
			return(false);

		$func = $not === true ? 'NOT IN' : 'IN';

		if($keys === true)
			$list = array_keys($list);

		$r = $col.' '.$func.'(\''.implode('\',\'',array_map(array(&$this,'escape_string'),$list)).'\')';

		return($r);
	}

	function in($col,$list,$keys=false)
	{
		return($this->_in($col,$list,$keys,false));
	}

	function notin($col,$list,$keys=false)
	{
		return($this->_in($col,$list,$keys,true));
	}

	function _search($col,$pattern,$format='',$not=false)
	{
		$col = $this->quote_identifier($col);
		$format = $format !== '' ? strtolower((string) $format) : 'exact';
		$not = (bool) $not;

		if(is_scalar($pattern) === true)
			$pattern = array((string) $pattern);
		else if(is_array($pattern) !== true)
			return(false);

		if($col === '')
			return(false);

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

	function search($col,$pattern,$format='')
	{
		return($this->_search($col,$pattern,$format,false));
	}

	function search_not($col,$pattern,$format='')
	{
		return($this->_search($col,$pattern,$format,true));
	}

	function search_exact($col,$pattern)
	{
		return($this->search($col,$pattern,'exact'));
	}

	function search_begin($col,$pattern)
	{
		return($this->search($col,$pattern,'begin'));
	}

	function search_contain($col,$pattern)
	{
		return($this->search($col,$pattern,'contain'));
	}

	function search_end($col,$pattern)
	{
		return($this->search($col,$pattern,'end'));
	}

	function search_not_exact($col,$pattern)
	{
		return($this->search_not($col,$pattern,'exact'));
	}

	function search_not_begin($col,$pattern)
	{
		return($this->search_not($col,$pattern,'begin'));
	}

	function search_not_contain($col,$pattern)
	{
		return($this->search_not($col,$pattern,'contain'));
	}

	function search_not_end($col,$pattern)
	{
		return($this->search_not($col,$pattern,'end'));
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
