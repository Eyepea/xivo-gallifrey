<?php

xivo_service_asterisk::required(array('phonebookaddress','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_phonebookaddress_mysql extends xivo_service_asterisk_phonebookaddress_abstract_sql
{
	function xivo_service_asterisk_phonebookaddress_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
