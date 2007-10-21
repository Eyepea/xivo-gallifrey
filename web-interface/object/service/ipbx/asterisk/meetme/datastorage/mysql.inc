<?php

xivo_service_asterisk::required(array('meetme','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_meetme_mysql extends xivo_service_asterisk_meetme_abstract_sql
{

}

?>
