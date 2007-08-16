<?php

$param['page'] = $page;

if(($arr = xivo_issa_val('peers',$_QR)) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

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
		$trunkcustom->add_origin();
}

xivo_go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

?>
