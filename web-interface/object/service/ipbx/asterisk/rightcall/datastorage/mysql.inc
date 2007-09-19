<?php

xivo_service_asterisk::required(array('rightcall','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_rightcall_mysql extends xivo_service_asterisk_rightcall_abstract_sql
{
	function xivo_service_asterisk_rightcall_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
