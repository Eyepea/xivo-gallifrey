<?php

xivo_service_asterisk::required(array('phonefunckey','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_phonefunckey_mysql extends xivo_service_asterisk_phonefunckey_abstract_sql
{
	function xivo_service_asterisk_phonefunckey_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
