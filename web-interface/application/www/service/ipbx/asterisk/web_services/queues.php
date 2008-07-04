<?php

$appqueue = &$ipbx->get_application('queue',null,false);
$appgroup = &$ipbx->get_application('group',null,false);

$queues = $appqueue->get_queues_list();
$groups = $appgroup->get_groups_list();

if($queues === false && $groups === false)
	xivo_die('no-data');

$total = count($queues);

$msg =	xivo_msg('beg-data')."\n".
	'"category"|"name"|"number"|"context"|"commented"'."\n";

for($i = 0;$i < $total;$i++)
{
	$ref = &$queues[$i];

	$msg .= '"'.str_replace('"','""',$ref['queue']['category']).'"|'.
		'"'.str_replace('"','""',$ref['queue']['name']).'"|'.
		'"'.str_replace('"','""',$ref['qfeatures']['number']).'"|'.
		'"'.str_replace('"','""',$ref['qfeatures']['context']).'"|'.
		'"'.str_replace('"','""',intval((bool) $ref['queue']['commented'])).'"'."\n";
}

$total = count($groups);

for($i = 0;$i < $total;$i++)
{
	$ref = &$groups[$i];

	$msg .= '"'.str_replace('"','""',$ref['queue']['category']).'"|'.
		'"'.str_replace('"','""',$ref['queue']['name']).'"|'.
		'"'.str_replace('"','""',$ref['gfeatures']['number']).'"|'.
		'"'.str_replace('"','""',$ref['gfeatures']['context']).'"|'.
		'"'.str_replace('"','""',intval((bool) $ref['queue']['commented'])).'"'."\n";
}

$msg .= xivo_msg('end-data')."\n";

if(isset($_QR['sum']) === true && $_QR['sum'] === md5($msg))
	$msg = xivo_msg('no-update');

die($msg);

?>
