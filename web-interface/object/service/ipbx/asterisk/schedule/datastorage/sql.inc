<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_schedule_sql extends xivo_service_asterisk_abstract_sql
{
	var $_unique	= 'name';

	function get_all_except($id,$disable=null,$order,$limit)
	{
		$this->_dso->new_select($this->_table,'*');

		$arr = array();
	
		if(isset($this->_forcecolumn) === true)
			$arr = $this->_forcecolumn;

		if($disable !== null && isset($this->_disable) === true)
			$arr[$this->_disable] = intval((bool) $disable);

		if(empty($arr) === false)
			$this->_dso->where($arr);

		$this->_dso->andwherenot(array('id' => $id));

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
