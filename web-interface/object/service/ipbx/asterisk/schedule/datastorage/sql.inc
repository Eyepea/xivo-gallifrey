<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_schedule_sql extends xivo_service_asterisk_abstract_sql
{
	var $_unique	= 'name';

	function get_all_except($id,$order=null,$limit=null)
	{
		$this->_dso->new_select($this->_table,'*');

		if(isset($this->_forcecolumn) === true)
			$this->_dso->where($this->_forcecolumn);

		if(($id = xivo_ulongint($id)) !== 0)
			$this->_dso->andwherenot(array('id' => $id));

		$this->_dso->new_order($order);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all();

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		return($r);
	}
}

?>
