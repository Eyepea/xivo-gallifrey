<?php

xivo_service_asterisk::required(array('usersip','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_usersip_sqlite extends xivo_service_asterisk_usersip_abstract_sql
{
	function xivo_service_asterisk_usersip_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
