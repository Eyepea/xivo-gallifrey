<?php

$param['page'] = $page;

if(xivo_issa('users',$_QR) === false || ($arr = xivo_get_aks($_QR['users'])) === false)
	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

$ufeatures = &$ipbx->get_module('userfeatures');
$qmember = &$ipbx->get_module('queuemember');
$ugroup = &$ipbx->get_module('usergroup');
$voicemail = &$ipbx->get_module('uservoicemail');
$extenumbers = &$ipbx->get_module('extenumbers');
$localexten = $hintsexten = &$ipbx->get_module('extensions');
$dfeatures = &$ipbx->get_module('didfeatures');
$autoprov = &$ipbx->get_module('autoprov');

$info = $extenum_where = $dfeatures_where = $quser_where = array();

$dfeatures_where['type'] = 'user';
$dfeatures_where['commented'] = 0;

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

		$info['dfeatures'] = false;

		$dfeatures_where['typeid'] = $info['ufeatures']['id'];

		if(($info['extenumbers'] = $extenumbers->get_where($extenum_where)) !== false)
		{
			if($extenumbers->delete($info['extenumbers']['id']) === false
			|| (($info['dfeatures'] = $dfeatures->get_list_where($dfeatures_where,false)) !== false
			   && $dfeatures->edit_where($dfeatures_where,array('commented' => 1)) === false) === true)
			{
				$protocol->add_origin();
				$ufeatures->add_origin();
		
				if($info['localexten'] !== false)
					$localexten->add_origin();

				if($info['dfeatures'] !== false)
					$extenumbers->add_origin();
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

			if($info['dfeatures'] !== false)
				$dfeatures->edit_list($info['dfeatures'],array('commented' => 0));
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

			if($info['dfeatures'] !== false)
				$dfeatures->edit_list($info['dfeatures'],array('commented' => 0));

			if($info['hints'] !== false)
				$hintsexten->add_origin();
			continue;
		}

		$ugroup->delete_where(array('userid' => $info['ufeatures']['id']));

		$voicemail->delete_where(array('mailbox' => $info['ufeatures']['number'],
					       'context' => $info['ufeatures']['context']));

		if($autoprov->get_where(array('iduserfeatures' => $info['ufeatures']['id'])) !== false)
			$autoprov->userdeleted($info['ufeatures']['id']);
	}
}

xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

?>
