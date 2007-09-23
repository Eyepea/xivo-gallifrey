<?php

$param['page'] = $page;

if(($arr = xivo_issa_val('peers',$_QR)) === false)
	$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

$outcall = &$ipbx->get_module('outcall');

$nb = count($arr);

$tfeatures_where = array();
$tfeatures_where['trunk'] = 'custom';

for($i = 0;$i < $nb;$i++)
{
	$tfeatures_where['trunkid'] = strval($arr[$i]);

	if(($info['tfeatures'] = $tfeatures->get_where($tfeatures_where)) === false
	|| $trunkcustom->delete($info['tfeatures']['trunkid']) === false)
		continue;

	if($tfeatures->delete($info['tfeatures']['id']) === false)
	{
		$trunkcustom->add_origin();
		continue;
	}

	$outcall->unlinked_where(array('trunkfeaturesid' => $info['tfeatures']['id']));
}

$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

?>
