<?php

class xivo_datastorage_abstract_sql
{
	var $_error		= '';
	var $_errno		= 0;
	var $_query		= '';
	var $_meta		= 'sql';
	var $_quote		= false;
	var $_link		= null;
	var $_result		= false;
	var $_debug		= false;
	var $_free		= false;
	var $_from		= '';
	var $_table		= array();
	var $_column		= array();
	var $_colall		= array();
	var $_join		= array();
	var $_joinwhere		= array();
	var $_aggjoinwhere	= array();
	var $_where		= '';
	var $_aggwhere		= array();
	var $_group		= array();
	var $_order		= array();
	var $_limit		= array();

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

	function get_meta()
	{
		return($this->_meta);
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

	function _chk_quote($name)
	{
		$r = '';

		if(is_object($name) === true)
		{
			if(isset($name->scalar) === false)
				return($r);

			$name = $name->scalar;
		}

		if(xivo_haslen($name) === true)
			$r = $name;

		return($r);
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
			case 'object':
				if(isset($val->scalar) === false)
					return(false);
				return((string) $val->scalar);
			case 'NULL':
				return('NULL');
		}

		return('\''.$this->escape_string(strval($val)).'\'');
	}

	function new_select($table,$column='*')
	{
		$this->reset_all();

		return($this->from($table,$column));
	}

	function from($table,$column='*')
	{
		if(($table = $this->_mk_table($table)) === false)
			return(false);
		else if(is_array($table) === true)
		{
			$this->_from = sprintf('%s AS %s',$this->quote_identifier($table[1]),
							  $this->quote_identifier($table[0]));
			$table = $table[0];
		}
		else
			$this->_from = $this->quote_identifier($table);

		return($this->_mk_column($table,$column));
	}

	function get_from()
	{
		if($this->_from !== '')
			return($this->_from);

		return(null);
	}

	function reset_from()
	{
		$this->_from = '';
	}

	function get_select()
	{
		if(($r = $this->_mk_select()) !== false)
			return($r);

		return(null);
	}

	function reset_all()
	{
		$this->_table = array();
		$this->reset_from();
		$this->reset_column();
		$this->reset_join();
		$this->reset_where();
		$this->reset_group();
		$this->reset_order();
		$this->reset_limit();
	}

	function _mk_select()
	{
		if(($from = $this->get_from()) === null
		|| ($column = $this->get_column()) === null)
			return(false);

		$r = 'SELECT '.$column.' FROM '.$from;

		if(($join = $this->get_join()) !== null)
			$r .= ' '.$join;

		if(($where = $this->get_where()) !== null)
			$r .= ' WHERE '.$where;

		if(($group = $this->get_group()) !== null)
			$r .= ' GROUP BY '.$group;

		if(($order = $this->get_order()) !== null)
			$r .= ' ORDER BY '.$order;

		if(($limit = $this->get_limit()) !== null)
			$r .= ' LIMIT '.$limit;

		return($r);
	}

	function innerjoin($table,$cond,$val='',$column='')
	{
		if($cond === null)
			return(false);

		return($this->_mk_join('inner',$table,$cond,$val,$column));
	}

	function leftjoin($table,$cond,$val='',$column='')
	{
		if($cond === null)
			return(false);

		return($this->_mk_join('left',$table,$cond,$val,$column));
	}

	function rightjoin($table,$cond,$val='',$column='')
	{
		if($cond === null)
			return(false);

		return($this->_mk_join('right',$table,$cond,$val,$column));
	}

	function crossjoin($table,$column='')
	{
		return($this->_mk_join('cross',$table,null,null,$column));
	}

	function naturaljoin($table,$column='')
	{
		return($this->_mk_join('natural',$table,null,null,$column));
	}

	function get_join()
	{
		if(($arr = xivo_get_aks($this->_join)) === false)
			return(null);

		$r = '';

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$table = &$arr['keys'][$i];
			$r .= ' '.$this->_join[$table];

			if(isset($this->_joinwhere[$table]) === true)
				$r .= ' ON '.$this->_joinwhere[$table];
		}

		if(($r = ltrim($r)) === '')
			$r = null;

		return($r);
	}

	function reset_join()
	{
		$this->_join = $this->_joinwhere = $this->_aggjoinwhere = array();
	}

	function _chk_join($type)
	{
		$type = strtoupper((string) $type);

		switch($type)
		{
			case 'INNER':
			case 'LEFT':
			case 'RIGHT':
			case 'CROSS':
			case 'FULL':
			case 'NATURAL':
				break;
			case '':
				$type = 'INNER';
			default:
				return(false);
		}

		return(sprintf('%s JOIN',$type));
	}

	function _mk_join($type,$table,$cond='',$val='',$column='')
	{
		if(($type = $this->_chk_join($type)) === false
		|| ($table = $this->_mk_table($table)) === false)
			return(false);
		else if(is_array($table) === true)
		{
			$this->_join[$table[0]] = sprintf('%s %s AS %s',$type,
								        $this->quote_identifier($table[1]),
								        $this->quote_identifier($table[0]));
			$table = $table[0];
		}
		else
			$this->_join[$table] = sprintf('%s %s',$type,$this->quote_identifier($table));

		if($cond !== null)
		{
			if(is_array($cond) === true)
				$val = $table;

			if($this->joinwhere($table,$cond,$val) === false)
				return(false);
		}

		return($this->_mk_column($table,$column));
	}

	function _mk_table($table)
	{
		if(xivo_haslen($table) === true)
		{
			if(isset($this->_table[$table]) === true)
				return(false);

			$this->_table[$table] = 1;
			return($table);
		}
		else if(is_array($table) === false)
			return(false);

		$key = key($table);
		$val = current($table);

		if(xivo_haslen($key) === false
		|| xivo_haslen($val) === false
		|| isset($this->_table[$key]) === true)
			return(false);

		$this->_table[$key] = $val;

		return(array($key,$val));
	}

	function reset_column()
	{
		$this->_column = $this->_colall = array();
	}

	function get_column()
	{
		if(isset($this->_colall[0]) === true
		&& ($join = implode('.*, ',array_map(array(&$this,'quote_identifier'),$this->_colall))) !== '')
			$r = $join.'.*';
		else
			$r = '';

		if(isset($this->_column[0]) === true
		&& ($join = implode(', ',$this->_column)) !== '')
			$r .= ($r !== '' ? ', ' : '').$join;

		if($r === '')
			$r = null;

		return($r);
	}

	function _mk_column($table,$column)
	{
		if(is_scalar($column) === true)
		{
			$column = trim($column);

			if(xivo_haslen($column) === false)
				return(null);

			if($column !== '*')
				$column = (array) $column;
			else
			{
				$this->_colall[] = $table;
				return(true);
			}
		}

		if(($arr = xivo_get_aks($column)) === false)
			return(null);

		$tabledot = $table.'.';
		$tabledotlen = strlen($tabledot);

		$noassoc = in_array(0,$arr['keys'],true);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$column[$key];

			$object = false;

			if(is_object($val) === true)
			{
				if(isset($val->scalar) === false)
					return(false);

				$object = true;
				$val = $val->scalar;
			}

			if(xivo_haslen($key) === false || xivo_haslen($val) === false)
				continue;
			else if($object === true || preg_match('/^[a-zA-Z0-9_\-\.]+$/',$val) === 0)
			{
				if($noassoc === true && is_int($key) === true)
					$col = sprintf('%s',$val);
				else
					$col = sprintf('%s AS %s',$val,
								  $this->quote_identifier($key));

				$this->_column[] = $col;
				continue;
			}

			if(strncasecmp($val,$tabledot,$tabledotlen) === 0)
				$val = substr($val,$tabledotlen);

			if(xivo_haslen($val) === false)
				continue;

			if($noassoc === true && is_int($key) === true)
				$col = sprintf('%s.%s',$this->quote_identifier($table),
						       $this->quote_identifier($val));
			else
				$col = sprintf('%s.%s AS %s',$this->quote_identifier($table),
							     $this->quote_identifier($val),
							     $this->quote_identifier($key));

			$this->_column[] = $col;
		}

		return(true);
	}

	function insert($table,$data)
	{
		return($this->_insertupdate('insert',$table,$data));
	}

	function update($table,$data,$condwhere='',$valwhere='',$usewhere=false,$useorder=false,$uselimit=false)
	{
		return($this->_insertupdate('update',$table,$data,true,$condwhere,$valwhere,$usewhere,$useorder,$uselimit));
	}

	function replace($table,$data)
	{
		return($this->_insertupdate('replace',$table,$data));
	}

	function delete($table,$cond='',$val='',$usewhere=false,$useorder=false,$uselimit=false)
	{
		$table = trim((string) $table);
		$usewhere = (bool) $usewhere;

		$order = $limit = null;

		if(is_bool($useorder) === true)
		{
			if($useorder === true)
				$order = $this->get_order();
		}
		else if(($order = $this->_mk_order($useorder)) === false)
			return(false);

		if(is_bool($uselimit) === true)
		{
			if($uselimit === true)
				$limit = $this->get_limit();
		}
		else if(($limit = $this->_mk_limit($uselimit)) === false)
			return(false);

		if($table === ''
		|| (($where = $this->_mk_where($cond,$val)) === false && $usewhere === false) === true
		|| ($usewhere === true && ($where = $this->get_where()) === null) === true)
			return(false);

		if($where === '')
			$where = 1;

		$other = '';

		if($order !== null)
			$other .= ' ORDER BY '.$order;

		if($limit !== null)
			$other .= ' LIMIT '.$limit;

		$query = sprintf('DELETE FROM %s WHERE %s%s',$this->quote_identifier($table),$where,$other);

		if(($r = $this->query($query)) !== false)
			$r = (bool) $this->affected_rows();

		return($r);
	}

	function _insertupdate($proc,$table,&$data,$set=false,$condwhere='',$valwhere='',$usewhere=false,$useorder=false,$uselimit=false)
	{
		$proc = strtolower((string) $proc);
		$table = trim((string) $table);
		$set = (bool) $set;
		$usewhere = (bool) $usewhere;

		if($table === '' || ($values = $this->_mk_sep($data,$set)) === false)
			return(false);

		$where = $other = '';

		switch($proc)
		{
			case 'insert':
			case 'replace':
				$proc = strtoupper($proc).' INTO';
				break;
			case 'update':
				$proc = 'UPDATE';

				if($condwhere === '' && $usewhere === false)
					$condwhere = 1;

				if((($where = $this->_mk_where($condwhere,$valwhere)) === false && $usewhere === false) === true
				|| ($usewhere === true && ($where = $this->get_where()) === null) === true)
					return(false);

				$order = $limit = null;

				if(is_bool($useorder) === true)
				{
					if($useorder === true)
						$order = $this->get_order();
				}
				else if(($order = $this->_mk_order($useorder)) === false)
					return(false);

				if(is_bool($uselimit) === true)
				{
					if($uselimit === true)
						$limit = $this->get_limit();
				}
				else if(($limit = $this->_mk_limit($uselimit)) === false)
					return(false);

				if($order !== null)
					$other .= ' ORDER BY '.$order;

				if($limit !== null)
					$other .= ' LIMIT '.$limit;
				break;
			default:
				return(false);
		}

		if($set === true)
			$query = sprintf('%s %s SET %s',$proc,
						    $this->quote_identifier($table),
						    implode(', ',$values['str']));
		else
			$query = sprintf('%s %s (%s) VALUES(%s)',$proc,
							     $this->quote_identifier($table),
							     implode(',',$values['keys']),
							     implode(',',$values['str']));

		if($where !== '')
			$query .= ' WHERE '.$where;

		if($other !== '')
			$query .= $other;

		if($this->query($query) === false)
			return(false);

		return(true);
	}

	function _mk_sep(&$val,$set=false,$table='')
	{
		$r = array('str' => array(),'keys' => array());

		$val = (array) $val;
		$set = (bool) $set;
		$table = trim((string) $table);

		if(($arr = xivo_get_aks($val)) === false)
			return(false);

		$tabledot = '';

		if($table !== '')
		{
			if(isset($this->_table[$table]) === false)
				return(false);

			$tabledot = $this->quote_identifier($table).'.';
		}

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$data = $this->escape($val[$key]);

			if(xivo_haslen($key) === false)
				continue;
			else if($set === true)
				$r['str'][] = $tabledot.$this->quote_identifier($key).' = '.$data;
			else
			{
				$r['keys'][] = $tabledot.$this->quote_identifier($key);
				$r['str'][] = $data;
			}
		}

		if(isset($r['str'][0]) === false)
			return(false);

		return($r);
	}

	function reset_where()
	{
		$this->_where = '';
		$this->_aggwhere = array();
	}

	function _set_where($cond,$val='',$condand=true,$and=true,$not=false,$join=false)
	{
		$and = (bool) $and === true ? ' AND ' : ' OR ';
		$not = (bool) $not;

		if(($where = $this->_mk_where($cond,$val,$condand,$join)) === false
		|| ($join !== false && isset($this->_join[$join]) === false) === true)
			return(false);

		if($join === false)
			$ref = &$this->_where;
		else
		{
			if(isset($this->_joinwhere[$join]) === false)
				$this->_joinwhere[$join] = '';

			$ref = &$this->_joinwhere[$join];
		}

		if($not === true)
			$where = 'NOT ('.$where.')';

		if($ref === '')
			$ref = $where;
		else
			$ref .= $and.$where; 

		return(true);
	}

	function _mk_where($cond,$val='',$condand=true,$join=false)
	{
		$condand = (bool) $condand === true ? ' AND ' : ' OR ';

		if(is_array($cond) === true)
		{
			if($join !== false)
				$val = $join;

			if(($condsep = $this->_mk_sep($cond,true,$val)) === false)
				return(false);

			$r = implode($condand,$condsep['str']);
		}
		else if(is_array($val) === true || xivo_haslen($val) === true)
		{
			$val = (array) $val;
			$r = vsprintf(xivo_printf_strip($cond,count($val)),
				      array_map(array(&$this,'escape'),$val));
		}
		else if(xivo_haslen($cond) === true)
			$r = (string) $cond;
		else
			return(false);

		if(trim($r) === '')
			return(false);

		return($r);
	}

	function get_where()
	{
		if(xivo_haslen($this->_where) === false)
			return(null);

		return($this->_where);
	}

	function aggregate_where($all=false)
	{
		$all = (bool) $all;

		if($this->_where === '')
			return(false);

		if($all === true || isset($this->_aggwhere[0]) === false)
		{
			$this->_where = '('.$this->_where.')';
			$this->_aggwhere[] = strlen($this->_where)+1;
			return(true);
		}
		else if(($last = xivo_last_key($this->_aggwhere)) === false)
			return(false);

		$start = substr($this->_where,0,$this->_aggwhere[$last]);
		$end = substr($this->_where,$this->_aggwhere[$last]);

		if(preg_match('/^([\s])?(AND|OR|XOR)([\(\s])?/i',$end,$match) === 0)
			return(false);

		$logic = $match[1].strtoupper($match[2]);
		$len = strlen($match[2]);

		if(isset($match[3]{0}) === false)
			$logic .= ' ';
		else
		{
			$len++;
			$logic .= $match[3];
		}

		$this->_where = $start.$logic.'('.substr(ltrim($end),$len).')';

		return(true);
	}

	function aggregate_where_all()
	{
		return($this->aggregate_where(true));
	}

	function where($cond,$val='',$condand=true)
	{
		return($this->andwhere($cond,$val,$condand));
	}

	function andwhere($cond,$val='',$condand=true)
	{
		return($this->_set_where($cond,$val,$condand,true,false));
	}

	function andwherenot($cond,$val='',$condand=true)
	{
		return($this->_set_where($cond,$val,$condand,true,true));
	}

	function orwhere($cond,$val='',$condand=true)
	{
		return($this->_set_where($cond,$val,$condand,false,false));
	}

	function orwherenot($cond,$val='',$condand=true)
	{
		return($this->_set_where($cond,$val,$condand,false,true));
	}

	function aggregate_joinwhere($table,$all=false)
	{
		$all = (bool) $all;

		if(isset($this->_joinwhere[$table]) === false
		|| $this->_joinwhere[$table] === '')
			return(false);

		if(isset($this->_aggjoinwhere[$table]) === false)
			$this->_aggjoinwhere[$table] = array();

		if($all === true || isset($this->_aggjoinwhere[$table][0]) === false)
		{
			$this->_joinwhere[$table] = '('.$this->_joinwhere[$table].')';
			$this->_aggjoinwhere[$table][] = strlen($this->_joinwhere[$table])+1;
			return(true);
		}
		else if(($last = xivo_last_key($this->_aggjoinwhere[$table])) === false)
			return(false);

		$start = substr($this->_joinwhere[$table],0,$this->_aggjoinwhere[$table][$last]);
		$end = substr($this->_joinwhere[$table],$this->_aggjoinwhere[$table][$last]);

		if(preg_match('/^([\s])?(AND|OR|XOR)([\(\s])?/i',$end,$match) === 0)
			return(false);

		$logic = $match[1].strtoupper($match[2]);
		$len = strlen($match[2]);

		if(isset($match[3]{0}) === false)
			$logic .= ' ';
		else
		{
			$len++;
			$logic .= $match[3];
		}

		$this->_joinwhere[$table] = $start.$logic.'('.substr(ltrim($end),$len).')';

		return(true);
	}

	function aggregate_joinwhere_all($table)
	{
		return($this->aggregate_where($table,true));
	}

	function joinwhere($table,$cond,$val='',$condand=true)
	{
		return($this->andjoinwhere($table,$cond,$val,$condand));
	}

	function andjoinwhere($table,$cond,$val='',$condand=true)
	{
		return($this->_set_where($cond,$val,$condand,true,false,$table));
	}

	function andjoinwherenot($table,$cond,$val='',$condand=true)
	{
		return($this->_set_where($cond,$val,$condand,true,true,$table));
	}

	function orjoinwhere($table,$cond,$val='',$condand=true)
	{
		return($this->_set_where($cond,$val,$condand,false,false,$table));
	}

	function orjoinwherenot($table,$cond,$val='',$condand=true)
	{
		return($this->_set_where($cond,$val,$condand,false,true,$table));
	}

	function _set_wherebetween($column,$beg,$end,$table='',$and=true,$not=false,$join=false)
	{
		$and = (bool) $and === true ? ' AND ' : ' OR ';
		$not = (bool) $not;

		if(($between = $this->_mk_wherebetween($column,$beg,$end,$table)) === false
		|| ($join !== false && isset($this->_join[$join]) === false) === true)
			return(false);

		if($join === false)
			$ref = &$this->_where;
		else
		{
			if(isset($this->_joinwhere[$join]) === false)
				$this->_joinwhere[$join] = '';

			$ref = &$this->_joinwhere[$join];
		}

		if($not === true)
			$between = 'NOT ('.$between.')';

		if($ref === '')
			$ref = $between;
		else
			$ref .= $and.$between;

		return(true);
	}

	function _mk_wherebetween($column,$beg,$end,$table='')
	{
		$column = trim((string) $column);
		$table = trim((string) $table);
	
		if($column === '')
			return(false);
		else if($table === '')
			$column = $this->quote_identifier($column);
		else if(isset($this->_table[$table]) === false)
			return(false);
		else
			$column = $this->quote_identifier($table).'.'.$this->quote_identifier($column);

		$r = sprintf('%s BETWEEN %s AND %s',$column,$this->escape($beg),$this->escape($end));

		return($r);
	}

	function wherebetween($column,$beg,$end,$table='')
	{
		return($this->andwherebetween($column,$beg,$end,$table));
	}

	function andwherebetween($column,$beg,$end,$table='')
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,true,false));
	}

	function andwherenotbetween($column,$beg,$end,$table='')
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,true,true));
	}

	function orwherebetween($column,$beg,$end,$table='')
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,false,false));
	}

	function orwherenotbetween($column,$beg,$end,$table='')
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,false,true));
	}

	function joinwherebetween($table,$column,$beg,$end)
	{
		return($this->andjoinwherebetween($table,$column,$beg,$end));
	}

	function andjoinwherebetween($table,$column,$beg,$end)
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,true,false,$table));
	}

	function andjoinwherenotbetween($table,$column,$beg,$end)
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,true,true,$table));
	}

	function orjoinwherebetween($table,$column,$beg,$end)
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,false,false,$table));
	}

	function orjoinwherenotbetween($table,$column,$beg,$end)
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,false,true,$table));
	}

	function _set_wherebetweencolumn($val,$beg,$end,$table='',$and=true,$not=false,$join=false)
	{
		$and = (bool) $and === true ? ' AND ' : ' OR ';
		$not = (bool) $not;

		if(($between = $this->_mk_wherebetweencolumn($val,$beg,$end,$table)) === false
		|| ($join !== false && isset($this->_join[$join]) === false) === true)
			return(false);

		if($join === false)
			$ref = &$this->_where;
		else
		{
			if(isset($this->_joinwhere[$join]) === false)
				$this->_joinwhere[$join] = '';

			$ref = &$this->_joinwhere[$join];
		}

		if($not === true)
			$between = 'NOT ('.$between.')';

		if($ref === '')
			$ref = $between;
		else
			$ref .= $and.$between;

		return(true);
	}

	function _mk_wherebetweencolumn($val,$beg,$end,$table='')
	{
		$beg = trim((string) $beg);
		$end = trim((string) $end);
		$table = trim((string) $table);
	
		if(isset($beg{0},$end{0}) === false)
			return(false);
		else if($table === '')
		{
			$beg = $this->quote_identifier($beg);
			$end = $this->quote_identifier($end);
		}
		else if(isset($this->_table[$table]) === false)
			return(false);
		else
		{
			$beg = $this->quote_identifier($table).'.'.$this->quote_identifier($beg);
			$end = $this->quote_identifier($table).'.'.$this->quote_identifier($end);
		}

		$r = sprintf('%s BETWEEN %s AND %s',$this->escape($val),$beg,$end);

		return($r);
	}

	function wherebetweencolumn($val,$beg,$end,$table='')
	{
		return($this->andwherebetweencolumn($val,$beg,$end,$table));
	}

	function andwherebetweencolumn($val,$beg,$end,$table='')
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,true,false));
	}

	function andwherenotbetweencolumn($val,$beg,$end,$table='')
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,true,true));
	}

	function orwherebetweencolumn($val,$beg,$end,$table='')
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,false,false));
	}

	function orwherenotbetweencolumn($val,$beg,$end,$table='')
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,false,true));
	}

	function joinwherebetweencolumn($table,$val,$beg,$end)
	{
		return($this->andjoinwherebetweencolumn($table,$val,$beg,$end));
	}

	function andjoinwherebetweencolumn($table,$val,$beg,$end)
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,true,false,$table));
	}

	function andjoinwherenotbetweencolumn($table,$val,$beg,$end)
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,true,true,$table));
	}

	function orjoinwherebetweencolumn($table,$val,$beg,$end)
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,false,false,$table));
	}

	function orjoinwherenotbetweencolumn($table,$val,$beg,$end)
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,false,true,$table));
	}

	function _set_wherein($column,&$list,$table='',$keys=false,$and=true,$not=false,$join=false)
	{
		$and = (bool) $and === true ? ' AND ' : ' OR ';

		if(($in = $this->_mk_wherein($column,$list,$table,$keys,$not)) === false
		|| ($join !== false && isset($this->_join[$join]) === false) === true)
			return(false);

		if($join === false)
			$ref = &$this->_where;
		else
		{
			if(isset($this->_joinwhere[$join]) === false)
				$this->_joinwhere[$join] = '';

			$ref = &$this->_joinwhere[$join];
		}

		if($ref === '')
			$ref = $in;
		else
			$ref .= $and.$in;

		return(true);
	}

	function _mk_wherein($column,&$list,$table='',$keys=false,$not=false)
	{
		$column = trim((string) $column);
		$table = trim((string) $table);
		$keys = (bool) $keys;
		$not = (bool) $not;

		if($column === '' || is_array($list) === false || empty($list) === true)
			return(false);
		else if($table === '')
			$column = $this->quote_identifier($column);
		else if(isset($this->_table[$table]) === false)
			return(false);
		else
			$column = $this->quote_identifier($table).'.'.$this->quote_identifier($column);

		$func = $not === true ? 'NOT ' : '';

		if($keys === true)
			$list = array_keys($list);

		$list = implode(',',array_map(array(&$this,'escape'),$list));

		if(trim($list) === '')
			return(false);

		return(sprintf('%s %sIN(%s)',$column,$func,$list));
	}

	function wherein($column,$list,$table='',$keys=false)
	{
		return($this->andwherein($column,$list,$table,$keys));
	}

	function andwherein($column,$list,$table='',$keys=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,true,false));
	}

	function andwherenotin($column,$list,$table='',$keys=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,true,true));
	}

	function orwherein($column,$list,$table='',$keys=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,false,false));
	}

	function orwherenotin($column,$list,$table='',$keys=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,false,true));
	}

	function joinwherein($table,$column,$list,$keys=false)
	{
		return($this->andjoinwherein($table,$column,$list,null,$keys));
	}

	function andjoinwherein($table,$column,$list,$keys=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,true,false,$table));
	}

	function andjoinwherenotin($table,$column,$list,$keys=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,true,true,$table));
	}

	function orjoinwherein($table,$column,$list,$keys=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,false,false,$table));
	}

	function orjoinwherenotin($table,$column,$list,$keys=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,false,true,$table));
	}

	function _set_wherelike($column,$pattern,$table='',$format='',$and=true,$not=false,$join=false)
	{
		$and = (bool) $and === true ? ' AND ' : ' OR ';

		if(($like = $this->_mk_wherelike($column,$pattern,$table,$format,$not)) === false
		|| ($join !== false && isset($this->_join[$join]) === false) === true)
			return(false);

		if($join === false)
			$ref = &$this->_where;
		else
		{
			if(isset($this->_joinwhere[$join]) === false)
				$this->_joinwhere[$join] = '';

			$ref = &$this->_joinwhere[$join];
		}

		if($ref === '')
			$ref = $like;
		else
			$ref .= $and.$like;

		return(true);
	}

	function _mk_wherelike($column,$pattern,$table='',$format='',$not=false)
	{
		$column = trim((string) $column);
		$table = trim((string) $table);
		$format = $format !== '' ? strtolower((string) $format) : 'exact';
		$not = (bool) $not;

		if(is_scalar($pattern) === true)
			$pattern = array((string) $pattern);
		else if(is_array($pattern) === false)
			return(false);

		if($column === '')
			return(false);
		else if($table === '')
			$column = $this->quote_identifier($column);
		else if(isset($this->_table[$table]) === false)
				return(false);
		else
			$column = $this->quote_identifier($table).'.'.$this->quote_identifier($column);

		$func = $not === true ? 'NOT LIKE' : 'LIKE';

		$like_format = '%s %s ';

		switch($format)
		{
			case 'begin':
				$like_format .= '\'%s%%\'';
				break;
			case 'contain':
				$like_format .= '\'%%%s%%\'';
				break;
			case 'end':
				$like_format .= '\'%%%s\'';
				break;
			default:
				$func = '=';
				$like_format .= '\'%s\'';
		}

		$value = array_values($pattern);

		if(($nb = count($value)) === 0)
			return(false);

		$r = '';

		for($i = 0;$i < $nb;$i++)
		{
			$pattern = $this->escape_string($this->_escape_like($value[$i]));

			if(isset($pattern{0}) === false)
				continue;

			if($i > 0)
				$r .= ' AND ';

			$r .= sprintf($like_format,$column,$func,$pattern);
		}

		return($r);
	}

	function _escape_like($str)
	{
		$r = str_replace(array('%','_'),array('\%','\_'),(string) $str);

		return($r);
	}

	function wherelike($column,$pattern,$format,$table='')
	{
		return($this->andwherelike($column,$pattern,$format,$table));
	}

	function andwherelike($column,$pattern,$format,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,true,false));
	}

	function andwherenotlike($column,$pattern,$format,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,true,true));
	}

	function orwherelike($column,$pattern,$format,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,false,false));
	}

	function orwherenotlike($column,$pattern,$format,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,false,true));
	}

	function whereexact($column,$pattern,$table='')
	{
		return($this->andwhereexact($column,$pattern,$table));
	}

	function andwhereexact($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',true,false));
	}

	function andwherenotexact($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',true,true));
	}

	function orwhereexact($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',false,false));
	}

	function orwherenotexact($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',false,true));
	}

	function wherebegin($column,$pattern,$table='')
	{
		return($this->andwherebegin($column,$pattern,$table));
	}

	function andwherebegin($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',true,false));
	}

	function andwherenotbegin($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',true,true));
	}

	function orwherebegin($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',false,false));
	}

	function orwherenotbegin($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',false,true));
	}

	function wherecontain($column,$pattern,$table='')
	{
		return($this->andwherecontain($column,$pattern,$table));
	}

	function andwherecontain($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',true,false));
	}

	function andwherenotcontain($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',true,true));
	}

	function orwherecontain($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',false,false));
	}

	function orwherenotcontain($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',false,true));
	}

	function whereend($column,$pattern,$table='')
	{
		return($this->andwhereend($column,$pattern,$table));
	}

	function andwhereend($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',true,false));
	}

	function andwherenotend($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',true,true));
	}

	function orwhereend($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',false,false));
	}

	function orwherenotend($column,$pattern,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',false,true));
	}

	function joinwherelike($table,$column,$pattern,$format)
	{
		return($this->andjoinwherelike($table,$column,$pattern,$format));
	}

	function andjoinwherelike($table,$column,$pattern,$format)
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,true,false,$table));
	}

	function andjoinwherenotlike($table,$column,$pattern,$format)
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,true,true,$table));
	}

	function orjoinwherelike($table,$column,$pattern,$format)
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,false,false,$table));
	}

	function orjoinwherenotlike($table,$column,$pattern,$format)
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,false,true,$table));
	}

	function joinwhereexact($table,$column,$pattern)
	{
		return($this->andjoinwhereexact($table,$column,$pattern));
	}

	function andjoinwhereexact($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',true,false,$table));
	}

	function andjoinwherenotexact($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',true,true,$table));
	}

	function orjoinwhereexact($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',false,false,$table));
	}

	function orjoinwherenotexact($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',false,true,$table));
	}

	function joinwherebegin($table,$column,$pattern)
	{
		return($this->andjoinwherebegin($table,$column,$pattern));
	}

	function andjoinwherebegin($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',true,false,$table));
	}

	function andjoinwherenotbegin($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',true,true,$table));
	}

	function orjoinwherebegin($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',false,false,$table));
	}

	function orjoinwherenotbegin($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',false,true,$table));
	}

	function joinwherecontain($table,$column,$pattern)
	{
		return($this->andjoinwherecontain($table,$column,$pattern));
	}

	function andjoinwherecontain($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',true,false,$table));
	}

	function andjoinwherenotcontain($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',true,true,$table));
	}

	function orjoinwherecontain($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',false,false,$table));
	}

	function orjoinwherenotcontain($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',false,true,$table));
	}

	function joinwhereend($table,$column,$pattern)
	{
		return($this->andjoinwhereend($table,$column,$pattern));
	}

	function andjoinwhereend($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',true,false,$table));
	}

	function andjoinwherenotend($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',true,true,$table));
	}

	function orjoinwhereend($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',false,false,$table));
	}

	function orjoinwherenotend($table,$column,$pattern)
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',false,true,$table));
	}

	function _set_wheretypetime($type,$column,$dbeg,$dend='',$table='',$format='',$time='',$and=true,$not=false,$join=false)
	{
		$and = (bool) $and === true ? ' AND ' : ' OR ';
		$not = (bool) $not;

		if(($time = $this->_mk_wheretypetime($type,$column,$dbeg,$dend,$table,$format,$time)) === false
		|| ($join !== false && isset($this->_join[$join]) === false) === true)
			return(false);

		if($join === false)
			$ref = &$this->_where;
		else
		{
			if(isset($this->_joinwhere[$join]) === false)
				$this->_joinwhere[$join] = '';

			$ref = &$this->_joinwhere[$join];
		}

		if($not === true)
			$time = 'NOT ('.$time.')';

		if($ref === '')
			$ref = $time;
		else
			$ref .= $and.$time; 

		return(true);
	}

	function _mk_wheretypetime($type,$column,$dbeg,$dend='',$table='',$format='',$time='')
	{
		$format = $format !== '' ? (string) $format : 'date';
		$lowerformat = strtolower($format);

		if(isset($this->_date[$lowerformat]) === true)
			$format = $this->_date[$lowerformat];

		if($dbeg === '' || ($expr_dbeg = $this->_format_typetime($format,$type,$column,$time,$table)) === false)
			return(false);

		if($dend === '')
			$r = $expr_dbeg.' = '.$this->escape($dbeg);
		else
			$r = $expr_dbeg.' BETWEEN '.$this->escape($dbeg).' AND '.$this->escape($dend);
		
		return($r);
	}

	function wheredate($column,$dbeg,$dend='',$table='',$format='',$time='')
	{
		return($this->andwheredate($column,$dbeg,$dend,$table,$format,$time));
	}

	function andwheredate($column,$dbeg,$dend='',$table='',$format='',$time='')
	{
		return($this->_set_wheretypetime('date',$column,$dbeg,$dend,$table,$format,$time,true,false));
	}

	function andwherenotdate($column,$dbeg,$dend='',$table='',$format='',$time='')
	{
		return($this->_set_wheretypetime('date',$column,$dbeg,$dend,$table,$format,$time,true,true));
	}

	function orwheredate($column,$dbeg,$dend='',$table='',$format='',$time='')
	{
		return($this->_set_wheretypetime('date',$column,$dbeg,$dend,$table,$format,$time,false,false));
	}

	function orwherenotdate($column,$dbeg,$dend='',$table='',$format='',$time='')
	{
		return($this->_set_wheretypetime('date',$column,$dbeg,$dend,$table,$format,$time,false,true));
	}

	function whereunixtime($column,$dbeg,$dend='',$table='',$format='',$time='')
	{
		return($this->andwhereunixtime($column,$dbeg,$dend,$table,$format,$time));
	}

	function andwhereunixtime($column,$dbeg,$dend='',$table='',$format='',$time='')
	{
		return($this->_set_wheretypetime('unix',$column,$dbeg,$dend,$table,$format,$time,true,false));
	}

	function andwherenotunixtime($column,$dbeg,$dend='',$table='',$format='',$time='')
	{
		return($this->_set_wheretypetime('unix',$column,$dbeg,$dend,$table,$format,$time,true,true));
	}

	function orwhereunixtime($column,$dbeg,$dend='',$table='',$format='',$time='')
	{
		return($this->_set_wheretypetime('unix',$column,$dbeg,$dend,$table,$format,$time,false,false));
	}

	function orwherenotunixtime($column,$dbeg,$dend='',$table='',$format='',$time='')
	{
		return($this->_set_wheretypetime('unix',$column,$dbeg,$dend,$table,$format,$time,false,true));
	}

	function joinwheredate($table,$column,$dbeg,$dend='',$format='',$time='')
	{
		return($this->andjoinwheredate($table,$column,$dbeg,$dend,$table,$format,$time));
	}

	function andjoinwheredate($table,$column,$dbeg,$dend='',$format='',$time='')
	{
		return($this->_set_wheretypetime('date',$column,$dbeg,$dend,$table,$format,$time,true,false,$table));
	}

	function andjoinwherenotdate($table,$column,$dbeg,$dend='',$format='',$time='')
	{
		return($this->_set_wheretypetime('date',$column,$dbeg,$dend,$table,$format,$time,true,true,$table));
	}

	function orjoinwheredate($table,$column,$dbeg,$dend='',$format='',$time='')
	{
		return($this->_set_wheretypetime('date',$column,$dbeg,$dend,$table,$format,$time,false,false,$table));
	}

	function orjoinwherenotdate($table,$column,$dbeg,$dend='',$format='',$time='')
	{
		return($this->_set_wheretypetime('date',$column,$dbeg,$dend,$table,$format,$time,false,true,$table));
	}

	function joinwhereunixtime($table,$column,$dbeg,$dend='',$format='',$time='')
	{
		return($this->andjoinwhereunixtime($table,$column,$dbeg,$dend,$table,$format,$time));
	}

	function andjoinwhereunixtime($table,$column,$dbeg,$dend='',$format='',$time='')
	{
		return($this->_set_wheretypetime('unix',$column,$dbeg,$dend,$table,$format,$time,true,false,$table));
	}

	function andjoinwherenotunixtime($table,$column,$dbeg,$dend='',$format='',$time='')
	{
		return($this->_set_wheretypetime('unix',$column,$dbeg,$dend,$table,$format,$time,true,true,$table));
	}

	function orjoinwhereunixtime($table,$column,$dbeg,$dend='',$format='',$time='')
	{
		return($this->_set_wheretypetime('unix',$column,$dbeg,$dend,$table,$format,$time,false,false,$table));
	}

	function orjoinwherenotunixtime($table,$column,$dbeg,$dend='',$format='',$time='')
	{
		return($this->_set_wheretypetime('unix',$column,$dbeg,$dend,$table,$format,$time,false,true,$table));
	}

	function _format_typetime($format,$type='',$column='',$time='',$table='')
	{
		return($this->_expr_typetime($type,$column,$time,$format,$table));
	}

	function format_date($format,$column='',$time='',$table='')
	{
		return($this->_format_typetime('date',$column,$time,$format,$table));
	}

	function datetime_from_date($column='',$time='',$table='')
	{
		return($this->_expr_typetime('date',$column,$time,$this->_date['datetime'],$table));
	}

	function date_from_date($column='',$time='',$table='')
	{
		return($this->_expr_typetime('date',$column,$time,$this->_date['date'],$table));
	}

	function time_from_date($column='',$time='',$table='',$table='')
	{
		return($this->_expr_typetime('date',$column,$time,$this->_date['time'],$table));
	}

	function format_unixtime($format,$column='',$time='',$table='')
	{
		return($this->_format_typetime($column,'unix',$time,$format,$table));
	}

	function datetime_from_unixtime($column='',$time='',$table='')
	{
		return($this->_expr_typetime('unix',$column,$time,$this->_date['datetime'],$table));
	}

	function date_from_unixtime($column='',$time='',$table='')
	{
		return($this->_expr_typetime('unix',$column,$time,$this->_date['date'],$table));
	}

	function time_from_unixtime($column='',$time='',$table='')
	{
		return($this->_expr_typetime('unix',$column,$time,$this->_date['time'],$table));
	}

	function new_group($column,$table='')
	{
		$this->reset_group();

		if(($r = $this->_set_group($column,$table)) === false)
			return(false);

		$this->_group = $r;

		return(true);
	}

	function add_group($column,$table='')
	{
		if(($r = $this->_set_group($column,$table)) === false)
			return(false);

		$this->_group = array_merge($this->_group,$r);

		return(true);
	}

	function reset_group()
	{
		$this->_group = array();
	}

	function get_group()
	{
		if(($r = $this->_mk_group()) !== false)
			return($r);

		return(null);
	}

	function _set_group($column,$table='')
	{
		$table = trim((string) $table);

		$tabledot = '';

		if($table !== '')
		{
			if(isset($this->_table[$table]) === false)
				return(false);

			$tabledot = $this->quote_identifier($table).'.';
		}

		$r = array();

		if(is_array($column) === true)
		{
			$column = array_values($column);

			if(($nb = count($column)) === 0)
				return(false);

			for($i = 0;$i < $nb;$i++)
			{
				$col = $this->quote_identifier(trim((string) $column[$i]));

				if($col === '')
					continue;

				$r[] = $tabledot.$col;
			}

			if(isset($r[0]) === false)
				$r = false;

			return($r);
		}

		$column = $this->quote_identifier(trim((string) $column));

		if($column === '')
			return(false);

		$r[] = $tabledot.$column;

		return($r);
	}

	function _mk_group($column='',$table='')
	{
		if(xivo_empty($column) === true)
			$group = $this->_group;
		else
			$group = $this->_set_group($column,$table);

		if(is_array($group) === false
		|| isset($group[0]) === false
		|| ($r = implode(', ',$group)) === '')
			return(false);

		return($r);
	}

	function new_order($column,$sort='',$table='')
	{
		$this->reset_order();

		if(($r = $this->_set_order($column,$sort,$table)) === false)
			return(false);

		$this->_order = $r;

		return(true);
	}

	function add_order($column,$sort='',$table='')
	{
		if(($r = $this->_set_order($column,$sort,$table)) === false)
			return(false);

		$this->_order = array_merge($this->_order,$r);

		return(true);
	}

	function reset_order()
	{
		$this->_order = array();
	}

	function get_order()
	{
		if(($r = $this->_mk_order()) !== false)
			return($r);

		return(null);
	}

	function _set_order($column,$sort='',$table='')
	{
		$table = trim((string) $table);

		$tabledot = '';

		if($table !== '')
		{
			if(isset($this->_table[$table]) === false)
				return(false);

			$tabledot = $this->quote_identifier($table).'.';
		}

		$r = array();

		if(is_array($column) === true)
		{
			if(($arr = xivo_get_aks($column)) === false)
				return(false);

			for($i = 0;$i < $arr['cnt'];$i++)
			{
				$key = &$arr['keys'][$i];
				$val = $column[$key];

				if(($key = $this->quote_identifier($key)) === ''
				|| ($val = $this->_chk_sort($val)) === false)
					continue;

				$r[] = $tabledot.$key.' '.$val;
			}

			if(isset($r[0]) === false)
				$r = false;

			return($r);
		}

		if(($column = $this->quote_identifier($column)) === ''
		|| ($sort = $this->_chk_sort($sort)) === false)
			return(false);

		$r[] = $tabledot.$column.' '.$sort;

		return($r);
	}

	function _mk_order($column='',$sort='',$table='')
	{
		if(xivo_empty($column) === true)
			$order = $this->_order;
		else
			$order = $this->_set_order($column,$sort,$table);

		if(is_array($order) === false
		|| isset($order[0]) === false
		|| ($r = implode(', ',$order)) === '')
			return(false);

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

	function limit($offset,$line='')
	{
		$this->reset_limit();

		if(($r = $this->_set_limit($offset,$line)) === false)
			return(false);

		$this->_limit = $r;

		return(true);
	}

	function get_limit()
	{
		if(($r = $this->_mk_limit()) !== false)
			return($r);

		return(null);
	}

	function reset_limit()
	{
		$this->_limit = array();
	}

	function _set_limit($offset,$line='')
	{
		$r = array();

		if(is_array($offset) === true && isset($offset[0]) === true)
		{
			if(isset($offset[1]) === true)
				$line = $offset[1];

			$offset = $offset[0];
		}

		$offset = (string) $offset;
		$line = (string) $line;

		if($offset === '')
			return(false);

		$r[0] = xivo_uint($offset);

		if($line !== '')
			$r[1] = xivo_uint($line);
			
		return($r);
	}

	function _mk_limit($offset='',$line='')
	{
		if(xivo_empty($offset) === true)
			$limit = $this->_limit;
		else
			$limit = $this->_set_limit($offset,$line);

		if(is_array($limit) === false || isset($limit[0]) === false)
			return(false);

		$r = xivo_uint($limit[0]);

		if(isset($limit[1]) === true)
			$r .= ', '.xivo_uint($limit[1]);

		return($r);
	}

	function select_singlecolumn($query='',$free=true)
	{
		$query = (string) $query;
		$free = (bool) $free;

		if((xivo_haslen($query) === false
		   && ($query = $this->get_select()) === null) === true
		|| $this->query($query,($free === true ? false : true)) === false)
			return(false);

		$r = array();

		for($i = 0;$m = $this->fetch_row();$i++)
			$r[$i] = $m[0];

		if($this->_free === true && $free === true)
			$this->free();

		return($r);
	}

	function select_columnpair($query='',$free=true)
	{
		$query = (string) $query;
		$free = (bool) $free;

		if((xivo_haslen($query) === false
		   && ($query = $this->get_select()) === null) === true
		|| $this->query($query,($free === true ? false : true)) === false)
			return(false);

		$r = array();

		while(($m = $this->fetch_row()) !== false)
		{
			if(isset($m[0],$m[1]) === false)
				return(false);

			$r[$m[0]] = $m[1];
		}

		if($this->_free === true && $free === true)
			$this->free();

		return($r);
	}

	function select_count($query='',$column='',$table='',$free=true)
	{
		$query = (string) $query;
		$free = (bool) $free;

		if(xivo_haslen($query) === false)
		{
			$column = trim((string) $column);
			$table = trim((string) $table);

			$tabledot = '';

			if($table !== '')
			{
				if(isset($this->_table[$table]) === false)
					return(false);

				$tabledot = $this->quote_identifier($table).'.';
			}

			if($column === '' || $column === '*')
				$column = '*';
			else
				$column = $this->quote_identifier($column);

			if(($from = $this->get_from()) === null)
				return(false);

			$query = 'SELECT COUNT('.$tabledot.$column.') FROM '.$from;

			if(($join = $this->get_join()) !== null)
				$query .= ' '.$join;

			if(($where = $this->get_where()) !== null)
				$query .= ' WHERE '.$where;

			if(($group = $this->get_group()) !== null)
				$query .= ' GROUP BY '.$group;

			if(($order = $this->get_order()) !== null)
				$query .= ' ORDER BY '.$order;

			if(($limit = $this->get_limit()) !== null)
				$query .= ' LIMIT '.$limit;
		}

		if($this->query($query,($free === true ? false : true)) === false)
			return(false);

		if(($r = $this->fetch_row()) !== false)
			$r = (int) $r[0];

		if($this->_free === true && $free === true)
			$this->free();

		return($r);
	}

	function select_one($query='',$free=true)
	{
		$query = (string) $query;
		$free = (bool) $free;

		if(xivo_haslen($query) === false)
		{
			$limit = $this->_limit;
			$this->limit(1);

			if(($query = $this->get_select()) === null)
			{
				$this->_limit = $limit;
				return(false);
			}

			$this->_limit = $limit;
		}

		if($this->query($query,($free === true ? false : true)) === false)
			return(false);

		if(($r = $this->fetch_row()) !== false)
			$r = $r[0];

		if($this->_free === true && $free === true)
			$this->free();

		return($r);
	}

	function select_single($query='',$free=true)
	{
		$query = (string) $query;
		$free = (bool) $free;

		if(xivo_haslen($query) === false)
		{
			$limit = $this->_limit;
			$this->limit(1);

			if(($query = $this->get_select()) === null)
			{
				$this->_limit = $limit;
				return(false);
			}

			$this->_limit = $limit;
		}

		if($this->query($query,($free === true ? false : true)) === false)
			return(false);

		$r = $this->fetch_assoc();

		if($this->_free === true && $free === true)
			$this->free();

		return($r);
	}

	function select_all($query='',$free=true)
	{
		$query = (string) $query;
		$free = (bool) $free;

		if((xivo_haslen($query) === false
		   && ($query = $this->get_select()) === null) === true
		|| $this->query($query,($free === true ? false : true)) === false)
			return(false);

		$r = array();

		for($i = 0;$m = $this->fetch_assoc();$i++)
			$r[$i] = $m;

		if($this->_free === true && $free === true)
			$this->free();

		return($r);
	}
}

?>
