<?php

$param['page'] = $page;

if(($arr = xivo_issa_val('peers',$_QR)) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

$nb = count($arr);

for($i = 0;$i < $nb;$i++)
{
	if(($info['trunk'] = $trunkcustom->get($id)) === false
	|| ($info['tfeatures'] = $tfeatures->get_where(array(
					'trunkid' => $info['trunk']['id'],
					'trunk' => 'custom'))) === false)
		continue;

	if($trunkcustom->delete($info['trunk']['id']) === false)
		continue;

	if($tfeatures->delete($info['tfeatures']['id']) === false)
		$trunkcustom->add_origin();
}

xivo_go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

?>
