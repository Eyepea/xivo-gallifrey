<?php

if(($users = $ipbx->get_users_list()) === false)
	die('XIVO-WEBI: no-data');

$total = count($users);

$msg = 'XIVO-WEBI: beg-data'."\n";

for($i = 0;$i < $total;$i++)
{
	$ref = &$users[$i];

	$msg .= '"'.str_replace('"','""',$ref['ufeatures']['protocol']).'"|'.
		'"'.str_replace('"','""',$ref['protocol']['name']).'"|'.
		'"'.str_replace('"','""',$ref['protocol']['secret']).'"|'.
		'"'.str_replace('"','""',$ref['ufeatures']['enableclient']).'"|'.
		'"'.str_replace('"','""',$ref['ufeatures']['number']).'"|'.
		'"'.str_replace('"','""',intval((bool) $ref['protocol']['initialized'])).'"|'.
		'"'.str_replace('"','""',intval((bool) $ref['protocol']['commented'])).'"|'.
		'"'.str_replace('"','""',$ref['protocol']['callerid']).'"|'.
		'"'.str_replace('"','""',$ref['ufeatures']['firstname']).'"|'.
		'"'.str_replace('"','""',$ref['ufeatures']['lastname']).'"|'.
		'"'.str_replace('"','""',$ref['ufeatures']['context']).'"'."\n";
}

$msg .= 'XIVO-WEBI: end-data'."\n";

if(isset($_QR['sum']) === true && $_QR['sum'] === md5($msg))
	$msg = 'XIVO-WEBI: no-update';

die($msg);

?>
