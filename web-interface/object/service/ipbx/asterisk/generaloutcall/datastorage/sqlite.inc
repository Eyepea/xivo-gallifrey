<?php

xivo_service_asterisk::required(array('generaloutcall','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_generaloutcall_sqlite extends xivo_service_asterisk_generaloutcall_abstract_sql
{
	function xivo_service_asterisk_generaloutcall_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
