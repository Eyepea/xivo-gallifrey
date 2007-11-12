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
		$_QRY->go($_HTML->url('service/ipbx'));
}

include(dirname(__FILE__).'/configfiles/'.$action.'.php');

$_HTML->set_var('act',$act);
$_HTML->set_var('element',$element);
$_HTML->set_var('info',$info);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/system_management/configfiles/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
