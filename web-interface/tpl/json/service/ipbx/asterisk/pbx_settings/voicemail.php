<?php

xivo::load_class('xivo_http');
$http = new xivo_http();

$voicemail = $this->get_var('voicemail');

if(is_array($voicemail) === false)
{
	$http->set_status(500);
	$http->send(true);
}
else if(($nb = count($voicemail)) === 0)
{
	$http->set_status(204);
	$http->send(true);
}

$data = $list = array();

for($i = 0;$i < $nb;$i++)
{
	$ref = &$voicemail[$i];

	$data['id'] = $ref['uniqueid'];
	$data['fullname'] = $ref['fullname'];
	$data['mailbox'] = $ref['mailbox'];
	$data['password'] = $ref['password'];
	$data['email'] = $ref['email'];
	$data['context'] = $ref['context'];
	$data['commented'] = $ref['commented'];

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
