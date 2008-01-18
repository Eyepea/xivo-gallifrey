<?php

xivo_service_asterisk::required(array('staticconf','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_generaliax_sql extends xivo_service_asterisk_staticconf_abstract_sql
{
	var $_forcecolumn	= array('filename' => 'iax.conf');
}

?>
