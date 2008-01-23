<?php

$param['page'] = $page;

if(($arr = xivo_issa_val('peers',$_QR)) === false)
	$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

$generalsip = &$ipbx->get_module('generalsip');

$disable = $act === 'disables';

$nb = count($arr);

$tfeatures_where = array();
$tfeatures_where['trunk'] = 'sip';

for($i = 0;$i < $nb;$i++)
{
	$tfeatures_where['trunkid'] = strval($arr[$i]);

	if(($info['tfeatures'] = $tfeatures->get_where($tfeatures_where)) === false)
		continue;

	if(xivo_ulongint($info['tfeatures']['registerid']) !== 0)
	{
		if($disable === true || (bool) $info['tfeatures']['registercommented'] === true)
			$generalsip->disable($info['tfeatures']['registerid']);
		else
			$generalsip->enable($info['tfeatures']['registerid']);
	}
	
	$trunksip->disable($info['tfeatures']['trunkid'],$disable);
}

$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

?>
