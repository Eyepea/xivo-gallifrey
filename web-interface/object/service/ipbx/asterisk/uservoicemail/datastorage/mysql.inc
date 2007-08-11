<?php

xivo_service_asterisk::required(array('uservoicemail','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_uservoicemail_mysql extends xivo_service_asterisk_uservoicemail_abstract_sql
{
	function xivo_service_asterisk_uservoicemail_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
