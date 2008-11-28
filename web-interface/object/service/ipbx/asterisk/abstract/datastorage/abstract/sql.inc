<?php

require_once(xivo_file::joinpath(XIVO_PATH_LIB,'datastorage','helper','table.inc'));

class xivo_service_asterisk_abstract_sql extends xivo_datastorage_helper_table
{
	var $_disable	= 'commented';

	function xivo_service_asterisk_abstract_sql(&$dso,$param=array())
	{
		parent::_init(&$dso,$param);
	}
}

?>
