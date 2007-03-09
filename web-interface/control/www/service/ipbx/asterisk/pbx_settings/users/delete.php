<?php

$ufeatures = &$ipbx->get_module('userfeatures');
$voicemail = &$ipbx->get_module('uservoicemail');
$qmember = &$ipbx->get_module('queuemember');
$ugroup = &$ipbx->get_module('usergroup');

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

	if($qmember->delete_by_interface($interface) === false)
	{
		$protocol->add_origin();
		$ufeatures->add_origin();
		break;
	}

	if(($info['usergroup'] = $ugroup->get_by_user($info['ufeatures']['id'])) !== false)
		$ugroup->delete($info['usergroup']['id']);

	if(($info['voicemail'] = $voicemail->get_by_mailbox($info['ufeatures']['number'])) !== false)
		$voicemail->delete($info['voicemail']['id']);
}
while(false);

xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

?>
