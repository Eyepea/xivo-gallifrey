<?php

$ufeatures = &$ipbx->get_module('userfeatures');
$voicemail = &$ipbx->get_module('uservoicemail');
$qmember = &$ipbx->get_module('queuemember');
$ugroup = &$ipbx->get_module('usergroup');
$extensions = &$ipbx->get_module('extensions');

$info = array();

if(isset($_QR['id']) === false
|| ($info['ufeatures'] = $ufeatures->get($_QR['id'])) === false
|| ($protocol = &$ipbx->get_protocol_module($info['ufeatures']['protocol'])) === false
|| ($info['protocol'] = $protocol->get($info['ufeatures']['protocolid'])) === false
|| ($interface = $ipbx->mk_interface($info['ufeatures']['protocol'],$info['protocol']['name'])) === false)
	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

do
{
	if($protocol->delete($info['protocol']['id']) === false)
		break;

	if($ufeatures->delete($info['ufeatures']['id']) === false)
	{
		$protocol->add_origin();
		break;
	}

	$localexten_where = array();
	$localexten_where['exten'] = $info['ufeatures']['number'];
	$localexten_where['app'] = 'Macro';
	$localexten_where['appdata'] = 'superuser';

	if($info['protocol']['context'] === '')
		$localexten_where['context'] = 'local-extensions';
	else
		$localexten_where['context'] = $info['protocol']['context'];

	if(($info['extensions'] = $extensions->get_where($localexten_where)) !== false
	&& $extensions->delete($info['extensions']['id']) === false)
	{
		$protocol->add_origin();
		$ufeatures->add_origin();
		break;
	}

	if($qmember->delete_by_interface($interface) === false)
	{
		$protocol->add_origin();
		$ufeatures->add_origin();

		if($info['extensions'] !== false)
			$extensions->add_origin();
		break;
	}

	$hints_where = array();
	$hints_where['context'] = 'hints';
	$hints_where['exten'] = $info['ufeatures']['number'];
	$hints_where['app'] = $interface;

	if(($info['extensions'] = $extensions->get_where($hints_where)) !== false)
		$extensions->delete($info['extensions']['id']);

	if(($info['usergroup'] = $ugroup->get_by_user($info['ufeatures']['id'])) !== false)
		$ugroup->delete($info['usergroup']['id']);

	if(($info['voicemail'] = $voicemail->get_by_mailbox($info['ufeatures']['number'])) !== false)
		$voicemail->delete($info['voicemail']['id']);
}
while(false);

xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

?>
