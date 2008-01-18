<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'datastorage.inc');

class xivo_abstract_datastorage_sql extends xivo_abstract_datastorage
{
	var $_cnt		= 0;
	var $_autoincrement	= true;

	function _init(&$dso,$param=array())
	{
		parent::_init(&$dso);

		$param = (array) $param;

		if(isset($param['table']) === true)
			$this->_table = (string) $param['table'];

		if(isset($this->_table) === false || xivo_haslen($this->_table) === false)
			trigger_error('Missing or invalid table',E_USER_ERROR);

		if(isset($param['primary']) === true)
			$this->_primary = $this->_mk_primary_key($param['primary']);
		else if(isset($this->_primary) === true)
			$this->_primary = $this->_mk_primary_key($this->_primary);

		if(isset($param['autoincrement']) === true)
			$this->_autoincrement = (bool) $param['autoincrement'];
		else if(isset($this->_autoincrement) === true)
			$this->_autoincrement = (bool) $this->_autoincrement;

		if(xivo_haslen($param,'disable') === true)
			$this->_disable = (string) $param['disable'];
		else if(isset($this->_disable) === false || xivo_haslen($this->_disable) === false)
			$this->_disable = null;

		if(isset($param['forcecolumn']) === true)
			$this->_forcecolumn = $this->_mk_forcecolumn($param['forcecolumn']);
		else if(isset($this->_forcecolumn) === true)
			$this->_forcecolumn = $this->_mk_forcecolumn($this->_forcecolumn);
	}

	function _mk_primary_key($primary)
	{
		if(xivo_haslen($primary) === true)
			return(array(strval($primary) => ''));
		else if(($arr = xivo_get_aks($primary)) === false)
			return(null);

		$r = array();
		$noassoc = in_array(0,$arr['keys'],true);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$primary[$key];

			if($noassoc === true && is_int($key) === true)
			{
				if(xivo_haslen($val) === true)
					$r[strval($val)] = '';
			}
			else if(xivo_haslen($key) === true)
				$r[strval($key)] = $val;
		}

		if(empty($r) === false)
			return($r);

		return(null);
	}

	function get_primary_key($val)
	{
		if(isset($this->_primary) === false
		|| is_scalar($val) === false
		|| is_array($this->_primary) === false
		|| count($this->_primary) !== 1)
			return(false);

		return(array(key($this->_primary) => $val));
	}

	function _mk_forcecolumn($column)
	{
		if(($arr = xivo_get_aks($column)) === false)
			return(null);

		$r = array();
		$noassoc = in_array(0,$arr['keys'],true);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$column[$key];

			if($noassoc === true && is_int($key) === true)
				continue;
			else if(xivo_haslen($key) === true)
				$r[strval($key)] = $val;
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
		&& ($arr = $this->get_primary_key($arr)) === false)
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

		if(($r = $this->_dso->select_single()) === false || $primary === false)
			return($r);

		if(($nb = count($column)) > 1)
		{
			for($i = 0;$i < $nb;$i++)
				settype($r[$column[$i]],gettype($this->_primary[$column[$i]]));
		}
		else
		{
			$r = $r[$column[0]];
			settype($r,gettype($this->_primary[$column[0]]));
		}

		return($r);
	}

	function get_id($arr,$disable=null)
	{
		return($this->get($arr,$disable,true));
	}

	function get_cnt()
	{
		return(xivo_uint($this->_cnt));
	}

	function get_nb($arr=null,$disable=null)
	{
		if($arr !== null
		&& (is_array($arr) === false || empty($arr) === true) === true)
			return(false);
	
		if(isset($this->_forcecolumn) === true)
			$arr = array_merge($arr,$this->_forcecolumn);

		if($disable !== null && isset($this->_disable) === true)
			$arr[$this->_disable] = intval((bool) $disable);

		$this->_dso->new_select($this->_table);

		if(is_array($arr) === true)
			$this->_dso->where($arr);

		return((int) $this->_dso->select_count());
	}

	function add($arr,$insertid=true)
	{
		if(is_array($arr) === false)
			return(false);
	
		if(isset($this->_forcecolumn) === true)
			$arr = array_merge($arr,$this->_forcecolumn);

		if(isset($this->_disable) === true
		&& array_key_exists($this->_disable,$arr) === true)
			$arr[$this->_disable] = intval((bool) $arr[$this->_disable]);

		if($this->_dso->insert($this->_table,$arr) === false)
			return(false);
		else if((bool) $insertid === false
		|| $this->_autoincrement === false
		|| isset($this->_primary) === false
		|| count($this->_primary) > 1
		|| is_int(current($this->_primary)) === false)
			return(true);

		return($this->_dso->insert_id());
	}

	function edit($arr,$value,$disable=null)
	{
		if(is_array($value) === false
		|| (is_array($arr) === false && ($arr = $this->get_primary_key($arr)) === false) === true)
			return(false);
	
		if(isset($this->_forcecolumn) === true)
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
		&& ($arr = $this->get_primary_key($arr)) === false)
			return(false);
	
		if(isset($this->_forcecolumn) === true)
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
			$column = '*';

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

		if((bool) $all === false && isset($this->_primary) === true)
			$column = array_keys($this->_primary);
		else
			$column = '*';

		$this->_dso->new_select($this->_table,$column);
	
		if(isset($this->_forcecolumn) === true)
			$arr = array_merge($arr,$this->_forcecolumn);

		if($disable !== null && isset($this->_disable) === true)
			$arr[$this->_disable] = intval((bool) $disable);

		$this->_dso->where($arr);

		$this->_dso->new_order($order);
		$this->_dso->limit($limit);

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
