<?php

xivo::load_class('xivo_http');
$http = new xivo_http();

$queues = $this->get_var('queues');

if(is_array($queues) === false)
{
	$http->set_status(500);
	$http->send(true);
}
else if(($nb = count($queues)) === 0)
{
	$http->set_status(204);
	$http->send(true);
}

$data = $list = array();

for($i = 0;$i < $nb;$i++)
{
	$ref = &$queues[$i];

	$data['id'] = $ref['qfeatures']['id'];
	$data['category'] = $ref['queue']['category'];
	$data['name'] = $ref['queue']['name'];
	$data['number'] = $ref['qfeatures']['number'];
	$data['context'] = $ref['qfeatures']['context'];
	$data['commented'] = $ref['queue']['commented'];

	$list[] = $data;
}

if(($list = xivo_json::encode($list)) === false)
{
	$http->set_status(500);
	$http->send(true);
}
else if($this->get_var('sum') === md5($list))
{
	$http->set_status(304);
	$http->send(true);
}

header(xivo_json::get_header());
die($list);

?>
