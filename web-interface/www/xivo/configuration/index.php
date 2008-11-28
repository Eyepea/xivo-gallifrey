<?php

require_once('xivo.php');

if(xivo_user::chk_authorize('root') === false)
	$_QRY->go($_HTML->url('xivo'));

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_css('css/xivo/configuration.css');

$application = $_HTML->get_application('xivo/configuration',2);

if($application === false)
	$_QRY->go($_HTML->url('xivo'));

die(include($application));

?>
