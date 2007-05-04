<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','staticconf','datastorage','sqlite.inc'),true);

class xivo_service_asterisk_musiconhold_sqlite extends xivo_service_asterisk_staticconf_sqlite
{
	var $_param	= array('table' => 'musiconhold');

	function xivo_service_asterisk_musiconhold_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
