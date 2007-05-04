<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','trunkfeatures','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_trunkfeatures_sqlite extends xivo_service_asterisk_trunkfeatures_abstract_sql
{
	function xivo_service_asterisk_trunkfeatures_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
