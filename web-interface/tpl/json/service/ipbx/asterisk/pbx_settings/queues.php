<?php

$queues = $this->get_var('queues');

if(is_array($queues) === false)
	xivo_die('Error/500');
else if(($nb = count($queues)) === 0)
	xivo_die('no-data');

$data = $list = array();

for($i = 0;$i < $nb;$i++)
{
	$ref = &$queues[$i];

	$data['category'] = $ref['queue']['category'];
	$data['name'] = $ref['queue']['name'];
	$data['number'] = $ref['qfeatures']['number'];
	$data['context'] = $ref['qfeatures']['context'];
	$data['commented'] = $ref['queue']['commented'];

	$list[] = $data;
}

if(($list = xivo_json::encode($list)) === false)
	xivo_die('Error/500');

if(isset($_QR['sum']) === true && $_QR['sum'] === md5($list))
	xivo_die('no-update');

header(xivo_json::get_header());
die($list);

?>
