<?php

xivo_service_asterisk::required(array('schedule','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_schedule_mysql extends xivo_service_asterisk_schedule_abstract_sql
{
	function xivo_service_asterisk_schedule_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
