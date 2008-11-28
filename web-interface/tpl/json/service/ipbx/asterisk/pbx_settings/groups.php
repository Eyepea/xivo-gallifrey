<?php

xivo::load_class('xivo_http');
$http = new xivo_http();

$groups = $this->get_var('groups');

if(is_array($groups) === false)
{
	$http->set_status(500);
	$http->send(true);
}
else if(($nb = count($groups)) === 0)
{
	$http->set_status(204);
	$http->send(true);
}

$data = $list = array();

for($i = 0;$i < $nb;$i++)
{
	$ref = &$groups[$i];

	$data['id'] = $ref['gfeatures']['id'];
	$data['category'] = $ref['queue']['category'];
	$data['name'] = $ref['queue']['name'];
	$data['number'] = $ref['gfeatures']['number'];
	$data['context'] = $ref['gfeatures']['context'];
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
