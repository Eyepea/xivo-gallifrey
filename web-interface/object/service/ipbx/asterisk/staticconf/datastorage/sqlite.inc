<?php

xivo_service_asterisk::required(array('staticconf','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_staticconf_sqlite extends xivo_service_asterisk_staticconf_abstract_sql
{

}

?>
