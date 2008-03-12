<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'user','datastorage','abstract','sql.inc'),true);

class xivo_user_sqlite extends xivo_user_abstract_sql
{

}

?>
