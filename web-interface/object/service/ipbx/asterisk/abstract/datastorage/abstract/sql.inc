<?php

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'helper'.XIVO_SEP_DIR.'table.inc');

class xivo_service_asterisk_abstract_sql extends xivo_datastorage_helper_table
{
	var $_disable	= 'commented';

	function xivo_service_asterisk_abstract_sql(&$dso,$param=array())
	{
		parent::_init(&$dso,$param);
	}
}

?>
