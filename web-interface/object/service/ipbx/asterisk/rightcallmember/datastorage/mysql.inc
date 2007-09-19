<?php

xivo_service_asterisk::required(array('rightcallmember','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_rightcallmember_mysql extends xivo_service_asterisk_rightcallmember_abstract_sql
{
	function xivo_service_asterisk_rightcallmember_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
