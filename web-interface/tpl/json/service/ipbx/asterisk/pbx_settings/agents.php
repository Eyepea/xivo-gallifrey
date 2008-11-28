<?php

xivo::load_class('xivo_http');
$http = new xivo_http();

$agents = $this->get_var('agents');

if(is_array($agents) === false)
{
	$http->set_status(500);
	$http->send(true);
}
else if(($nb = count($agents)) === 0)
{
	$http->set_status(204);
	$http->send(true);
}

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
	$data['language'] = $ref['afeatures']['language'];
	$data['silent'] = $ref['afeatures']['silent'];
	$data['ackcall'] = $ref['agentoptions']['ackcall'];
	$data['endcall'] = $ref['agentoptions']['endcall'];
	$data['autologoff'] = $ref['agentoptions']['autologoff'];
	$data['autologoffunavail'] = $ref['agentoptions']['autologoffunavail'];
	$data['wrapuptime'] = $ref['agentoptions']['wrapuptime'];
	$data['maxlogintries'] = $ref['agentoptions']['maxlogintries'];
	$data['updatecdr'] = $ref['agentoptions']['updatecdr'];
	$data['recordagentcalls'] = $ref['agentoptions']['recordagentcalls'];
	$data['recordformat'] = $ref['agentoptions']['recordformat'];
	$data['urlprefix'] = $ref['agentoptions']['urlprefix'];
	$data['beep'] = $ref['agentoptions']['custom_beep'];
	$data['goodbye'] = $ref['agentoptions']['goodbye'];
	$data['commented'] = $ref['agent']['commented'];

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
