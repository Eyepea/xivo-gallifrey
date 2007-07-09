<?php

require_once('xivo.php');

if($_HTML->chk_policy(true) === false)
	xivo_go($_HTML->url('xivo'));

$ipbx = &$_SRE->get('ipbx');
$service_name = $ipbx->get_name();

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_css('css/service/ipbx/'.$service_name.'.css');
$dhtml->set_js('js/service/ipbx/'.$service_name.'.js');

$_HTML->load_i18n_file('struct/service/ipbx/'.$service_name);

$application = $_HTML->get_application('service/ipbx/'.$service_name,2);

if($application !== false)
	die(include($application));

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/'.$service_name);

$_HTML->assign('service_name',$service_name);
$_HTML->assign('bloc','index');
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
