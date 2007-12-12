<?php

xivo_service_asterisk::required(array('realstatic','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_realstatic_sqlite extends xivo_service_asterisk_realstatic_abstract_sql
{

}

?>
