<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_cdr_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'cdr');

	function search($arr,$order=null,$limit=null)
	{
		if(($where = $this->_search_where($arr)) === false)
			return(false);

		$this->_dso->order($order);
		$this->_dso->limit($limit);

		$select = '*, '.$this->_dso->unix_timestamp('calldate','localtime').' AS callunixtime';

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

			if(($rs = $this->_dso->cond_date('calldate',$dbeg,$dend,$format,'localtime')) !== false)
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
}

?>
