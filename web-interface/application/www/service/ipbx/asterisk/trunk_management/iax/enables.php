<?php

$param['page'] = $page;

if(($arr = xivo_issa_val('peers',$_QR)) === false)
	$_QRY->go($_HTML->url('service/ipbx/trunk_management/iax'),$param);

$generaliax = &$ipbx->get_module('generaliax');

$disable = $act === 'disables';

$nb = count($arr);

$tfeatures_where = array();
$tfeatures_where['trunk'] = 'iax';

for($i = 0;$i < $nb;$i++)
{
	$tfeatures_where['trunkid'] = strval($arr[$i]);

	if(($info['tfeatures'] = $tfeatures->get_where($tfeatures_where)) === false)
		continue;

	if(xivo_ulongint($info['tfeatures']['registerid']) !== 0)
	{
		if($disable === true || (bool) $info['tfeatures']['registercommented'] === true)
			$generaliax->disable($info['tfeatures']['registerid']);
		else
			$generaliax->enable($info['tfeatures']['registerid']);
	}
	
	$trunkiax->disable($info['tfeatures']['trunkid'],$disable);
}

$_QRY->go($_HTML->url('service/ipbx/trunk_management/iax'),$param);

?>
