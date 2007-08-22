<?php

xivo_service_asterisk::required(array('outcall','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_outcall_mysql extends xivo_service_asterisk_outcall_abstract_sql
{
	function xivo_service_asterisk_outcall_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
