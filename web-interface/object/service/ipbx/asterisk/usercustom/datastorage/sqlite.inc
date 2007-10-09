<?php

xivo_service_asterisk::required(array('usercustom','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_usercustom_sqlite extends xivo_service_asterisk_usercustom_abstract_sql
{
	function xivo_service_asterisk_usercustom_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
