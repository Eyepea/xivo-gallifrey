<?php

$param['page'] = $page;

if(xivo_issa('users',$_QR) === false || ($arr = xivo_get_aks($_QR['users'])) === false)
	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

$disable = $act === 'disables' ? true : false;

for($i = 0;$i < $arr['cnt'];$i++)
{
	$k = $arr['keys'][$i];

	if(($protocol = &$ipbx->get_protocol_module($k)) === false
	|| ($v = xivo_issa_val($k,$_QR['users'])) === false)
		continue;

	$nb = count($v);

	for($j = 0;$j < $nb;$j++)
		$protocol->disable(strval($v[$j]),$disable);
}

xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

?>
