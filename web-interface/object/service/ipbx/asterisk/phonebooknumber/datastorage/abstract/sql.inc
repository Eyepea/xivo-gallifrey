<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_phonebooknumber_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'phonebooknumber');

	function get_search($value,$except=null,$order=null,$limit=null)
	{
		if(($where = $this->_dso->search_contain('number',$value)) === false)
			return(false);

		if(is_array($except) === true && isset($except[0]) === true)
		{
			if(($rs = $this->_dso->notin('phonebookid',$except)) === false)
				return(false);
			else
				$where = '('.$where.') AND '.$rs;
		}

		$this->_dso->order($order);
		$this->_dso->limit($limit);

		$where .= ' GROUP BY phonebookid';

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
