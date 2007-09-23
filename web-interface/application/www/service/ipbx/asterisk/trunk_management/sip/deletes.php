<?php

$param['page'] = $page;

if(($arr = xivo_issa_val('peers',$_QR)) === false)
	$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

$generalsip = &$ipbx->get_module('generalsip');
$outcall = &$ipbx->get_module('outcall');

$nb = count($arr);

$tfeatures_where = array();
$tfeatures_where['trunk'] = 'sip';

for($i = 0;$i < $nb;$i++)
{
	$tfeatures_where['trunkid'] = strval($arr[$i]);

	if(($info['tfeatures'] = $tfeatures->get_where($tfeatures_where)) === false
	|| $trunksip->delete($info['tfeatures']['trunkid']) === false)
		continue;

	if($tfeatures->delete($info['tfeatures']['id']) === false)
	{
		$trunksip->add_origin();
		continue;
	}

	if($info['tfeatures']['registerid'] !== 0
	&& $generalsip->delete($info['tfeatures']['registerid']) === false)
	{
		$trunksip->add_origin();
		$tfeatures->add_origin();
		continue;
	}

	$outcall->unlinked_where(array('trunkfeaturesid' => $info['tfeatures']['id']));
}

$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

?>
