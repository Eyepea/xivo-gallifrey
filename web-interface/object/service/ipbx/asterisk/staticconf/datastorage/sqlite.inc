<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','staticconf','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_staticconf_sqlite extends xivo_service_asterisk_staticconf_abstract_sql
{
	function _init(&$dso,$param=array())
	{
		parent::_init(&$dso,$param);
	}
}

?>
