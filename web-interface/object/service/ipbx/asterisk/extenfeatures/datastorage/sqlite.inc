<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','extensions','datastorage','sqlite.inc'),true);

class xivo_service_asterisk_extenfeatures_sqlite extends xivo_service_asterisk_extensions_sqlite
{
	function xivo_service_asterisk_extenfeatures_sqlite(&$dso,$param=array())
	{
		$this->xivo_service_asterisk_extensions_sqlite(&$dso,$param);
	}
}

?>
