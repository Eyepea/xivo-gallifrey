<?php

$protocols = array(XIVO_SRE_IPBX_AST_PROTO_SIP,
		   XIVO_SRE_IPBX_AST_PROTO_IAX);

$appuser = &$ipbx->get_application('user',null,false);

if(($users = $appuser->get_users_list($protocols)) === false)
	xivo_die('no-data');

$total = count($users);

$msg =	xivo_msg('beg-data')."\n".
	'"protocol"|"name"|"secret"|"enableclient"|'.
	'"number"|"initialized"|"commented"|"callerid"|'.
	'"firstname"|"lastname"|"context"|"enablehint"'."\n";

for($i = 0;$i < $total;$i++)
{
	$ref = &$users[$i];

	$msg .= '"'.str_replace('"','""',$ref['protocol']).'"|'.
		'"'.str_replace('"','""',$ref['name']).'"|'.
		'"'.str_replace('"','""',$ref['secret']).'"|'.
		'"'.str_replace('"','""',intval((bool) $ref['enableclient'])).'"|'.
		'"'.str_replace('"','""',$ref['number']).'"|'.
		'"'.str_replace('"','""',intval((bool) $ref['initialized'])).'"|'.
		'"'.str_replace('"','""',intval((bool) $ref['commented'])).'"|'.
		'"'.str_replace('"','""',$ref['callerid']).'"|'.
		'"'.str_replace('"','""',$ref['firstname']).'"|'.
		'"'.str_replace('"','""',$ref['lastname']).'"|'.
		'"'.str_replace('"','""',$ref['context']).'"|'.
		'"'.str_replace('"','""',intval((bool) $ref['enablehint'])).'"'."\n";
}

$msg .= xivo_msg('end-data')."\n";

if(isset($_QR['sum']) === true && $_QR['sum'] === md5($msg))
	$msg = xivo_msg('no-update');

die($msg);

?>
