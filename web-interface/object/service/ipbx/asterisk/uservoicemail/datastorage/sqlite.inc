<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','uservoicemail','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_uservoicemail_sqlite extends xivo_service_asterisk_uservoicemail_abstract_sql
{
	function xivo_service_asterisk_uservoicemail_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
