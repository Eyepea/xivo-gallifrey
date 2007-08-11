<?php

xivo_service_asterisk::required(array('trunkiax','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_trunkiax_mysql extends xivo_service_asterisk_trunkiax_abstract_sql
{
	function xivo_service_asterisk_trunkiax_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
