<?php

xivo_service_asterisk::required(array('rightcallexten','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_rightcallexten_mysql extends xivo_service_asterisk_rightcallexten_abstract_sql
{
	function xivo_service_asterisk_rightcallexten_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
