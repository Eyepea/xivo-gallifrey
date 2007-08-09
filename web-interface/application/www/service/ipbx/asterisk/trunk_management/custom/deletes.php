<?php

$param['page'] = $page;

if(($arr = xivo_issa_val('peers',$_QR)) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

$nb = count($arr);

for($i = 0;$i < $nb;$i++)
{
	$id = &$_QR['peers'][$i];

	if(($info['trunk'] = $trunkcustom->get($id)) === false
	|| ($info['tfeatures'] = $tfeatures->get_by_trunk($info['trunk']['id'],'custom')) === false)
		continue;

	if($trunkcustom->delete($info['trunk']['id']) === false)
		continue;

	if($tfeatures->delete($info['tfeatures']['id']) === false)
		$trunkcustom->add_origin();
}

xivo_go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

?>
