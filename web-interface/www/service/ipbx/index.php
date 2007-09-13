<?php

require_once('xivo.php');

if($_HTML->chk_acl(true) === false)
	xivo_go($_HTML->url('xivo'));

$ipbx = &$_SRE->get('ipbx');

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_css('css/service/ipbx/'.$ipbx->get_name().'.css');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'.js');

$_HTML->load_i18n_file('struct/service/ipbx/'.$ipbx->get_name());

$application = $_HTML->get_application('service/ipbx/'.$ipbx->get_name(),2);

if($application !== false)
	die(include($application));

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/index');
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
