<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','queuefeatures','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_queuefeatures_sqlite extends xivo_service_asterisk_queuefeatures_abstract_sql
{
	function xivo_service_asterisk_queuefeatures_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
