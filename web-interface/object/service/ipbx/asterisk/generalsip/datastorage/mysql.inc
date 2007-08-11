<?php

xivo_service_asterisk::required(array('staticconf','datastorage','mysql.inc'),true);

class xivo_service_asterisk_generalsip_mysql extends xivo_service_asterisk_staticconf_mysql
{
	var $_param	= array('table' => 'generalsip');

	function xivo_service_asterisk_generalsip_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
