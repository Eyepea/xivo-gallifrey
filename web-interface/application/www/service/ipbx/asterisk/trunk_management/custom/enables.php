<?php

$param['page'] = $page;

if(($arr = xivo_issa_val('peers',$_QR)) === false)
	$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

$disable = $act === 'disables' ? true : false;

$nb = count($arr);

$tfeatures_where = array();
$tfeatures_where['trunk'] = 'custom';

for($i = 0;$i < $nb;$i++)
{
	$tfeatures_where['trunkid'] = strval($arr[$i]);

	if(($info['tfeatures'] = $tfeatures->get_where($tfeatures_where)) === false)
		continue;

	$trunkcustom->disable($info['tfeatures']['trunkid'],$disable);
}

$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

?>
