<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','agent','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_agent_mysql extends xivo_service_asterisk_agent_abstract_sql
{
	var $_param	= array('table' => 'agent');

	function xivo_service_asterisk_agent_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
