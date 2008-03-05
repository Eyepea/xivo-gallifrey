<?php

require_once('xivo.php');

if(xivo_user::chk_authorize('root') === false)
	$_QRY->go($_HTML->url('xivo'));

$application = $_HTML->get_application('xivo/configuration',2);

if($application === false)
	$_QRY->go($_HTML->url('xivo'));

die(include($application));

?>
