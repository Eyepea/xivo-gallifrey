<?php

$param['page'] = $page;

if(($arr = xivo_issa_val('peers',$_QR)) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

$generalsip = &$ipbx->get_module('generalsip');

$disable = $act === 'disables' ? true : false;

$nb = count($arr);

$tfeatures_where = array();
$tfeatures_where['trunk'] = 'sip';

for($i = 0;$i < $nb;$i++)
{
	$tfeatures_where['trunksip'] = $arr[$i];

	if(($info['tfeatures'] = $tfeatures->get($tfeatures_where)) === false)
		continue;

	if((int) $info['tfeatures']['registerid'] !== 0)
	{
		if($disable === true || (bool) $info['tfeatures']['registercommented'] === true)
			$generalsip->disable($info['tfeatures']['registerid']);
		else
			$generalsip->enable($info['tfeatures']['registerid']);
	}
	
	$trunksip->disable($arr[$i],$disable);
}

xivo_go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

?>
