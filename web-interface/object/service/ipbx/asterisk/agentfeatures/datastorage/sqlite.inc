<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','agentfeatures','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_agentfeatures_sqlite extends xivo_service_asterisk_agentfeatures_abstract_sql
{
	function xivo_service_asterisk_agentfeatures_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
