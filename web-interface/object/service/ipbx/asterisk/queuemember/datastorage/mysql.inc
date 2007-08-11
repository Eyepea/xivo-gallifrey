<?php

xivo_service_asterisk::required(array('queuemember','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_queuemember_mysql extends xivo_service_asterisk_queuemember_abstract_sql
{
	function xivo_service_asterisk_queuemember_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
