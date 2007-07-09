<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$dir = isset($_QR['dir']) === true ? $_QR['dir'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$param = array();
$param['act'] = 'list';

$sounds = &$ipbx->get_module('sounds');

if(($list_dirs = $sounds->get_list_dirs()) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort();
	usort($list_dirs,array(&$sort,'str_usort'));
}

$_HTML->assign('list_dirs',$list_dirs);

switch($act)
{
	case 'adddir':
	case 'editdir':
	case 'deletedir':
	case 'listdir':
		$param['act'] = 'listdir';
		$action = $act;
		break;
	case 'add':
		if($list_dirs === false)
		{
			$param['act'] = $action = $act = 'listdir';
			break;
		}
	case 'edit':
	case 'delete':
	case 'list':
	case 'download':
		$action = $act;
		$param['dir'] = $dir;
		break;
	default:
		xivo_go($_HTML->url('service/ipbx'));
}

include(dirname(__FILE__).'/sounds/'.$action.'.php');

$_HTML->assign('act',$act);
$_HTML->assign('dir',$dir);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/general_settings/sounds');

$_HTML->assign('bloc','general_settings/sounds/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
