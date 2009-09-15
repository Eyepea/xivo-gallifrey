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

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'helper'.XIVO_SEP_DIR.'abstract.inc');

class xivo_datastorage_helper_table extends xivo_datastorage_helper_abstract
{
	var $_cnt		= 0;
	var $_primary		= null;
	var $_unique		= null;
	var $_autoincrement	= true;
	var $_disable		= null;
	var $_forcecolumn	= null;

	function _init(&$dso,$param=array())
	{
		parent::_init(&$dso);

		$param = (array) $param;

		if(isset($param['table']) === true)
			$this->_table = (string) $param['table'];

		if(isset($this->_table) === false || xivo_has_len($this->_table) === false)
			trigger_error('Missing or invalid table',E_USER_ERROR);

		if(isset($param['primary']) === true)
			$this->_primary = $this->_mk_key($param['primary']);
		else if(isset($this->_primary) === true)
			$this->_primary = $this->_mk_key($this->_primary);

		if(isset($param['unique']) === true)
			$this->_unique = $this->_mk_key($param['unique'],true);
		else if(isset($this->_unique) === true)
			$this->_unique = $this->_mk_key($this->_unique,true);

		if(isset($param['autoincrement']) === true)
			$this->_autoincrement = (bool) $param['autoincrement'];
		else if(isset($this->_autoincrement) === true)
			$this->_autoincrement = (bool) $this->_autoincrement;

		if(xivo_has_len($param,'disable') === true)
			$this->_disable = (string) $param['disable'];
		else if(isset($this->_disable) === false || xivo_has_len($this->_disable) === false)
			$this->_disable = null;

		if(isset($param['forcecolumn']) === true)
			$this->_forcecolumn = $this->_mk_forcecolumn($param['forcecolumn']);
		else if(isset($this->_forcecolumn) === true)
			$this->_forcecolumn = $this->_mk_forcecolumn($this->_forcecolumn);
	}

	function _mk_key($list,$next=false)
	{
		if(xivo_has_len($list) === true)
			return(array(strval($list) => ''));
		else if(is_array($list) === false || empty($list) === true)
			return(null);

		$keys = array_keys($list);
		$noassoc = in_array(0,$keys,true);

		$nb = count($keys);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$key = &$keys[$i];
			$val = &$list[$key];

			if(is_array($val) === true)
			{
				if((bool) $next === true
				&& ($rs = $this->_mk_key($val)) !== null)
					$r[] = $rs;
			}
			else if($noassoc === true && is_int($key) === true)
			{
				if(xivo_has_len($val) === true)
					$r[strval($val)] = '';
			}
			else if(xivo_has_len($key) === true)
			{
				if(is_null($val) === true || is_bool($val) === true)
					$r[strval($key)] = '';
				else if(is_scalar($val) === true)
					$r[strval($key)] = $val;
			}
		}

		if(empty($r) === false)
			return($r);

		return(null);
	}

	function get_primary_key()
	{
		if(isset($this->_primary) === false
		|| is_array($this->_primary) === false)
			return(false);

		return($this->_primary);
	}

	function get_disable()
	{
		if(isset($this->_disable) === true)
			return($this->_disable);

		return(false);
	}

	function mk_primary_key_value($val)
	{
		if(isset($this->_primary) === false
		|| is_scalar($val) === false
		|| is_array($this->_primary) === false
		|| count($this->_primary) !== 1)
			return(false);

		return(array(key($this->_primary) => $val));
	}

	function has_primary()
	{
		if(isset($this->_primary) === false
		|| is_array($this->_primary) === false)
			return(false);

		return(true);
	}

	function is_single_primary()
	{
		if(isset($this->_primary) === false
		|| is_array($this->_primary) === false
		|| count($this->_primary) !== 1)
			return(false);

		return(true);
	}

	function is_multi_primary()
	{
		if(isset($this->_primary) === false
		|| is_array($this->_primary) === false
		|| count($this->_primary) < 2)
			return(false);

		return(true);
	}

	function has_autoincrement()
	{
		if(isset($this->_autoincrement) === false
		|| (bool) $this->_autoincrement === false)
			return(false);

		return(true);
	}

	function _mk_forcecolumn($column)
	{
		if(is_array($column) === false || empty($column) === true)
			return(null);

		$keys = array_keys($column);
		$noassoc = in_array(0,$keys,true);

		$nb = count($keys);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$key = &$keys[$i];

			if(($noassoc === true && is_int($key) === true) === false
			&& xivo_has_len($key) === true)
				$r[strval($key)] = $column[$key];
		}

		if(empty($r) === false)
			return($r);

		return(null);
	}

	function get($arr,$disable=null,$primary=false)
	{
		$primary = (bool) $primary;

		$column = '*';

		if(is_array($arr) === false
		&& ($arr = $this->mk_primary_key_value($arr)) === false)
			return(false);
		else if($primary === true)
		{
			if(isset($this->_primary) === true)
				$column = array_keys($this->_primary);
			else
				return(false);
		}

		$this->_dso->new_select($this->_table,$column);

		if(isset($this->_forcecolumn) === true)
			$arr = array_merge($arr,$this->_forcecolumn);

		if($disable !== null && isset($this->_disable) === true)
			$arr[$this->_disable] = intval((bool) $disable);

		$this->_dso->where($arr);

		if($primary === true && $this->is_single_primary() === true)
			return($this->_dso->select_one());

		return($this->_dso->select_single());
	}

	function get_primary($arr,$disable=null)
	{
		return($this->get($arr,$disable,true));
	}

	function get_cnt()
	{
		return(xivo_ulongint($this->_cnt));
	}

	function get_nb($arr=null,$disable=null)
	{
		if($arr !== null
		&& (is_array($arr) === false || empty($arr) === true) === true)
			return(false);
		else if(isset($this->_forcecolumn) === true)
			$arr = array_merge($arr,$this->_forcecolumn);

		if($disable !== null && isset($this->_disable) === true)
			$arr[$this->_disable] = intval((bool) $disable);

		$this->_dso->new_select($this->_table);

		if(is_array($arr) === true)
			$this->_dso->where($arr);

		return(xivo_ulongint($this->_dso->select_count()));
	}

	function chk_unique($arr,$primary=null)
	{
		if(isset($this->_unique) === false
		|| is_array($this->_unique) === false
		|| empty($this->_unique) === true)
			return(null);

		$this->_dso->new_select($this->_table);

		foreach($this->_unique as $key => $val)
		{
			if(is_array($val) === true)
			{
				$where = array();

				$i = 0;

				reset($val);

				while(list($key2) = each($val))
				{
					if(isset($arr[$key2]) === true)
					{
						$where[$key2] = $arr[$key2];
						$i++;
					}
					else if($i > 0)
						return(false);
				}

				if($i === 0)
					continue;

				$this->_dso->orwhere($where);
				$this->_dso->aggregate_where();
			}
			else if(isset($arr[$key]) === true)
			{
				$this->_dso->orwhere(array($key => $arr[$key]));
				$this->_dso->aggregate_where();
			}
		}

		$this->_dso->aggregate_where_all();

		if($this->_dso->get_where() === null)
			return(true);
		else if(is_array($primary) === true)
			$this->_dso->andwherenot($primary,null,false);

		return(($this->_dso->select_single() === false));
	}

	function add($arr,$insertid=true,$unique=true)
	{
		if(is_array($arr) === false
		|| ((bool) $unique === true && $this->chk_unique($arr) === false) === true)
			return(false);
		else if(isset($this->_forcecolumn) === true)
			$arr = array_merge($arr,$this->_forcecolumn);

		if(isset($this->_disable) === true
		&& array_key_exists($this->_disable,$arr) === true)
			$arr[$this->_disable] = intval((bool) $arr[$this->_disable]);

		if($this->_dso->insert($this->_table,$arr) === false)
			return(false);
		else if((bool) $insertid === true
		&& $this->_autoincrement === true
		&& $this->is_single_primary() === true)
			return($this->_dso->insert_id());

		return(true);
	}

	function edit($arr,$value,$disable=null,$unique=true)
	{
		if(is_array($value) === false
		|| (is_array($arr) === false && ($arr = $this->mk_primary_key_value($arr)) === false) === true
		|| ((bool) $unique === true && $this->chk_unique($value,$arr) === false) === true)
			return(false);
		else if(isset($this->_forcecolumn) === true)
		{
			$arr = array_merge($arr,$this->_forcecolumn);
			$value = array_merge($value,$this->_forcecolumn);
		}

		if(isset($this->_disable) === true)
		{
			if($disable !== null)
				$arr[$this->_disable] = intval((bool) $disable);

			if(array_key_exists($this->_disable,$value) === true)
				$value[$this->_disable] = intval((bool) $value[$this->_disable]);
		}

		return($this->_dso->update($this->_table,$value,$arr));
	}

	function delete($arr,$disable=null)
	{
		if(is_array($arr) === false
		&& ($arr = $this->mk_primary_key_value($arr)) === false)
			return(false);
		else if(isset($this->_forcecolumn) === true)
			$arr = array_merge($arr,$this->_forcecolumn);

		if($disable !== null && isset($this->_disable) === true)
			$arr[$this->_disable] = intval((bool) $disable);

		return($this->_dso->delete($this->_table,$arr));
	}

	function get_list($disable=null,$order=null,$limit=null,$all=false)
	{
		if((bool) $all === false && isset($this->_primary) === true)
			$column = array_keys($this->_primary);
		else
		{
			$all = true;
			$column = '*';
		}

		$this->_dso->new_select($this->_table,$column);

		$arr = array();

		if(isset($this->_forcecolumn) === true)
			$arr = $this->_forcecolumn;

		if($disable !== null && isset($this->_disable) === true)
			$arr[$this->_disable] = intval((bool) $disable);

		if(empty($arr) === false)
			$this->_dso->where($arr);

		$this->_dso->new_order($order);
		$this->_dso->limit($limit);

		if($all === false && $this->is_single_primary() === true)
			$r = $this->_dso->select_singlecolumn();
		else
			$r = $this->_dso->select_all();

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count()) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_list_where($arr,$disable=null,$order=null,$limit=null,$all=false)
	{
		$this->_cnt = 0;

		if(is_array($arr) === false || empty($arr) === true)
			return(false);
		else if((bool) $all === false && isset($this->_primary) === true)
			$column = array_keys($this->_primary);
		else
		{
			$all = true;
			$column = '*';
		}

		$this->_dso->new_select($this->_table,$column);

		if(isset($this->_forcecolumn) === true)
			$arr = array_merge($arr,$this->_forcecolumn);

		if($disable !== null && isset($this->_disable) === true)
			$arr[$this->_disable] = intval((bool) $disable);

		$this->_dso->where($arr);

		$this->_dso->new_order($order);
		$this->_dso->limit($limit);

		if($all === false && $this->is_single_primary() === true)
			$r = $this->_dso->select_singlecolumn();
		else
			$r = $this->_dso->select_all();

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count()) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all($disable=null,$order=null,$limit=null)
	{
		return($this->get_list($disable,$order,$limit,true));
	}

	function get_all_where($arr,$disable=null,$order=null,$limit=null)
	{
		return($this->get_list_where($arr,$disable,$order,$limit,true));
	}
}

?>
