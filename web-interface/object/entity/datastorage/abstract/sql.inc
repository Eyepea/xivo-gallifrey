<?php

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'helper'.XIVO_SEP_DIR.'table.inc');

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
