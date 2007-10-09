<?php

xivo_service_asterisk::required(array('usercustom','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_usercustom_mysql extends xivo_service_asterisk_usercustom_abstract_sql
{
	function xivo_service_asterisk_usercustom_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
