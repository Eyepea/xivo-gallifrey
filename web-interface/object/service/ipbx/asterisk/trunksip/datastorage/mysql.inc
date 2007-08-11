<?php

xivo_service_asterisk::required(array('trunksip','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_trunksip_mysql extends xivo_service_asterisk_trunksip_abstract_sql
{
	function xivo_service_asterisk_trunksip_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
