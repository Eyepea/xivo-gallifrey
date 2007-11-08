<?php

$protocols = array(XIVO_SRE_IPBX_AST_PROTO_SIP,
		   XIVO_SRE_IPBX_AST_PROTO_IAX);

if(($users = $ipbx->get_users_list($protocols)) === false)
	xivo_die('no-data');

$total = count($users);

$msg = xivo_msg('beg-data')."\n";

for($i = 0;$i < $total;$i++)
{
	$ref = &$users[$i];

	$msg .= '"'.str_replace('"','""',$ref['ufeatures']['protocol']).'"|'.
		'"'.str_replace('"','""',$ref['protocol']['name']).'"|'.
		'"'.str_replace('"','""',$ref['protocol']['secret']).'"|'.
		'"'.str_replace('"','""',intval((bool) $ref['ufeatures']['enableclient'])).'"|'.
		'"'.str_replace('"','""',$ref['ufeatures']['number']).'"|'.
		'"'.str_replace('"','""',intval((bool) $ref['protocol']['initialized'])).'"|'.
		'"'.str_replace('"','""',intval((bool) $ref['protocol']['commented'])).'"|'.
		'"'.str_replace('"','""',$ref['protocol']['callerid']).'"|'.
		'"'.str_replace('"','""',$ref['ufeatures']['firstname']).'"|'.
		'"'.str_replace('"','""',$ref['ufeatures']['lastname']).'"|'.
		'"'.str_replace('"','""',$ref['ufeatures']['context']).'"|'.
		'"'.str_replace('"','""',intval((bool) $ref['ufeatures']['enablehint'])).'"'."\n";
}

$msg .= xivo_msg('end-data')."\n";

if(isset($_QR['sum']) === true && $_QR['sum'] === md5($msg))
	$msg = xivo_msg('no-update');

die($msg);

?>
