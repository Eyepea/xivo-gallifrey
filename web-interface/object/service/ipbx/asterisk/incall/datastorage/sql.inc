<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_incall_sql extends xivo_service_asterisk_abstract_sql
{
	function search($exten,$disable=null,$order=null,$limit=null)
	{
		$this->_dso->new_select($this->_table);
		$this->_dso->wherecontain('exten',$exten);

		if($disable !== null)
			$this->_dso->andwhere(array($this->_disable => intval((bool) $disable)));

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
}

?>
