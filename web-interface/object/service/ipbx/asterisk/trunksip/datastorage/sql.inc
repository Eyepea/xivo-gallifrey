<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_trunksip_sql extends xivo_service_asterisk_abstract_sql
{
	var $_forcecolumn	= array('protocol'	=> XIVO_SRE_IPBX_AST_PROTO_SIP,
					'category'	=> 'trunk');
}

?>
