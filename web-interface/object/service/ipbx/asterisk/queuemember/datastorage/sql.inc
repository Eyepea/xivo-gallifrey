<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_queuemember_sql extends xivo_service_asterisk_abstract_sql
{
	var $_disable		= null;
	var $_autoincrement	= false;
	var $_unique		= array(array('queue_name','channel','usertype','userid','category'));
}

?>
