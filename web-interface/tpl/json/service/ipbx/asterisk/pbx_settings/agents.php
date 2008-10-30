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
	xivo_die('Error/500');

if(isset($_QR['sum']) === true && $_QR['sum'] === md5($list))
	xivo_die('no-update');

header(xivo_json::get_header());
die($list);

?>
