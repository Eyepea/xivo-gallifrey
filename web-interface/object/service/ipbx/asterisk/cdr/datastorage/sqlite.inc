<?php

xivo_service_asterisk::required(array('cdr','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_cdr_sqlite extends xivo_service_asterisk_cdr_abstract_sql
{
	function xivo_service_asterisk_cdr_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
