<?php

xivo_service_asterisk::required(array('incall','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_incall_mysql extends xivo_service_asterisk_incall_abstract_sql
{
	function xivo_service_asterisk_incall_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
