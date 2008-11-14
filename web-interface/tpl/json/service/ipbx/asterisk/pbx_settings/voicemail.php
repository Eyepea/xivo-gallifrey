<?php

$voicemail = $this->get_var('voicemail');

if(is_array($voicemail) === false)
	xivo_die('Error/500');
else if(($nb = count($voicemail)) === 0)
	xivo_die('no-data');

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
	xivo_die('Error/500');

if($this->get_var('sum') === md5($list))
	xivo_die('no-update');

header(xivo_json::get_header());
die($list);

?>
