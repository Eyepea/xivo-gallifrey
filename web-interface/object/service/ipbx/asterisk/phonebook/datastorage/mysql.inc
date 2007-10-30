<?php

xivo_service_asterisk::required(array('phonebook','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_phonebook_mysql extends xivo_service_asterisk_phonebook_abstract_sql
{
	function xivo_service_asterisk_phonebook_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
