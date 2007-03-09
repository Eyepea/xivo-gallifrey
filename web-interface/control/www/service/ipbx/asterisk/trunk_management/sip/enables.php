<?php

if(xivo_issa('peers',$_QR) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/sip'),'act=list&page='.$page);

$disable = $act === 'disables' ? true : false;

$arr = array_values($_QR['peers']);
$nb = count($arr);

for($i = 0;$i < $nb;$i++)
{
	$trunksip->disable($_QR['peers'][$i],$disable);
}

xivo_go($_HTML->url('service/ipbx/trunk_management/sip'),'act=list&page='.$page);

?>
