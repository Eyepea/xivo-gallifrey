<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version, with a Section 7 Additional
# Permission as follows:
#   This notice constitutes a grant of such permission as is necessary
#   to combine or link this software, or a modified version of it, with
#   the MySQL Client Library, as published by Sun Microsystems and/or
#   MySQL AB, or a derivative work of the MySQL CLient LIbrary, and to
#   copy, modify, and distribute the resulting work. The MySQL Client
#   LIbrary is licensed under version 2 of the GNU General Public
#   License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

require_once(dwho_file::joinpath(DWHO_PATH_ROOT,'datastorage','adapter','abstract','abstract.inc'));

class dwho_datastorage_adapter_abstract_sql extends dwho_datastorage_adapter_abstract
{
	var $_query		= '';
	var $_meta		= 'sql';
	var $_tablecolumn	= array();
	var $_colalias		= array();
	var $_join		= array();
	var $_jointype		= array();
	var $_joinwhere		= array();
	var $_aggjoinwhere	= array();
	var $_group		= array();

	function _get_scalar($val,$trim=false)
	{
		$r = '';

		if(is_object($val) === true)
		{
			if(isset($val->scalar) === false)
				return($r);

			$val = $val->scalar;
		}

		if(is_scalar($val) === false)
			return($r);
		else if((bool) $trim === true)
			$val = trim($val);

		if(dwho_has_len($val) === true)
			$r = $val;

		return($r);
	}

	function quote($val)
	{
		switch(gettype($val))
		{
			case 'boolean':
				$val = intval($val);
			case 'integer':
			case 'float':
				return('QUOTE('.$val.')');
			case 'object':
				if(isset($val->scalar) === false)
					return(false);
				return('QUOTE('.$val->scalar.')');
			case 'NULL':
				return('NULL');
		}

		return('QUOTE(\''.$this->escape_string(strval($val)).'\')');
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

	function is_escaped($val)
	{
		if(is_scalar($val) === false || isset($val{0},$val{1}) === false)
			return(false);

		$lastchar = substr($val,-1);

		if(($val{0} === '\'' && $lastchar === '\'') === true
		|| ($val{0} === '"' && $lastchar === '"') === true)
			return(true);

		return(false);
	}

	function concat($list)
	{
		if(is_array($list) === false)
			return(false);

		return(implode('||',array_map(array(&$this,'escape'),$list)));
	}

	function reset_all()
	{
		parent::reset_all();
		$this->reset_join();
		$this->reset_group();
	}

	function reset_column()
	{
		parent::reset_column();
		$this->_tablecolumn = $this->_colalias = array();
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
		if(is_array($this->_join) === false
		|| empty($this->_join) === true)
			return(null);

		$r = '';

		foreach($this->_join as $table => $join)
		{
			$r .= ' '.$join;

			if(isset($this->_joinwhere[$table]) === true)
				$r .= ' ON '.$this->_joinwhere[$table];
		}

		if(($r = ltrim($r)) === '')
			$r = null;

		return($r);
	}

	function reset_join()
	{
		$this->_join = $this->_jointype = $this->_joinwhere = $this->_aggjoinwhere = array();
	}

	function _chk_join($type)
	{
		$type = strtoupper($type);

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

	function _mk_join($keytype,$table,$cond='',$val='',$column='')
	{
		if(($type = $this->_chk_join($keytype)) === false
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

		if(isset($this->_jointype[$keytype]) === false)
			$this->_jointype[$keytype] = array();

		$this->_jointype[$keytype][$table] = 1;

		return($this->_mk_column($table,$column));
	}

	function _mk_table($table)
	{
		if(dwho_has_len($table) === true)
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

		if(dwho_has_len($key) === false
		|| dwho_has_len($val) === false
		|| isset($this->_table[$key]) === true)
			return(false);

		$this->_table[$key] = $val;

		return(array($key,$val));
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
		if(($strcolumn = $this->_get_scalar($column,true)) !== '')
		{
			if($strcolumn !== '*')
				$column = (array) $strcolumn;
			else
			{
				$this->_colall[] = $table;
				return(true);
			}
		}

		if(is_array($column) === false || empty($column) === true)
			return(null);

		$tabledot = $table.'.';
		$tabledotlen = strlen($tabledot);

		$keys = array_keys($column);
		$noassoc = in_array(0,$keys,true);
		$nb = count($keys);

		if(isset($this->_colalias[$table]) === false)
			$this->_colalias[$table] = array();

		for($i = 0;$i < $nb;$i++)
		{
			$key = &$keys[$i];
			$val = &$column[$key];

			$object = false;

			if(is_object($val) === true)
			{
				if(isset($val->scalar) === false)
					return(false);

				$object = true;
				$val = $val->scalar;
			}

			if(dwho_has_len($key) === false || dwho_has_len($val) === false)
				continue;
			else if($val === '*')
			{
				$this->_column[] = sprintf('%s.%s',$this->quote_identifier($table),
								   $val);
				continue;
			}
			else if($object === true || preg_match('/^[a-zA-Z0-9_\-\.]+$/',$val) === 0)
			{
				if($noassoc === true && is_int($key) === true)
					$col = sprintf('%s',$val);
				else
				{
					$col = sprintf('%s AS %s',$val,
								  $this->quote_identifier($key));
					$this->_colalias[$table][$key] = $val;
				}

				$this->_column[] = $col;
				continue;
			}

			if(strncasecmp($val,$tabledot,$tabledotlen) === 0)
				$val = substr($val,$tabledotlen);

			if(dwho_has_len($val) === false)
				continue;

			$col = sprintf('%s.%s',$this->quote_identifier($table),
					       $this->quote_identifier($val));

			if(isset($this->_tablecolumn[$table]) === false)
				$this->_tablecolumn[$table] = array();

			$this->_tablecolumn[$table][] = $col;

			if($noassoc === false || is_int($key) === false)
			{
				$col .= ' AS '.$this->quote_identifier($key);
				$this->_colalias[$table][$key] = $val;
			}

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
		$table = trim($table);
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

		return(($this->query($query) !== false ? $this->affected_rows() : false));
	}

	function _insertupdate($proc,$table,&$data,$set=false,$condwhere='',$valwhere='',$usewhere=false,$useorder=false,$uselimit=false)
	{
		$proc = strtolower($proc);
		$table = trim($table);
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

		return(($this->query($query) !== false ? $this->affected_rows() : false));
	}

	function _mk_sep(&$data,$set=false,$table='',$where=false)
	{
		$data = (array) $data;
		$set = (bool) $set;
		$table = trim($table);
		$where = (bool) $where;

		if(empty($data) === true)
			return(false);

		$tabledot = '';

		if($table !== '')
		{
			if(isset($this->_table[$table]) === false)
				return(false);

			$tabledot = $this->quote_identifier($table).'.';
		}

		$r = array('str' => array(),'keys' => array());

		foreach($data as $key => $val)
		{
			if(dwho_has_len($key) === false)
				continue;
			else if($set === true)
			{
				if($where === true && $val === null && $this->escape($val) === 'NULL')
					$r['str'][] = $tabledot.$this->quote_identifier($key).' IS NULL';
				else
					$r['str'][] = $tabledot.$this->quote_identifier($key).' = '.$this->escape($val);
			}
			else
			{
				$r['keys'][] = $tabledot.$this->quote_identifier($key);
				$r['str'][] = $this->escape($val);
			}
		}

		if(isset($r['str'][0]) === false)
			return(false);

		return($r);
	}

	function _set_where($cond,$val='',$condand=true,$and=true,$not=false,$join=false,$jointablecond=false)
	{
		if($jointablecond === false || isset($this->_table[$jointablecond]) === false)
			$jointablecond = $join;

		if(($where = $this->_mk_where($cond,$val,$condand,$jointablecond,$join)) === false
		|| ($join !== false && isset($this->_join[$join]) === false) === true)
			return(false);
		else if($join === false)
			$ref = &$this->_where;
		else
		{
			if(isset($this->_joinwhere[$join]) === false)
				$this->_joinwhere[$join] = '';

			$ref = &$this->_joinwhere[$join];
		}

		if((bool) $not === true)
			$where = 'NOT ('.$where.')';

		if($ref === '')
			$ref = $where;
		else
			$ref .= ((bool) $and === true ? ' AND ' : ' OR ').$where;

		return(true);
	}

	function _mk_where($cond,$val='',$condand=true,$jointablecond=false,$join=false)
	{
		if(is_array($cond) === true)
		{
			if($jointablecond !== false)
				$val = $jointablecond;

			if(($condsep = $this->_mk_sep($cond,true,$val,true)) === false)
				return(false);

			$r = implode(((bool) $condand === true ? ' AND ' : ' OR '),$condsep['str']);
		}
		else if(is_array($val) === true || dwho_has_len($val) === true)
		{
			$val = (array) $val;

			$map = array_map(array(&$this,'escape'),$val);

			// XXX Fix condition with SQLite2
			// when there are multi left join with comparison of constant.

			if($join !== false
			&& $this->get_type() === 'sqlite'
			&& isset($this->_jointype['left'],$this->_jointype['left'][$join]) === true
			&& empty($this->_tablecolumn[$join]) === false)
			{
				if(isset($this->_tablecolumn[$join],$this->_tablecolumn[$join][0]) === true)
					$this->_fix_leftjoin_column = $this->_tablecolumn[$join][0];
				else
					$this->_fix_leftjoin_column = $this->_tablecolumn[key($this->_tablecolumn)][0];

				$map = array_map(array(&$this,'_fix_leftjoin_condition'),$map);

				$this->_fix_leftjoin_condition = false;
			}

			$r = vsprintf(dwho_printf_escape($cond,count($val)),$map);
		}
		else if(dwho_has_len($cond) === true)
			$r = (string) $cond;
		else
			return(false);

		if(trim($r) === '')
			return(false);

		return($r);
	}

	function valid_logic($logic)
	{
		$r = '';

		if(is_string($logic) === true)
			$next = strtoupper($logic);
		else
			return($r);

		switch($next)
		{
			case 'AND':
			case 'OR':
			case 'XOR':
				$r = $logic;
				break;
			default:
		}

		return($r);
	}

	function aggregate_where($all=false,$agglogic='')
	{
		if($this->_where === '')
			return(false);
		else if((bool) $all === true || isset($this->_aggwhere[0]) === false)
		{
			$this->_where = '('.$this->_where.')';
			$this->_aggwhere[] = strlen($this->_where) + 1;
			return(true);
		}
		else if(($last = dwho_last_key($this->_aggwhere)) === false)
			return(false);

		$start = substr($this->_where,0,$this->_aggwhere[$last]);
		$end = substr($this->_where,$this->_aggwhere[$last]);

		if(preg_match('/^([\s])?(AND|OR|XOR)([\(\s])?/i',$end,$match) === 0)
			return(false);

		if(($agglogic = $this->valid_logic($agglogic)) === '')
			$logic = $match[1].strtoupper($match[2]);
		else
			$logic = $match[1].$agglogic;

		$len = strlen($match[2]);

		if(isset($match[3]{0}) === false)
			$logic .= ' ';
		else
		{
			$len++;
			$logic .= $match[3];
		}

		$this->_where = $start.$logic.'('.substr(ltrim($end),$len).')';
		$this->_aggwhere[] = strlen($this->_where) + 1;

		return(true);
	}

	function aggregate_where_all()
	{
		return($this->aggregate_where(true));
	}

	function aggregate_joinwhere($table,$all=false,$agglogic='')
	{
		if(isset($this->_joinwhere[$table]) === false
		|| $this->_joinwhere[$table] === '')
			return(false);
		else if(isset($this->_aggjoinwhere[$table]) === false)
			$this->_aggjoinwhere[$table] = array();

		if((bool) $all === true || isset($this->_aggjoinwhere[$table][0]) === false)
		{
			$this->_joinwhere[$table] = '('.$this->_joinwhere[$table].')';
			$this->_aggjoinwhere[$table][] = strlen($this->_joinwhere[$table]) + 1;
			return(true);
		}
		else if(($last = dwho_last_key($this->_aggjoinwhere[$table])) === false)
			return(false);

		$start = substr($this->_joinwhere[$table],0,$this->_aggjoinwhere[$table][$last]);
		$end = substr($this->_joinwhere[$table],$this->_aggjoinwhere[$table][$last]);

		if(preg_match('/^([\s])?(AND|OR|XOR)([\(\s])?/i',$end,$match) === 0)
			return(false);

		if(($agglogic = $this->valid_logic($agglogic)) === '')
			$logic = $match[1].strtoupper($match[2]);
		else
			$logic = $match[1].$agglogic;

		$len = strlen($match[2]);

		if(isset($match[3]{0}) === false)
			$logic .= ' ';
		else
		{
			$len++;
			$logic .= $match[3];
		}

		$this->_joinwhere[$table] = $start.$logic.'('.substr(ltrim($end),$len).')';
		$this->_aggjoinwhere[$table][] = strlen($this->_joinwhere[$table]) + 1;

		return(true);
	}

	function aggregate_joinwhere_all($table)
	{
		return($this->aggregate_where($table,true));
	}

	function joinwhere($table,$cond,$val='',$condand=true,$tablecond=false)
	{
		return($this->andjoinwhere($table,$cond,$val,$condand,$tablecond));
	}

	function andjoinwhere($table,$cond,$val='',$condand=true,$tablecond=false)
	{
		return($this->_set_where($cond,$val,$condand,true,false,$table,$tablecond));
	}

	function andjoinwherenot($table,$cond,$val='',$condand=true,$tablecond=false)
	{
		return($this->_set_where($cond,$val,$condand,true,true,$table,$tablecond));
	}

	function orjoinwhere($table,$cond,$val='',$condand=true,$tablecond=false)
	{
		return($this->_set_where($cond,$val,$condand,false,false,$table,$tablecond));
	}

	function orjoinwherenot($table,$cond,$val='',$condand=true,$tablecond=false)
	{
		return($this->_set_where($cond,$val,$condand,false,true,$table,$tablecond));
	}

	function _set_wherenull($column,$table='',$and=true,$not=false,$join=false,$jointablecond=false)
	{
		if($join === false)
			$tablecond = $table;
		else if($jointablecond === false || isset($this->_table[$jointablecond]) === false)
			$tablecond = $join;
		else
			$tablecond = $jointablecond;

		if(($where = $this->_mk_wherenull($column,$tablecond)) === false
		|| ($join !== false && isset($this->_join[$join]) === false) === true)
			return(false);
		else if($join === false)
			$ref = &$this->_where;
		else
		{
			if(isset($this->_joinwhere[$join]) === false)
				$this->_joinwhere[$join] = '';

			$ref = &$this->_joinwhere[$join];
		}

		if((bool) $not === true)
			$where = 'NOT ('.$where.')';

		if($ref === '')
			$ref = $where;
		else
			$ref .= ((bool) $and === true ? ' AND ' : ' OR ').$where;

		return(true);
	}

	function _mk_wherenull($column,$table='')
	{
		$column = trim($column);
		$table = trim($table);

		if($column === '')
			return(false);
		else if($table === '')
			$column = $this->quote_identifier($column);
		else if(isset($this->_table[$table]) === false)
			return(false);
		else
			$column = $this->quote_identifier($table).'.'.$this->quote_identifier($column);

		return($column.' IS NULL');
	}

	function joinwherenull($table,$column,$tablecond=false)
	{
		return($this->andjoinwherenull($table,$column,$tablecond));
	}

	function andjoinwherenull($table,$column,$tablecond=false)
	{
		return($this->_set_wherenull($column,$table,true,false,$table,$tablecond));
	}

	function andjoinwherenotnull($table,$column,$tablecond=false)
	{
		return($this->_set_wherenull($column,$table,true,true,$table,$tablecond));
	}

	function orjoinwherenull($table,$column,$tablecond=false)
	{
		return($this->_set_wherenull($column,$table,false,false,$table,$tablecond));
	}

	function orjoinwherenotnull($table,$column,$tablecond=false)
	{
		return($this->_set_wherenull($column,$table,false,true,$table,$tablecond));
	}

	function _set_wherebetween($column,$beg,$end,$table='',$and=true,$not=false,$join=false,$jointablecond=false)
	{
		if($join === false)
			$tablecond = $table;
		else if($jointablecond === false || isset($this->_table[$jointablecond]) === false)
			$tablecond = $join;
		else
			$tablecond = $jointablecond;

		if(($between = $this->_mk_wherebetween($column,$beg,$end,$table)) === false
		|| ($join !== false && isset($this->_join[$join]) === false) === true)
			return(false);
		else if($join === false)
			$ref = &$this->_where;
		else
		{
			if(isset($this->_joinwhere[$join]) === false)
				$this->_joinwhere[$join] = '';

			$ref = &$this->_joinwhere[$join];
		}

		if((bool) $not === true)
			$between = 'NOT ('.$between.')';

		if($ref === '')
			$ref = $between;
		else
			$ref .= ((bool) $and === true ? ' AND ' : ' OR ').$between;

		return(true);
	}

	function _mk_wherebetween($column,$beg,$end,$table='')
	{
		$column = trim($column);
		$table = trim($table);

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

	function joinwherebetween($table,$column,$beg,$end,$tablecond=false)
	{
		return($this->andjoinwherebetween($table,$column,$beg,$end,$tablecond));
	}

	function andjoinwherebetween($table,$column,$beg,$end,$tablecond=false)
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,true,false,$table,$tablecond));
	}

	function andjoinwherenotbetween($table,$column,$beg,$end,$tablecond=false)
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,true,true,$table,$tablecond));
	}

	function orjoinwherebetween($table,$column,$beg,$end,$tablecond=false)
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,false,false,$table,$tablecond));
	}

	function orjoinwherenotbetween($table,$column,$beg,$end,$tablecond=false)
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,false,true,$table,$tablecond));
	}

	function _set_wherebetweencolumn($val,$beg,$end,$table='',$and=true,$not=false,$join=false,$jointablecond=false)
	{
		if($join === false)
			$tablecond = $table;
		else if($jointablecond === false || isset($this->_table[$jointablecond]) === false)
			$tablecond = $join;
		else
			$tablecond = $jointablecond;

		if(($between = $this->_mk_wherebetweencolumn($val,$beg,$end,$tablecond)) === false
		|| ($join !== false && isset($this->_join[$join]) === false) === true)
			return(false);
		else if($join === false)
			$ref = &$this->_where;
		else
		{
			if(isset($this->_joinwhere[$join]) === false)
				$this->_joinwhere[$join] = '';

			$ref = &$this->_joinwhere[$join];
		}

		if((bool) $not === true)
			$between = 'NOT ('.$between.')';

		if($ref === '')
			$ref = $between;
		else
			$ref .= ((bool) $and === true ? ' AND ' : ' OR ').$between;

		return(true);
	}

	function _mk_wherebetweencolumn($val,$beg,$end,$table='')
	{
		$beg = trim($beg);
		$end = trim($end);
		$table = trim($table);

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

	function joinwherebetweencolumn($table,$val,$beg,$end,$tablecond=false)
	{
		return($this->andjoinwherebetweencolumn($table,$val,$beg,$end,$tablecond));
	}

	function andjoinwherebetweencolumn($table,$val,$beg,$end,$tablecond=false)
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,true,false,$table,$tablecond));
	}

	function andjoinwherenotbetweencolumn($table,$val,$beg,$end,$tablecond=false)
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,true,true,$table,$tablecond));
	}

	function orjoinwherebetweencolumn($table,$val,$beg,$end,$tablecond=false)
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,false,false,$table,$tablecond));
	}

	function orjoinwherenotbetweencolumn($table,$val,$beg,$end,$tablecond=false)
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,false,true,$table,$tablecond));
	}

	function _set_wherein($column,&$list,$table='',$keys=false,$and=true,$not=false,$join=false,$jointablecond=false)
	{
		if($join === false)
			$tablecond = $table;
		else if($jointablecond === false || isset($this->_table[$jointablecond]) === false)
			$tablecond = $join;
		else
			$tablecond = $jointablecond;

		if(($in = $this->_mk_wherein($column,$list,$tablecond,$keys,$not)) === false
		|| ($join !== false && isset($this->_join[$join]) === false) === true)
			return(false);
		else if($join === false)
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
			$ref .= ((bool) $and === true ? ' AND ' : ' OR ').$in;

		return(true);
	}

	function _mk_wherein($column,$list,$table='',$keys=false,$not=false)
	{
		$column = trim($column);
		$table = trim($table);

		if($column === '' || is_array($list) === false || empty($list) === true)
			return(false);
		else if($table === '')
			$column = $this->quote_identifier($column);
		else if(isset($this->_table[$table]) === false)
			return(false);
		else
			$column = $this->quote_identifier($table).'.'.$this->quote_identifier($column);

		$func = (bool) $not === false ? '' : 'NOT ';

		if((bool) $keys === true)
			$list = array_keys($list);

		$strlist = implode(',',array_map(array(&$this,'escape'),$list));

		if(trim($strlist) === '')
			return(false);

		return(sprintf('%s %sIN(%s)',$column,$func,$strlist));
	}

	function joinwherein($table,$column,$list,$keys=false,$tablecond=false)
	{
		return($this->andjoinwherein($table,$column,$list,null,$keys,$tablecond));
	}

	function andjoinwherein($table,$column,$list,$keys=false,$tablecond=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,true,false,$table,$tablecond));
	}

	function andjoinwherenotin($table,$column,$list,$keys=false,$tablecond=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,true,true,$table,$tablecond));
	}

	function orjoinwherein($table,$column,$list,$keys=false,$tablecond=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,false,false,$table,$tablecond));
	}

	function orjoinwherenotin($table,$column,$list,$keys=false,$tablecond=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,false,true,$table,$tablecond));
	}

	function _set_wherelike($column,$pattern,$table='',$format='',$condand=true,$and=true,$not=false,$join=false,$jointablecond=false)
	{
		if($join === false)
			$tablecond = $table;
		else if($jointablecond === false || isset($this->_table[$jointablecond]) === false)
			$tablecond = $join;
		else
			$tablecond = $jointablecond;

		if(($like = $this->_mk_wherelike($column,$pattern,$tablecond,$format,$condand,$not)) === false
		|| ($join !== false && isset($this->_join[$join]) === false) === true)
			return(false);
		else if($join === false)
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
			$ref .= ((bool) $and === true ? ' AND ' : ' OR ').$like;

		return(true);
	}

	function _mk_wherelike($column,$pattern,$table='',$format='',$condand=true,$not=false)
	{
		if(is_scalar($column) === true)
			$column = trim($column);

		$table = trim($table);
		$format = $format !== '' ? strtolower($format) : 'exact';

		if(is_scalar($pattern) === true || is_null($pattern) === true)
			$pattern = array((string) $pattern);
		else if(is_array($pattern) === false || empty($pattern) === true)
			return(false);

		if(dwho_empty($column) === true)
			return(false);
		else if($table === '')
			$column = $this->quote_identifier($column);
		else if(isset($this->_table[$table]) === false)
				return(false);
		else
			$column = $this->quote_identifier($table).'.'.$this->quote_identifier($column);

		$func = (bool) $not === false ? 'LIKE' : 'NOT LIKE';

		switch($format)
		{
			case 'begin':
				$like_format = '%s%%';
				break;
			case 'contain':
				$like_format = '%%%s%%';
				break;
			case 'end':
				$like_format = '%%%s';
				break;
			default:
				$like_format = '%s';
		}

		$condand = (bool) $condand === true ? ' AND ' : ' OR ';

		$r = '';

		foreach($pattern as $value)
		{
			$value = $this->escape_like($this->escape_string($value));

			if($r !== '')
				$r .= $condand;

			$r .= sprintf('%s %s \''.$like_format.'\'',$column,$func,$value);
		}

		if($r === '')
			$r = false;

		return($r);
	}

	function escape_like($str)
	{
		return(str_replace(array('%','_'),array('\%','\_'),$str));
	}

	function joinwherelike($table,$column,$pattern,$format,$condand=true,$tablecond=false)
	{
		return($this->andjoinwherelike($table,$column,$pattern,$format,$condand,$tablecond));
	}

	function andjoinwherelike($table,$column,$pattern,$format,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,$condand,true,false,$table,$tablecond));
	}

	function andjoinwherenotlike($table,$column,$pattern,$format,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,$condand,true,true,$table,$tablecond));
	}

	function orjoinwherelike($table,$column,$pattern,$format,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,$condand,false,false,$table,$tablecond));
	}

	function orjoinwherenotlike($table,$column,$pattern,$format,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,$condand,false,true,$table,$tablecond));
	}

	function joinwhereexact($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->andjoinwhereexact($table,$column,$pattern,$condand,$tablecond));
	}

	function andjoinwhereexact($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',$condand,true,false,$table,$tablecond));
	}

	function andjoinwherenotexact($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',$condand,true,true,$table,$tablecond));
	}

	function orjoinwhereexact($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',$condand,false,false,$table,$tablecond));
	}

	function orjoinwherenotexact($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',$condand,false,true,$table,$tablecond));
	}

	function joinwherebegin($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->andjoinwherebegin($table,$column,$pattern,$condand,$tablecond));
	}

	function andjoinwherebegin($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',$condand,true,false,$table,$tablecond));
	}

	function andjoinwherenotbegin($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',$condand,true,true,$table,$tablecond));
	}

	function orjoinwherebegin($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',$condand,false,false,$table,$tablecond));
	}

	function orjoinwherenotbegin($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',$condand,false,true,$table,$tablecond));
	}

	function joinwherecontain($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->andjoinwherecontain($table,$column,$pattern,$condand,$tablecond));
	}

	function andjoinwherecontain($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',$condand,true,false,$table,$tablecond));
	}

	function andjoinwherenotcontain($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',$condand,true,true,$table,$tablecond));
	}

	function orjoinwherecontain($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',$condand,false,false,$table,$tablecond));
	}

	function orjoinwherenotcontain($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',$condand,false,true,$table,$tablecond));
	}

	function joinwhereend($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->andjoinwhereend($table,$column,$pattern,$condand,$tablecond));
	}

	function andjoinwhereend($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',$condand,true,false,$table,$tablecond));
	}

	function andjoinwherenotend($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',$condand,true,true,$table,$tablecond));
	}

	function orjoinwhereend($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',$condand,false,false,$table,$tablecond));
	}

	function orjoinwherenotend($table,$column,$pattern,$condand=true,$tablecond=false)
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',$condand,false,true,$table,$tablecond));
	}

	function _set_wheretypetime($type,$column,$dbeg,$dend='',$table='',$format='',$time='',$and=true,$not=false,$join=false,$jointablecond=false)
	{
		if($join === false)
			$tablecond = $table;
		else if($jointablecond === false || isset($this->_table[$jointablecond]) === false)
			$tablecond = $join;
		else
			$tablecond = $jointablecond;

		if(($time = $this->_mk_wheretypetime($type,$column,$dbeg,$dend,$tablecond,$format,$time)) === false
		|| ($join !== false && isset($this->_join[$join]) === false) === true)
			return(false);
		else if($join === false)
			$ref = &$this->_where;
		else
		{
			if(isset($this->_joinwhere[$join]) === false)
				$this->_joinwhere[$join] = '';

			$ref = &$this->_joinwhere[$join];
		}

		if((bool) $not === true)
			$time = 'NOT ('.$time.')';

		if($ref === '')
			$ref = $time;
		else
			$ref .= ((bool) $and === true ? ' AND ' : ' OR ').$time;

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
		else if($dend === '')
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

	function joinwheredate($table,$column,$dbeg,$dend='',$format='',$time='',$tablecond=false)
	{
		return($this->andjoinwheredate($table,$column,$dbeg,$dend,$table,$format,$time,$tablecond));
	}

	function andjoinwheredate($table,$column,$dbeg,$dend='',$format='',$time='',$tablecond=false)
	{
		return($this->_set_wheretypetime('date',$column,$dbeg,$dend,$table,$format,$time,true,false,$table,$tablecond));
	}

	function andjoinwherenotdate($table,$column,$dbeg,$dend='',$format='',$time='',$tablecond=false)
	{
		return($this->_set_wheretypetime('date',$column,$dbeg,$dend,$table,$format,$time,true,true,$table,$tablecond));
	}

	function orjoinwheredate($table,$column,$dbeg,$dend='',$format='',$time='',$tablecond=false)
	{
		return($this->_set_wheretypetime('date',$column,$dbeg,$dend,$table,$format,$time,false,false,$table,$tablecond));
	}

	function orjoinwherenotdate($table,$column,$dbeg,$dend='',$format='',$time='',$tablecond=false)
	{
		return($this->_set_wheretypetime('date',$column,$dbeg,$dend,$table,$format,$time,false,true,$table,$tablecond));
	}

	function joinwhereunixtime($table,$column,$dbeg,$dend='',$format='',$time='',$tablecond=false)
	{
		return($this->andjoinwhereunixtime($table,$column,$dbeg,$dend,$table,$format,$time,$tablecond));
	}

	function andjoinwhereunixtime($table,$column,$dbeg,$dend='',$format='',$time='',$tablecond=false)
	{
		return($this->_set_wheretypetime('unix',$column,$dbeg,$dend,$table,$format,$time,true,false,$table,$tablecond));
	}

	function andjoinwherenotunixtime($table,$column,$dbeg,$dend='',$format='',$time='',$tablecond=false)
	{
		return($this->_set_wheretypetime('unix',$column,$dbeg,$dend,$table,$format,$time,true,true,$table,$tablecond));
	}

	function orjoinwhereunixtime($table,$column,$dbeg,$dend='',$format='',$time='',$tablecond=false)
	{
		return($this->_set_wheretypetime('unix',$column,$dbeg,$dend,$table,$format,$time,false,false,$table,$tablecond));
	}

	function orjoinwherenotunixtime($table,$column,$dbeg,$dend='',$format='',$time='',$tablecond=false)
	{
		return($this->_set_wheretypetime('unix',$column,$dbeg,$dend,$table,$format,$time,false,true,$table,$tablecond));
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
		$table = trim($table);

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
			if(empty($column) === true)
				return(false);

			foreach($column as $val)
			{
				$col = $this->quote_identifier(trim($val));

				if($col !== '')
					$r[] = $tabledot.$col;
			}

			if(isset($r[0]) === false)
				$r = false;

			return($r);
		}

		$column = $this->quote_identifier(trim($column));

		if($column === '')
			return(false);

		$r[] = $tabledot.$column;

		return($r);
	}

	function _mk_group($column='',$table='')
	{
		if(dwho_empty($column) === true)
			$group = $this->_group;
		else
			$group = $this->_set_group($column,$table);

		if(is_array($group) === false
		|| isset($group[0]) === false
		|| ($r = implode(', ',$group)) === '')
			return(false);

		return($r);
	}

	function _set_order($column,$sort='',$table='')
	{
		$table = trim($table);

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
			if(empty($column) === true)
				return(false);

			foreach($column as $key => $val)
			{
				if(($key = $this->quote_identifier($key)) !== '')
					$r[] = $tabledot.$key.' '.$this->_set_sort($val);
			}

			if(isset($r[0]) === false)
				$r = false;

			return($r);
		}
		else if(($column = $this->quote_identifier($column)) === '')
			return(false);

		$r[] = $tabledot.$column.' '.$this->_set_sort($sort);

		return($r);
	}

	function _mk_order($column='',$sort='',$table='')
	{
		if(dwho_empty($column) === true)
			$order = $this->_order;
		else
			$order = $this->_set_order($column,$sort,$table);

		if(is_array($order) === false
		|| isset($order[0]) === false
		|| ($r = implode(', ',$order)) === '')
			return(false);

		return($r);
	}

	function _set_sort($sort='')
	{
		$sort = strtoupper($sort);

		switch($sort)
		{
			case 'D':
			case 'DESC':
			case SORT_DESC:
				return('DESC');
		}

		return('ASC');
	}

	function _mk_limit($offset='',$line='')
	{
		if(dwho_empty($offset) === true)
			$limit = $this->_limit;
		else
			$limit = $this->_set_limit($offset,$line);

		if(is_array($limit) === false || isset($limit[0]) === false)
			return(false);

		$r = dwho_uint($limit[0]);

		if(isset($limit[1]) === true)
			$r .= ', '.dwho_uint($limit[1]);

		return($r);
	}

	function select_singlecolumn($query='',$free=true)
	{
		$query = (string) $query;
		$free = (bool) $free;

		if((dwho_has_len($query) === false
		   && ($query = $this->get_select()) === null) === true
		|| $this->query($query,($free === false)) === false)
			return(false);

		$r = array();

		for($i = 0;$m = $this->fetch_row();$i++)
			$r[$i] = $m[0];

		if(array_key_exists(0,$r) === false)
			$r = false;

		if($this->_free === true && $free === true)
			$this->free();

		return($r);
	}

	function select_columnpair($query='',$free=true)
	{
		$query = (string) $query;
		$free = (bool) $free;

		if((dwho_has_len($query) === false
		   && ($query = $this->get_select()) === null) === true
		|| $this->query($query,($free === false)) === false
		|| ($m = $this->fetch_row()) === false
		|| array_key_exists(0,$m) === false
		|| array_key_exists(1,$m) === false)
			return(false);

		$r = array();
		$r[$m[0]] = $m[1];

		while(($m = $this->fetch_row()) !== false)
			$r[$m[0]] = $m[1];

		if($this->_free === true && $free === true)
			$this->free();

		return($r);
	}

	function _query_dictinct_count_singlecolumn($fullcolumn)
	{
		if(($from = $this->get_from()) === null)
			return(false);

		$query = 'SELECT COUNT(DISTINCT('.$fullcolumn.')) FROM '.$from;

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

		return($query);
	}

	function select_distinct_count_singlecolumn($query='',$column='',$table='',$free=true)
	{
		$query = (string) $query;
		$free = (bool) $free;

		if(dwho_has_len($query) === false)
		{
			$tabledot = '';

			$colobj = is_object($column);

			if(($column = $this->_get_scalar($column,true)) === '' || $column === '*')
				return(false);

			if(($table = trim($table)) !== '')
			{
				if(isset($this->_table[$table]) === false)
					return(false);

				if($colobj === false)
					$tabledot = $this->quote_identifier($table).'.';
			}

			if(($query = $this->_query_dictinct_count_singlecolumn($tabledot.$column)) === false)
				return(false);
		}

		if($this->query($query,($free === false)) === false)
			return(false);
		else if(($r = $this->fetch_row()) !== false)
			$r = dwho_ulongint($r[0]);

		if($this->_free === true && $free === true)
			$this->free();

		return($r);
	}

	function select_count($query='',$column='',$table='',$free=true)
	{
		$query = (string) $query;
		$free = (bool) $free;

		if(dwho_has_len($query) === false)
		{
			$tabledot = '';

			$colobj = is_object($column);

			if(($column = $this->_get_scalar($column,true)) === '')
				$column = '*';

			if(($table = trim($table)) !== '')
			{
				if(isset($this->_table[$table]) === false)
					return(false);

				if($colobj === false || $column === '*')
					$tabledot = $this->quote_identifier($table).'.';
			}

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

		if($this->query($query,($free === false)) === false)
			return(false);
		else if(($r = $this->fetch_row()) !== false)
			$r = dwho_ulongint($r[0]);

		if($this->_free === true && $free === true)
			$this->free();

		return($r);
	}

	function select_one($query='',$free=true)
	{
		$query = (string) $query;
		$free = (bool) $free;

		if(dwho_has_len($query) === false)
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

		if($this->query($query,($free === false)) === false)
			return(false);
		else if(($r = $this->fetch_row()) !== false)
			$r = $r[0];

		if($this->_free === true && $free === true)
			$this->free();

		return($r);
	}

	function select_single($query='',$free=true)
	{
		$query = (string) $query;
		$free = (bool) $free;

		if(dwho_has_len($query) === false)
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

		if($this->query($query,($free === false)) === false)
			return(false);

		$r = $this->fetch_assoc();

		if($this->_free === true && $free === true)
			$this->free();

		return($r);
	}

	function select_all($query='',$free=true,$full_column_names=false,$assoc_column_value=null)
	{
		$query = (string) $query;
		$free = (bool) $free;

		if(dwho_has_len($query) === false
		&& ($query = $this->get_select()) === null)
			return(false);

		if((bool) $full_column_names === true)
			$r = $this->_fetch_assoc_full_column_names($query,$free,$assoc_column_value);
		else
		{
			$r = array();

			if((dwho_has_len($query) === false
			   && ($query = $this->get_select()) === null) === true
			|| $this->query($query,($free === false)) === false)
				return(false);

			if(dwho_has_len($assoc_column_value) === false)
			{
				for($i = 0;$m = $this->fetch_assoc();$i++)
					$r[$i] = $m;
			}
			else
			{
				for($i = 0;$m = $this->fetch_assoc();$i++)
				{
					if(isset($m[$assoc_column_value]) === true)
						$r[$m[$assoc_column_value]] = $m;
				}
			}
		}

		if(is_array($r) === false || empty($r) === true)
			$r = false;

		if($this->_free === true && $free === true)
			$this->free();

		return($r);
	}
}

?>
