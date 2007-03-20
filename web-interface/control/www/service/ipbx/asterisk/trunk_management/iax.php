<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$element = $info = $result = array();

$trunkiax = &$ipbx->get_module('trunkiax');
$tfeatures = &$ipbx->get_module('trunkfeatures');

switch($act)
{
	case 'add':
	case 'edit':
		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/trunkiax.js');
	case 'delete':
	case 'deletes':
	case 'list':
		$action = $act;
		break;
	case 'enables':
	case 'disables':
		$action = 'enables';
		break;
	default:
		xivo_go($_HTML->url('service/ipbx'));
}

include(dirname(__FILE__).'/iax/'.$action.'.php');

$_HTML->assign('act',$act);
$_HTML->assign('element',$element);
$_HTML->assign('info',$info);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/trunk_management/iax');

$_HTML->assign('bloc','trunk_management/iax/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
