<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_incall_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'incall');

	function get_search($exten,$disable=null,$order=null,$limit=null)
	{
		if(($where = $this->_dso->search_contain('exten',$exten)) === false)
			return(false);

		if($disable !== null)
			$where .= ' AND commented = '.intval((bool) $disable);

		$this->_dso->new_order($order);
		$this->_dso->new_limit($limit);

		$r = $this->_dso->select_all($this->_param['table'],null,$where);

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count($this->_param['table'],null,$where)) === false
		|| empty($r) === true)
			$r = false;

		return($r);
	}
}

?>
