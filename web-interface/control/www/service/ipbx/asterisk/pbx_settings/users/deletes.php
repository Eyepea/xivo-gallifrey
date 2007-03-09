<?php

if(xivo_issa('users',$_QR) === false || ($arr = xivo_get_aks($_QR['users'])) === false)
	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

$ufeatures = &$ipbx->get_module('userfeatures');
$qmember = &$ipbx->get_module('queuemember');
$ugroup = &$ipbx->get_module('usergroup');
$voicemail = &$ipbx->get_module('uservoicemail');

$info = array();

for($i = 0;$i < $arr['cnt'];$i++)
{
	$k = &$arr['keys'][$i];

	if(is_array($_QR['users'][$k]) === false || ($protocol = &$ipbx->get_protocol_module($k)) === false)
		continue;

	$v = array_values($_QR['users'][$k]);

	if(($nb = count($v)) === 0)
		continue;

	for($j = 0;$j < $nb;$j++)
	{
		if(($info['ufeatures'] = $ufeatures->get_by_protocol($v[$j],$k)) === false
		|| ($info['protocol'] = $protocol->get($info['ufeatures']['protocolid'])) === false
		|| ($interface = $ipbx->mk_interface($info['ufeatures']['protocol'],$info['protocol']['name'])) === false)
			continue;

		if($protocol->delete($info['protocol']['id']) === false)
			continue;

		if($ufeatures->delete($info['ufeatures']['id']) === false)
		{
			$protocol->add_origin();
			continue;
		}

		if($qmember->delete_by_interface($interface) === false)
		{
			$protocol->add_origin();
			$ufeatures->add_origin();
			continue;
		}

		if(($info['usergroup'] = $ugroup->get_by_user($info['ufeatures']['id'])) !== false)
			$ugroup->delete($info['usergroup']['id']);

		if(($info['voicemail'] = $voicemail->get_by_mailbox($info['ufeatures']['number'])) !== false)
			$voicemail->delete($info['voicemail']['id']);
	}
}

xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

?>
