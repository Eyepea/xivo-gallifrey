<?php

$queues = $this->get_var('queues');

if(is_array($queues) === false)
	xivo_die('Error/500');
else if(($nb = count($queues)) === 0)
	xivo_die('no-data');

$data = array();

for($i = 0;$i < $nb;$i++)
{
	$ref = &$queues[$i];

	$data['category'] = $ref['queue']['category'];
	$data['name'] = $ref['queue']['name'];
	$data['number'] = $ref['qfeatures']['number'];
	$data['context'] = $ref['qfeatures']['context'];
	$data['commented'] = $ref['queue']['commented'];
}

if(($data = xivo_json::encode($data)) === false)
	xivo_die('Error/500');

if(isset($_QR['sum']) === true && $_QR['sum'] === md5($data))
	xivo_die('no-update');

header(xivo_json::get_header());
die($data);

?>
