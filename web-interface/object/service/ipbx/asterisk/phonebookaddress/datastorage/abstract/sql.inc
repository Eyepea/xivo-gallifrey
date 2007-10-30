<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_phonebookaddress_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'phonebookaddress');
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

	function exists_type($arr)
	{
		if(($where = $this->_dso->where($arr)) === false)
			return(null);

		if(($r = $this->_dso->select_one($this->_param['table'],'id',$where)) !== false)
			$r = (int) $r['id'];

		return($r);
	}

	function get_search($value,$except=null,$order=null,$limit=null)
	{
		$where = array();

		if(($rs = $this->_dso->search_contain('address1',$value)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_contain('address2',$value)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_contain('city',$value)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_contain('state',$value)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_contain('zipcode',$value)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_contain('country',$value)) !== false)
			$where[] = $rs;

		if(($where = implode(' OR ',$where)) === '')
			return(false);

		if(is_array($except) === true && isset($except[0]) === true)
		{
			if(($rs = $this->_dso->notin('phonebookid',$except)) === false)
				return(false);
			else
				$where = '('.$where.') AND '.$rs;
		}

		$where .= ' GROUP BY phonebookid';

		$this->_dso->order($order);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all($this->_param['table'],null,$where);

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
}

?>
