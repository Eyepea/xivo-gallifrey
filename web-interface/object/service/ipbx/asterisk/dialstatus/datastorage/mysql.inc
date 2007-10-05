<?php

xivo_service_asterisk::required(array('dialstatus','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_dialstatus_mysql extends xivo_service_asterisk_dialstatus_abstract_sql
{
	function xivo_service_asterisk_dialstatus_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
