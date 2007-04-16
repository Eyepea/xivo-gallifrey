<?php

$ufeatures = &$ipbx->get_module('userfeatures');
$voicemail = &$ipbx->get_module('uservoicemail');
$qmember = &$ipbx->get_module('queuemember');
$ugroup = &$ipbx->get_module('usergroup');
$extenumbers = &$ipbx->get_module('extenumbers');
$localexten = $hintsexten = &$ipbx->get_module('extensions');
$autoprov = &$ipbx->get_module('autoprov');

$info = array();

$param['page'] = $page;

if(isset($_QR['id']) === false
|| ($info['ufeatures'] = $ufeatures->get($_QR['id'])) === false
|| ($protocol = &$ipbx->get_protocol_module($info['ufeatures']['protocol'])) === false
|| ($info['protocol'] = $protocol->get($info['ufeatures']['protocolid'])) === false
|| ($interface = $ipbx->mk_interface($info['protocol']['name'],
				     $info['ufeatures']['protocol'],
			     	     $info['ufeatures']['number'],
			     	     $info['protocol']['context'])) === false)
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

	if(($info['localexten'] = $localexten->get_where($localexten_where)) !== false
	&& $localexten->delete($info['localexten']['id']) === false)
	{
		$protocol->add_origin();
		$ufeatures->add_origin();
		break;
	}

	$extenum_where = array();
	$extenum_where['number'] = $localexten_where['exten'];
	$extenum_where['context'] = $localexten_where['context'];

	$info['dfeatures'] = false;

	if(($info['extenumbers'] = $extenumbers->get_where($extenum_where)) !== false)
	{
		$dfeatures = &$ipbx->get_module('didfeatures');
		$dfeatures_where = array();
		$dfeatures_where['type'] = 'user';
		$dfeatures_where['typeid'] = $info['ufeatures']['id'];
		$dfeatures_where['disable'] = 0;

		if($extenumbers->delete($info['extenumbers']['id']) === false
		|| (($info['dfeatures'] = $dfeatures->get_list_where($dfeatures_where,false)) !== false
		   && $dfeatures->edit_where($dfeatures_where,array('disable' => 1)) === false) === true)
		{
			$protocol->add_origin();
			$ufeatures->add_origin();
		
			if($info['localexten'] !== false)
				$localexten->add_origin();

			if($info['dfeatures'] !== false)
				$extenumbers->add_origin();
			break;
		}
	}

	$hints_where = array();
	$hints_where['context'] = 'hints';
	$hints_where['exten'] = $info['ufeatures']['number'];
	$hints_where['app'] = $ipbx->mk_interface($info['protocol']['name'],$info['ufeatures']['protocol']);

	$info['hints'] = false;

	if($hints_where['app'] !== false
	&& ($info['hints'] = $hintsexten->get_where($hints_where)) !== false
	&& $hintsexten->delete($info['hints']['id']) === false)
	{
		$protocol->add_origin();
		$ufeatures->add_origin();
		
		if($info['localexten'] !== false)
			$localexten->add_origin();

		if($info['extenumbers'] !== false)
			$extenumbers->add_origin();

		if($info['dfeatures'] !== false)
			$dfeatures->edit_list_where($info['dfeatures'],array('disable' => 0));
		break;
	}

	if($qmember->get_list_by_interface($interface) !== false
	&& $qmember->delete_by_interface($interface) === false)
	{
		$protocol->add_origin();
		$ufeatures->add_origin();

		if($info['localexten'] !== false)
			$localexten->add_origin();

		if($info['extenumbers'] !== false)
			$extenumbers->add_origin();

		if($info['dfeatures'] !== false)
			$dfeatures->edit_list_where($info['dfeatures'],array('disable' => 0));

		if($info['hints'] !== false)
			$hintsexten->add_origin();
		break;
	}

	if(($info['usergroup'] = $ugroup->get_by_user($info['ufeatures']['id'])) !== false)
		$ugroup->delete($info['usergroup']['id']);

	if(($info['voicemail'] = $voicemail->get_by_mailbox($info['ufeatures']['number'])) !== false)
		$voicemail->delete($info['voicemail']['id']);

	if($autoprov->get_by_iduserfeatures($info['ufeatures']['id']) !== false)
		$autoprov->userdeleted($info['ufeatures']['id']);
}
while(false);

xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

?>
