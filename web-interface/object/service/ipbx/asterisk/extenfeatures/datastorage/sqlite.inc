<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'extensions'.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'sqlite.inc');

class xivo_service_asterisk_extenfeatures_sqlite extends xivo_service_asterisk_extensions_sqlite
{
	function xivo_service_asterisk_extenfeatures_sqlite(&$dso,$param=array())
	{
		$this->xivo_service_asterisk_extensions_sqlite(&$dso,$param);
	}
}

?>
