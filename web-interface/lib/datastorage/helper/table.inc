<?php

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

		if(isset($this->_table) === false || xivo_haslen($this->_table) === false)
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

		if(xivo_haslen($param,'disable') === true)
			$this->_disable = (string) $param['disable'];
		else if(isset($this->_disable) === false || xivo_haslen($this->_disable) === false)
			$this->_disable = null;

		if(isset($param['forcecolumn']) === true)
			$this->_forcecolumn = $this->_mk_forcecolumn($param['forcecolumn']);
		else if(isset($this->_forcecolumn) === true)
			$this->_forcecolumn = $this->_mk_forcecolumn($this->_forcecolumn);
	}
	
	function _mk_key($list,$next=false)
	{
		if(xivo_haslen($list) === true)
			return(array(strval($list) => ''));
		else if(($arr = xivo_get_aks($list)) === false)
			return(null);

		$r = array();
		$noassoc = in_array(0,$arr['keys'],true);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$list[$key];

			if(is_array($val) === true)
			{
				if((bool) $next === true
				&& ($rs = $this->_mk_key($val)) !== null)
					$r[] = $rs;
			}
			else if($noassoc === true && is_int($key) === true)
			{
				if(xivo_haslen($val) === true)
					$r[strval($val)] = '';
			}
			else if(xivo_haslen($key) === true && is_scalar($val) === true)
				$r[strval($key)] = $val;
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

		if(($r = $this->_dso->select_single()) === false || $primary === false)
			return($r);
		else if(($nb = count($column)) > 1)
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
		else if(isset($this->_forcecolumn) === true)
			$arr = array_merge($arr,$this->_forcecolumn);

		if($disable !== null && isset($this->_disable) === true)
			$arr[$this->_disable] = intval((bool) $disable);

		$this->_dso->new_select($this->_table);

		if(is_array($arr) === true)
			$this->_dso->where($arr);

		return((int) $this->_dso->select_count());
	}

	function chk_unique($arr,$primary=null)
	{
		if(isset($this->_unique) === false 
		|| ($uniq = xivo_get_aks($this->_unique)) === false)
			return(null);

		$this->_dso->new_select($this->_table);

		for($i = 0;$i < $uniq['cnt'];$i++)
		{
			$key = &$uniq['keys'][$i];
			$val = &$this->_unique[$key];

			if(is_array($val) === true)
			{
				$where = array();
				$keys = array_keys($val);
				$nb = count($keys);

				for($j = $k = 0;$j < $nb;$j++)
				{
					$key2 = &$keys[$j];

					if(isset($arr[$key2]) === true)
					{
						$where[$key2] = $arr[$key2];
						$k++;
					}
					else if($k > 0)
						return(false);
				}

				if(empty($where) === true)
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

		if(isset($this->_forcecolumn) === true)
		{
			$this->_dso->aggregate_where();
			$this->_dso->andwhere($this->_forcecolumn);
		}

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
		else if((bool) $insertid === false
		|| $this->_autoincrement === false
		|| $this->is_single_primary() === false
		|| is_int(current($this->_primary)) === false)
			return(true);

		return($this->_dso->insert_id());
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
