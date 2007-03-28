<?php

if(xivo_issa('users',$_QR) === false || ($arr = xivo_get_aks($_QR['users'])) === false)
	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

$ufeatures = &$ipbx->get_module('userfeatures');
$qmember = &$ipbx->get_module('queuemember');
$ugroup = &$ipbx->get_module('usergroup');
$voicemail = &$ipbx->get_module('uservoicemail');
$extensions = &$ipbx->get_module('extensions');

$info = $hints_where = $localexten_where = array();

$hints_where['context'] = 'hints';

$localexten_where['app'] = 'Macro';
$localexten_where['appdata'] = 'superuser';

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

		$localexten_where['exten'] = $info['ufeatures']['number'];

		if($info['protocol']['context'] === '')
			$localexten_where['context'] = 'local-extensions';
		else
			$localexten_where['context'] = $info['protocol']['context'];

		if(($info['extensions'] = $extensions->get_where($localexten_where)) !== false
		&& $extensions->delete($info['extensions']['id']) === false)
		{
			$protocol->add_origin();
			$ufeatures->add_origin();
			continue;
		}

		if($qmember->delete_by_interface($interface) === false)
		{
			$protocol->add_origin();
			$ufeatures->add_origin();

			if($info['extensions'] !== false)
				$extensions->add_origin();

			continue;
		}

		$hints_where['exten'] = $info['ufeatures']['number'];
		$hints_where['app'] = $interface;

		if(($info['extensions'] = $extensions->get_where($hints_where)) !== false)
			$extensions->delete($info['extensions']['id']);

		if(($info['usergroup'] = $ugroup->get_by_user($info['ufeatures']['id'])) !== false)
			$ugroup->delete($info['usergroup']['id']);

		if(($info['voicemail'] = $voicemail->get_by_mailbox($info['ufeatures']['number'])) !== false)
			$voicemail->delete($info['voicemail']['id']);
	}
}

xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

?>
