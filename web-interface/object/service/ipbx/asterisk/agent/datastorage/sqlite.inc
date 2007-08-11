<?php

xivo_service_asterisk::required(array('agent','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_agent_sqlite extends xivo_service_asterisk_agent_abstract_sql
{
	var $_param	= array('table' => 'agent');

	function xivo_service_asterisk_agent_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
