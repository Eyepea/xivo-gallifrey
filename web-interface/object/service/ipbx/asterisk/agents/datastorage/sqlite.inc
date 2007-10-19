<?php

xivo_service_asterisk::required(array('agents','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_agents_sqlite extends xivo_service_asterisk_agents_abstract_sql
{

}

?>
