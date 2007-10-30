<?php

xivo_service_asterisk::required(array('phonebooknumber','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_phonebooknumber_sqlite extends xivo_service_asterisk_phonebooknumber_abstract_sql
{
	function xivo_service_asterisk_phonebooknumber_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
