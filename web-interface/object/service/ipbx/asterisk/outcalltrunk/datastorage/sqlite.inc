<?php

xivo_service_asterisk::required(array('outcalltrunk','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_outcalltrunk_sqlite extends xivo_service_asterisk_outcalltrunk_abstract_sql
{
	function xivo_service_asterisk_outcalltrunk_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
