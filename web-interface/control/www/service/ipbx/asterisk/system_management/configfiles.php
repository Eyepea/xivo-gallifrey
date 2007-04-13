<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$element = $info = $result = array();

$param = array();
$param['act'] = 'list';

$configfiles = &$ipbx->get_module('configfiles');

switch($act)
{
	case 'edit':
	case 'list':
		$action = $act;
		break;
	default:
		xivo_go($_HTML->url('service/ipbx'));
}

include(dirname(__FILE__).'/configfiles/'.$action.'.php');

$_HTML->assign('act',$act);
$_HTML->assign('element',$element);
$_HTML->assign('info',$info);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');

$_HTML->assign('bloc','system_management/configfiles/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
