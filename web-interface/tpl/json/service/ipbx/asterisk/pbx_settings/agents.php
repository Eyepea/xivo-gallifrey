<?php

$agents = $this->get_var('agents');

if(is_array($agents) === false)
	xivo_die('Error/500');
else if(($nb = count($agents)) === 0)
	xivo_die('no-data');

$data = $list = array();

for($i = 0;$i < $nb;$i++)
{
	$ref = &$agents[$i];

	$data['id'] = $ref['afeatures']['id'];
	$data['firstname'] = $ref['afeatures']['firstname'];
	$data['lastname'] = $ref['afeatures']['lastname'];
	$data['number'] = $ref['afeatures']['number'];
	$data['passwd'] = $ref['afeatures']['passwd'];
	$data['context'] = $ref['afeatures']['context'];
	$data['commented'] = $ref['agent']['commented'];

	$list[] = $data;
}

if(($list = xivo_json::encode($list)) === false)
	xivo_die('Error/500');

if(isset($_QR['sum']) === true && $_QR['sum'] === md5($list))
	xivo_die('no-update');

header(xivo_json::get_header());
die($list);

?>
