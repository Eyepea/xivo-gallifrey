<?php

xivo_service_asterisk::required(array('outcalltrunk','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_outcalltrunk_mysql extends xivo_service_asterisk_outcalltrunk_abstract_sql
{
	function xivo_service_asterisk_outcalltrunk_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
