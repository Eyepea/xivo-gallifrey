<?php

$meetme = $this->get_var('meetme');

if(is_array($meetme) === false)
	xivo_die('Error/500');
else if(($nb = count($meetme)) === 0)
	xivo_die('no-data');

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
	xivo_die('Error/500');

if($this->get_var('sum') === md5($list))
	xivo_die('no-update');

header(xivo_json::get_header());
die($list);

?>
