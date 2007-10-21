<?php

xivo_service_asterisk::required(array('generalqueue','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_generalqueue_mysql extends xivo_service_asterisk_generalqueue_abstract_sql
{

}

?>
