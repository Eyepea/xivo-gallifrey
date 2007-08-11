<?php

xivo_service_asterisk::required(array('generaloutcall','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_generaloutcall_mysql extends xivo_service_asterisk_generaloutcall_abstract_sql
{
	function xivo_service_asterisk_generaloutcall_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
