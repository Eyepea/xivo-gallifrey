<?php

xivo_service_asterisk::required(array('useriax','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_useriax_sqlite extends xivo_service_asterisk_useriax_abstract_sql
{
	function xivo_service_asterisk_useriax_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
