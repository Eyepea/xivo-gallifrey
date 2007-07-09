<?php

$param['page'] = $page;

if(($arr = xivo_issa_val('peers',$_QR)) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/iax'),$param);

$generaliax = &$ipbx->get_module('generaliax');

$disable = $act === 'disables' ? true : false;

$nb = count($arr);

$tfeatures_where = array();
$tfeatures_where['trunk'] = 'iax';

for($i = 0;$i < $nb;$i++)
{
	$tfeatures_where['trunkiax'] = $arr[$i];

	if(($info['tfeatures'] = $tfeatures->get($tfeatures_where)) === false)
		continue;

	if((int) $info['tfeatures']['registerid'] !== 0)
	{
		if($disable === true || (bool) $info['tfeatures']['registercommented'] === true)
			$generaliax->disable($info['tfeatures']['registerid']);
		else
			$generaliax->enable($info['tfeatures']['registerid']);
	}
	
	$trunkiax->disable($arr[$i],$disable);
}

xivo_go($_HTML->url('service/ipbx/trunk_management/iax'),$param);

?>
