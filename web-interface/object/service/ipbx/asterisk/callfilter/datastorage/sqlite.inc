<?php

xivo_service_asterisk::required(array('callfilter','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_callfilter_sqlite extends xivo_service_asterisk_callfilter_abstract_sql
{
	function xivo_service_asterisk_callfilter_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
