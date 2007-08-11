<?php

xivo_service_asterisk::required(array('trunksip','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_trunksip_sqlite extends xivo_service_asterisk_trunksip_abstract_sql
{
	function xivo_service_asterisk_trunksip_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
