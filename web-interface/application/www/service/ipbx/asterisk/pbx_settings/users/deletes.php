<?php

$param['page'] = $page;

if(xivo_issa('users',$_QR) === false || ($arr = xivo_get_aks($_QR['users'])) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

$ufeatures = &$ipbx->get_module('userfeatures');
$qmember = &$ipbx->get_module('queuemember');
$ugroup = &$ipbx->get_module('usergroup');
$voicemail = &$ipbx->get_module('uservoicemail');
$extenumbers = &$ipbx->get_module('extenumbers');
$localexten = $hintsexten = &$ipbx->get_module('extensions');
$autoprov = &$ipbx->get_module('autoprov');
$incall = &$ipbx->get_module('incall');
$schedule = &$ipbx->get_module('schedule');

$info = $extenum_where = $incall_where = $quser_where = array();

$quser_where['usertype'] = 'user';

for($i = 0;$i < $arr['cnt'];$i++)
{
	$k = &$arr['keys'][$i];

	if(($protocol = &$ipbx->get_protocol_module($k)) === false
	|| ($v = xivo_issa_val($k,$_QR['users'])) === false)
		continue;

	$nb = count($v);

	for($j = 0;$j < $nb;$j++)
	{
		if(($info['ufeatures'] = $ufeatures->get_by_protocol($v[$j],$k)) === false
		|| ($info['protocol'] = $protocol->get($info['ufeatures']['protocolid'])) === false
		|| $protocol->delete($info['protocol']['id']) === false)
			continue;

		if($ufeatures->delete($info['ufeatures']['id']) === false)
		{
			$protocol->add_origin();
			continue;
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
			continue;
		}

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
				continue;
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
			continue;
		}

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
			continue;
		}

		$ugroup->delete_where(array('userid' => $info['ufeatures']['id']));

		if($info['extenumbers'] !== false)
		{
			$incall->unlinked_where(array('type' => 'user',
						      'typeval' => $info['ufeatures']['id']));

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
}

$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

?>
