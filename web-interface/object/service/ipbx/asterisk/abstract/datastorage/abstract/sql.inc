<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'sql.inc');

class xivo_service_asterisk_abstract_sql extends xivo_abstract_datastorage_sql
{
	var $_disable_varname	= 'commented';

	function xivo_service_asterisk_abstract_sql(&$dso,$param=array())
	{
		parent::_init(&$dso,$param);
	}
}

?>
