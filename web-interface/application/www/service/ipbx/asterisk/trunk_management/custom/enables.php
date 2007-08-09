<?php

$param['page'] = $page;

if(($arr = xivo_issa_val('peers',$_QR)) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

$disable = $act === 'disables' ? true : false;

$nb = count($arr);

$tfeatures_where = array();
$tfeatures_where['trunk'] = 'custom';

for($i = 0;$i < $nb;$i++)
{
	$tfeatures_where['trunkcustom'] = $arr[$i];

	if(($info['tfeatures'] = $tfeatures->get($tfeatures_where)) === false)
		continue;

	$trunkcustom->disable($arr[$i],$disable);
}

xivo_go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

?>
