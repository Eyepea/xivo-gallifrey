<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$dir = isset($_QR['dir']) === true ? strval($_QR['dir']) : '';
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
	case 'deletes':
	case 'list':
	case 'download':
		$action = $act;
		$param['dir'] = $dir;
		break;
	default:
		$_QRY->go($_HTML->url('service/ipbx'));
}

include(dirname(__FILE__).'/sounds/'.$action.'.php');

$_HTML->assign('act',$act);
$_HTML->assign('dir',$dir);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_services/sounds');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_services/sounds/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
