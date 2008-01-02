<?php

require_once('xivo.php');

if(xivo_user::chk_acl(true) === false)
	$_QRY->go($_HTML->url('xivo'));

$ipbx = &$_SRE->get('ipbx');

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_css('css/service/ipbx/'.$ipbx->get_name().'.css');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'.js');

$_HTML->load_i18n_file('struct/service/ipbx/'.$ipbx->get_name());

$application = $_HTML->get_application('service/ipbx/'.$ipbx->get_name(),2);

if($application === false)
	$_QRY->go($_HTML->url('xivo'));

die(include($application));

?>
