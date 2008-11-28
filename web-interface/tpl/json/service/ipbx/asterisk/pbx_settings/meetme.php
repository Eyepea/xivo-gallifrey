<?php

xivo::load_class('xivo_http');
$http = new xivo_http();

$meetme = $this->get_var('meetme');

if(is_array($meetme) === false)
{
	$http->set_status(500);
	$http->send(true);
}
else if(($nb = count($meetme)) === 0)
{
	$http->set_status(204);
	$http->send(true);
}

$data = $list = array();

for($i = 0;$i < $nb;$i++)
{
	$ref = &$meetme[$i];

	$data['id'] = $ref['mfeatures']['id'];
	$data['name'] = $ref['mfeatures']['name'];
	$data['number'] = $ref['mfeatures']['number'];
	$data['pin'] = $ref['meetmeroom']['pin'];
	$data['admin-pin'] = $ref['meetmeroom']['admin-pin'];
	$data['musiconhold'] = $ref['mfeatures']['musiconhold'];
	$data['context'] = $ref['mfeatures']['context'];
	$data['commented'] = $ref['meetmeroom']['commented'];

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
