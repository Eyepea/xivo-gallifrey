<?php

define('XIVO_TPL_SPACE','json');

require_once('xivo.php');

$ipbx = &$_SRE->get('ipbx');

$application = $_HTML->get_application('service/ipbx/'.$ipbx->get_name().'/ajs/',2);

if($application === false)
{
	$dhtml = &$_HTML->get_module('dhtml');
	$dhtml->ajs_die('Error/404');
}

die(include($application));

?>
