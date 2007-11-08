<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_phonebook_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'phonebook');

	function get_search($value,$list=null,$order=null,$limit=null,$name=false)
	{
		$name = (bool) $name;

		$where = array();

		if(($rs = $this->_dso->search_contain('firstname',$value)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_contain('lastname',$value)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_contain('displayname',$value)) !== false)
			$where[] = $rs;

		if($name === false)
		{
			if(($rs = $this->_dso->search_contain('society',$value)) !== false)
				$where[] = $rs;

			if(($rs = $this->_dso->search_contain('email',$value)) !== false)
				$where[] = $rs;

			if(($rs = $this->_dso->search_contain('url',$value)) !== false)
				$where[] = $rs;

			if(($rs = $this->_dso->search_contain('description',$value)) !== false)
				$where[] = $rs;
		}

		if(is_array($list) === true && isset($list[0]) === true)
		{
			if(($rs = $this->_dso->in('id',$list)) === false)
				return(false);
			else
				$where[] = $rs;
		}

		if(($where = implode(' OR ',$where)) === '')
			return(false);

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
