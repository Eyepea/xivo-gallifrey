<?php

class xivo_service_asterisk_cdr_abstract_sql
{
	var $_param	= array('table' => 'cdr');
	var $_dso	= null;

	function _init(&$dso,$param=array())
	{
		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		$param = (array) $param;

		if(isset($param['table']) === true)
			$this->_param['table'] = (string) $param['table'];
	}

	function search($arr)
	{
		if(is_array($arr) === false)
			return(false);

		$where = '';

		if(isset($arr['dbeg']) === true)
		{
			$dbeg = $arr['dbeg'];
			$len = strlen($dbeg);

			if($len === 4)
				$format = '%Y';
			else if($len === 7)
				$format = '%Y-%m';
			else
				$format = '%Y-%m-%d';

			if(isset($arr['dend']) === true)
				$dend = $arr['dend'];
			else
				$dend = '';

			$where .= ' AND '.$this->_dso->cond_date('calldate',$dbeg,$dend,$format);
		}

		if(isset($arr['channel']) === true)
			$where .= ' AND '.$this->_dso->search_exact('channel',$arr['channel']);

		if(isset($arr['disposition']) === true)
			$where .= ' AND '.$this->_dso->search_exact('disposition',$arr['disposition']);

		if(xivo_issa('src',$arr) === true)
			$where .= ' AND '.$this->_dso->search('src',$arr['src']['pattern'],$arr['src']['type']);

		if(xivo_issa('clid',$arr) === true)
			$where .= ' AND '.$this->_dso->search('src',$arr['clid']['pattern'],$arr['clid']['type']);

		if(xivo_issa('accountcode',$arr) === true)
			$where .= ' AND '.$this->_dso->search('accountcode',$arr['accountcode']['pattern'],$arr['accountcode']['type']);

		if(xivo_issa('userfield',$arr) === true)
			$where .= ' AND '.$this->_dso->search('userfield',$arr['userfield']['pattern'],$arr['userfield']['type']);

		if($where === '')
			return(false);

		$r = $this->_dso->select_all($this->_param['table'],null,'1 '.$where,'LIMIT 10');

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$r = $this->_dso->select_one($this->_param['table'],null,'id = '.$id);

		return($r);
	}

	function add($arr)
	{
		$r = false;

		if($this->_dso->insert($this->_param['table'],$arr) === true)
			$r = $this->_dso->insert_id();

		return($r);
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$r = $this->_dso->update($this->_param['table'],$arr,'id = '.$id);

		return($r);
	}

	function delete($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$r = $this->_dso->delete($this->_param['table'],'id = '.$id);

		return($r);
	}

	function get_list()
	{
		$rs = $this->_dso->query('SELECT id FROM '.$this->_param['table']);

		if($rs === false)
			return(false);

		$r = array();

		while($rw = $this->_dso->fetch_assoc($rs))
			$r[] = $rw['id'];

		if($this->_dso->is_free() === true)
			$this->_dso->free($rs);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
