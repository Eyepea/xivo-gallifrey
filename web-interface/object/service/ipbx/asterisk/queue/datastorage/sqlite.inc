<?php

xivo_service_asterisk::required(array('queue','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_queue_sqlite extends xivo_service_asterisk_queue_abstract_sql
{
	function xivo_service_asterisk_queue_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
