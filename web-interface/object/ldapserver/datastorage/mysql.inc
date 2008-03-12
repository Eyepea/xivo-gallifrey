<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'ldapserver','datastorage','abstract','sql.inc'),true);

class xivo_ldapserver_mysql extends xivo_ldapserver_abstract_sql
{

}

?>
