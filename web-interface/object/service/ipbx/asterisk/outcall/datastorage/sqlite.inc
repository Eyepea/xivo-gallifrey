<?php

xivo_service_asterisk::required(array('outcall','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_outcall_sqlite extends xivo_service_asterisk_outcall_abstract_sql
{
	function xivo_service_asterisk_outcall_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
