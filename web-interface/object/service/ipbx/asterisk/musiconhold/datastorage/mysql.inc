<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','staticconf','datastorage','mysql.inc'),true);

class xivo_service_asterisk_musiconhold_mysql extends xivo_service_asterisk_staticconf_mysql
{
	var $_param	= array('table' => 'musiconhold');

	function xivo_service_asterisk_musiconhold_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
