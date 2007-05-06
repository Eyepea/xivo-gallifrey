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

		if(($this->_errno = $this->errno()) === false)
			$this->_errno = 'unknown';

		if(($this->_error = $this->error()) === false)
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

		$q = sprintf('SELECT %s FROM %s WHERE %s%s',$s,$t,($w !== '' ? $w : 1),$o);

		if($this->query($q) === false)
			return(false);

		$r = array();

		for($i = 0;$m = $this->fetch_assoc();$i++)
		{
			if($m === false)
				break;

			$r[$i] = $m;
		}

		if($this->_free === true)
			$this->free();

		return($r);
	}

	function select_one($t,$s='',$w='',$o='')
	{
		$t = trim((string) $t);
		$s = trim((string) $s);
		$w = trim((string) $w);
		$o = trim((string) $o);

		if($s === '')
			$s = '*';

		if($o === '')
			$o = ' ';
		else
			$o = ' '.$o.' ';

		$r = false;
		$q = sprintf('SELECT %s FROM %s WHERE %s%s%s',$s,$t,($w !== '' ? $w : 1),$o,'LIMIT 1');

		if($this->query($q) === false)
			return($r);

		$r = $this->fetch_assoc();

		if($this->_free === true)
			$this->free();

		return($r);
	}

	function insert($t,&$data,$escape=true)
	{
		return($this->_insertupdate('INSERT INTO',$t,$data,null,$escape));
	}

	function update($t,&$data,$w='',$escape=true,$o='')
	{
		return($this->_insertupdate('UPDATE',$t,$data,$w,$escape,true,$o));
	}

	function replace($t,&$data,$escape=true)
	{
		return($this->_insertupdate('REPLACE INTO',$t,$data,null,$escape));
	}

	function delete($t,$w='',$o='')
	{
		$t = trim((string) $t);
		$w = trim((string) $w);
		$o = trim((string) $o);

		if($o !== '')
			$o = ' '.$o;
	
		$q = sprintf('DELETE FROM %s WHERE %s%s',$t,($w !== '' ? $w : 1),$o);

		return(($this->query($q) !== false ? true : false));
	}

	function where($data,$escape=true)
	{
		$r = false;
	
		if(($where = $this->_separator($data,$sep=' AND ',$escape,true,false)) !== false)
		{
			$r = $where['str'];

			if(trim($r) === '')
				$r = false;
		}

		return($r);
	}

	function where_not($data,$escape=true)
	{
		$r = false;

		if(($where = $this->_separator($data,$sep=' AND ',$escape,true,false,'!=')) !== false)
		{
			$r = $where['str'];

			if(trim($r) === '')
				$r = false;
		}

		return($r);	
	}

	function _insertupdate($p,$t,&$data,$w='',$escape=true,$set=false,$o='')
	{
		$p = (string) $p;
		$t = (string) $t;
		$w = (string) $w;
		$set = (bool) $set;
		$o = (string) $o;

		$r = false;

		if(($values = $this->_separator($data,',',$escape,$set)) === false)
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

			if(is_array($val[$key]) === true && isset($val[$key]['type'],$val[$key]['data']) === true)
			{
				$type = (string) $val[$key]['type'];
				$data = &$val[$key]['data'];
			}
			else
			{
				$type = gettype($val[$key]);
				$data = &$val[$key];
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
}

?>
