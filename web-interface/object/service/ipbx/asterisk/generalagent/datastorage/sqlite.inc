<?php

xivo_service_asterisk::required(array('staticconf','datastorage','sqlite.inc'),true);

class xivo_service_asterisk_generalagent_sqlite extends xivo_service_asterisk_staticconf_sqlite
{
	var $_param	= array('table' => 'agent');

	function xivo_service_asterisk_generalagent_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
