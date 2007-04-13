<?php

$param['page'] = $page;

if(xivo_issa('users',$_QR) === false || ($arr = xivo_get_aks($_QR['users'])) === false)
	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

$disable = $act === 'disables' ? true : false;
	
for($i = 0;$i < $arr['cnt'];$i++)
{
	$k = &$arr['keys'][$i];

	if(is_array($_QR['users'][$k]) === false
	|| ($protocol = &$ipbx->get_protocol_module($k)) === false)
		continue;

	$v = array_values($_QR['users'][$k]);

	if(($nb = count($v)) === 0)
		continue;

	for($j = 0;$j < $nb;$j++)
		$protocol->disable($v[$j],$disable);
}

xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

?>
