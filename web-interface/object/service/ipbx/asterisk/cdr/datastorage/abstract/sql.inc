<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_cdr_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_cnt	= 0;
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

	function get_cnt()
	{
		if(isset($this->_cnt) === false)
			$this->_cnt = 0;

		return(xivo_uint($this->_cnt));
	}

	function search($arr,$order=null,$limit=null)
	{
		if(($where = $this->_search_where($arr)) === false)
			return(false);

		$this->_dso->order($order);
		$this->_dso->limit($limit);

		$select = '*, '.$this->_dso->unix_timestamp('calldate').' AS callunixtime';

		$r = $this->_dso->select_all($this->_param['table'],$select,$where);

		$this->_dso->order();
		$this->_dso->limit();

		if(empty($r) === true)
			$r = false;
		else
			$this->_cnt = $this->_dso->select_count($this->_param['table'],null,$where);

		if($this->_cnt === false)
			$r = false;

		return($r);
	}

	function _search_where($arr)
	{
		if(is_array($arr) === false)
			return(false);

		$where = array();

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

			if(($rs = $this->_dso->cond_date('calldate',$dbeg,$dend,$format)) !== false)
				$where[] = $rs;
		}

		if(isset($arr['channel']) === true)
		{
			if(is_array($arr['channel']) === true)
				$rs = $this->_dso->search_not_begin('channel',$arr['channel']);
			else
				$rs = $this->_dso->search_begin('channel',$arr['channel']);

			if($rs !== false)
				$where[] = $rs;
		}

		if(isset($arr['disposition']) === true
		&& ($rs = $this->_dso->search_exact('disposition',$arr['disposition'])) !== false)
			$where[] = $rs;

		if(isset($arr['amaflags']) === true
		&& ($rs = $this->_dso->search_exact('amaflags',$arr['amaflags'])) !== false)
			$where[] = $rs;

		if(xivo_issa('src',$arr) === true
		&& ($rs = $this->_dso->search('src',$arr['src']['pattern'],$arr['src']['type'])) !== false)
			$where[] = $rs;

		if(xivo_issa('dst',$arr) === true
		&& ($rs = $this->_dso->search('dst',$arr['dst']['pattern'],$arr['dst']['type'])) !== false)
			$where[] = $rs;

		if(xivo_issa('clid',$arr) === true
		&& ($rs = $this->_dso->search('clid',$arr['clid']['pattern'],$arr['clid']['type'])) !== false)
			$where[] = $rs;

		if(xivo_issa('accountcode',$arr) === true
		&& ($rs = $this->_dso->search('accountcode',$arr['accountcode']['pattern'],$arr['accountcode']['type'])) !== false)
			$where[] = $rs;

		if(xivo_issa('userfield',$arr) === true
		&& ($rs = $this->_dso->search('userfield',$arr['userfield']['pattern'],$arr['userfield']['type'])) !== false)
			$where[] = $rs;

		if(isset($arr['dubeg']) === true)
		{
			if(isset($arr['duend']) === true)
				$rs = $this->_dso->between_value('duration',$arr['dubeg'],$arr['duend']);
			else
				$rs = $this->_dso->search_exact('duration',$arr['dubeg']);

			if($rs !== false)
				$where[] = $rs;
		}

		if(($r = implode(' AND ',$where)) === '')
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
