<?php

xivo_service_asterisk::required(array('rightcallmember','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_rightcallmember_sqlite extends xivo_service_asterisk_rightcallmember_abstract_sql
{
	function xivo_service_asterisk_rightcallmember_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
