<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','phonebook','datastorage','abstract','sql.inc'),true);

class xivo_application_service_asterisk_phonebook_mysql extends xivo_application_service_asterisk_phonebook_abstract_sql
{
	function xivo_application_service_asterisk_phonebook_mysql(&$dso,&$dsoconf)
	{
		$this->_init(&$dso,&$dsoconf);
	}
}

?>
