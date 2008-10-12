<?php

$groups = $this->get_var('groups');

if(is_array($groups) === false)
	xivo_die('Error/500');
else if(($nb = count($groups)) === 0)
	xivo_die('no-data');

$data = array();

for($i = 0;$i < $nb;$i++)
{
	$ref = &$groups[$i];

	$data['category'] = $ref['queue']['category'];
	$data['name'] = $ref['queue']['name'];
	$data['number'] = $ref['gfeatures']['number'];
	$data['context'] = $ref['gfeatures']['context'];
	$data['commented'] = $ref['queue']['commented'];
}

if(($data = xivo_json::encode($data)) === false)
	xivo_die('Error/500');

if(isset($_QR['sum']) === true && $_QR['sum'] === md5($data))
	xivo_die('no-update');

header(xivo_json::get_header());
die($data);

?>
