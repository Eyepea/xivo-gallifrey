<?php

if(isset($_GET['mac']) === true
&& preg_match('/^[A-F0-9]{12}$/',strval($_GET['mac']),$match) === 1)
	$macaddr = $match[0];
else
	$macaddr = '';

if(isset($_SERVER['HTTP_USER_AGENT']) === false
|| preg_match('/^(snom3[026]0)/',$_SERVER['HTTP_USER_AGENT'],$match) !== 1)
	snom_get_config('snom',$macaddr);
else
	snom_get_config($match[1],$macaddr);

die();

function snom_get_config($type,$macaddr)
{
	if(isset($macaddr{0}) === true)
		$filename = $type.'-'.$macaddr.'.htm';
	else
		$filename = $type.'.htm';

	if(is_file($filename) === true)
		include($filename);
}

?>
