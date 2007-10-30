<?php

xivo_service_asterisk::required(array('phonebooknumber','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_phonebooknumber_mysql extends xivo_service_asterisk_phonebooknumber_abstract_sql
{
	function xivo_service_asterisk_phonebooknumber_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
