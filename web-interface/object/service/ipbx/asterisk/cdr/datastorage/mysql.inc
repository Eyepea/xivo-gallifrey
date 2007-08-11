<?php

xivo_service_asterisk::required(array('cdr','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_cdr_mysql extends xivo_service_asterisk_cdr_abstract_sql
{
	function xivo_service_asterisk_cdr_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
