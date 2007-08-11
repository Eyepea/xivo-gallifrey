<?php

xivo_service_asterisk::required(array('staticconf','datastorage','mysql.inc'),true);

class xivo_service_asterisk_generalvoicemail_mysql extends xivo_service_asterisk_staticconf_mysql
{
	var $_param	= array('table' => 'generalvoicemail');

	function xivo_service_asterisk_generalvoicemail_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
