<?php

require_once(xivo_file::joinpath(XIVO_PATH_LIB,'datastorage','helper','table.inc'));

class xivo_entity_abstract_sql extends xivo_datastorage_helper_table
{
	var $_table	= 'entity';
	var $_primary	= array('id' => 0);
	var $_disable	= 'disable';
	var $_unique	= 'name';

	function xivo_entity_abstract_sql(&$dso,$param=array())
	{
		parent::_init(&$dso,$param);
	}
}

?>
