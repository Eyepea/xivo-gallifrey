<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','meetmefeatures','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_meetmefeatures_sqlite extends xivo_service_asterisk_meetmefeatures_abstract_sql
{
	function xivo_service_asterisk_meetmefeatures_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
