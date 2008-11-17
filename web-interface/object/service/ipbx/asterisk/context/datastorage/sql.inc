<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_context_sql extends xivo_service_asterisk_abstract_sql
{
	var $_autoincrement	= false;
	var $_unique		= 'name';

	function get_all_except($name,$order=null,$limit=null)
	{
		$this->_dso->new_select($this->_table,'*');

		if(isset($this->_forcecolumn) === true)
			$this->_dso->where($this->_forcecolumn);

		if(xivo_haslen($name) === true)
			$this->_dso->andwherenot(array('name' => $name));

		$this->_dso->new_order($order);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all();

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
