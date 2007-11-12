<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$element = $info = $result = array();

$param = array();
$param['act'] = 'list';

$trunkiax = &$ipbx->get_module('trunkiax');
$tfeatures = &$ipbx->get_module('trunkfeatures');

switch($act)
{
	case 'add':
	case 'edit':
		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/trunkiax.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
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
		$_QRY->go($_HTML->url('service/ipbx'));
}

include(dirname(__FILE__).'/iax/'.$action.'.php');

$_HTML->set_var('act',$act);
$_HTML->set_var('element',$element);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/trunk_management/iax');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/trunk_management/iax/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
