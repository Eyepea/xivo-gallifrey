<?php

xivo_service_asterisk::required(array('staticconf','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_staticconf_mysql extends xivo_service_asterisk_staticconf_abstract_sql
{
	function _init(&$dso,$param=array())
	{
		parent::_init(&$dso,$param);
	}
}

?>
