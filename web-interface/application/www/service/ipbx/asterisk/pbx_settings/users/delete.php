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
|| ($info['protocol'] = $protocol->get($info['ufeatures']['protocolid'])) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

do
{
	if($protocol->delete($info['protocol']['id']) === false)
		break;

	if($ufeatures->delete($info['ufeatures']['id']) === false)
	{
		$protocol->add_origin();
		break;
	}

	if($info['protocol']['context'] === '')
		$localextencontext = 'local-extensions';
	else
		$localextencontext = $info['protocol']['context'];

	if(($info['localexten'] = $localexten->get_exten('macro',
							 $info['ufeatures']['number'],
							 $localextencontext,
							 array('appdata' => 'superuser'))) !== false
	&& $localexten->delete($info['localexten']['id']) === false)
	{
		$protocol->add_origin();
		$ufeatures->add_origin();
		break;
	}

	$extenum_where = array();
	$extenum_where['exten'] = $info['ufeatures']['number'];
	$extenum_where['context'] = $localextencontext;

	if(($info['extenumbers'] = $extenumbers->get_where($extenum_where)) !== false)
	{
		if($extenumbers->delete($info['extenumbers']['id']) === false)
		{
			$protocol->add_origin();
			$ufeatures->add_origin();
		
			if($info['localexten'] !== false)
				$localexten->add_origin();
			break;
		}
	}

	if(($info['hints'] = $hintsexten->get_hints($info['protocol']['name'],
						    $info['ufeatures']['protocol'],
						    $info['ufeatures']['number'])) !== false
	&& $hintsexten->delete($info['hints']['id']) === false)
	{
		$protocol->add_origin();
		$ufeatures->add_origin();
		
		if($info['localexten'] !== false)
			$localexten->add_origin();

		if($info['extenumbers'] !== false)
			$extenumbers->add_origin();
		break;
	}

	$quser_where = array();
	$quser_where['usertype'] = 'user';
	$quser_where['userid'] = $info['ufeatures']['id'];

	if($qmember->get_id($quser_where) !== false
	&& $qmember->delete_where($quser_where) === false)
	{
		$protocol->add_origin();
		$ufeatures->add_origin();

		if($info['localexten'] !== false)
			$localexten->add_origin();

		if($info['extenumbers'] !== false)
			$extenumbers->add_origin();

		if($info['hints'] !== false)
			$hintsexten->add_origin();
		break;
	}

	$ugroup->delete_where(array('userid' => $info['ufeatures']['id']));

	if($info['extenumbers'] !== false)
	{
		$incall = &$ipbx->get_module('incall');

		$incall->unlinked_where(array('type' => 'user',
					      'typeval' => $info['ufeatures']['id']));

		$schedule = &$ipbx->get_module('schedule');

		$schedule->unlinked_where(array('typetrue' => 'user',
					        'typevaltrue' => $info['ufeatures']['id']));

		$schedule->unlinked_where(array('typefalse' => 'user',
					        'typevalfalse' => $info['ufeatures']['id']));
	}

	$voicemail->delete_where(array('mailbox' => $info['ufeatures']['number'],
				       'context' => $info['ufeatures']['context']));

	if($autoprov->get_where(array('iduserfeatures' => $info['ufeatures']['id'])) !== false)
		$autoprov->userdeleted($info['ufeatures']['id']);
}
while(false);

$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

?>
