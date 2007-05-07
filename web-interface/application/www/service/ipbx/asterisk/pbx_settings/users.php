<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$ract = isset($_QR['ract']) === true ? $_QR['ract'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;
$search = isset($_QR['search']) === true ? $_QR['search'] : '';
$context = isset($_QR['context']) === true ? $_QR['context'] : '';

$param = array();
$param['act'] = 'list';

if($ract === 'search' && $search !== '')
{
	$param['act'] = 'search';
	$param['search'] = $search;
}

$ufeatures = &$ipbx->get_module('userfeatures');

if(($contexts = $ufeatures->get_all_context()) !== false)
	ksort($contexts);

switch($act)
{
	case 'add':
	case 'edit':
	case 'delete':
	case 'deletes':
	case 'search':
	case 'list':
	case 'context':
		$action = $act;
		break;
	case 'enables':
	case 'disables':
		$action = 'enables';
		break;
	default:
		xivo_go($_HTML->url('service/ipbx'));
}

include(dirname(__FILE__).'/users/'.$action.'.php');

$_HTML->assign('act',$act);
$_HTML->assign('contexts',$contexts);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/pbx_settings/users');

$_HTML->assign('bloc','pbx_settings/users/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
