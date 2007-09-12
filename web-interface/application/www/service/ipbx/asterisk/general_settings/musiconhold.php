<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$cat = isset($_QR['cat']) === true ? strval($_QR['cat']) : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$element = $info = $result = array();

$param = array();
$param['act'] = 'list';

$musiconhold = &$ipbx->get_module('musiconhold');

if(($list_cats = $musiconhold->get_all_by_category()) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('key' => 'category'));
	usort($list_cats,array(&$sort,'str_usort'));
}

$_HTML->assign('list_cats',$list_cats);

switch($act)
{
	case 'add':
	case 'edit':
		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/musiconhold.js');
	case 'delete':
	case 'deletes':
	case 'list':
		$action = $act;
		break;
	case 'addfile':
		if($list_cats === false)
		{
			$action = $act = 'list';
			break;
		}
	case 'editfile':
	case 'listfile':
	case 'deletefile':
	case 'download':
		$action = $act;
		$param['act'] = 'listfile';
		$param['cat'] = $cat;
		break;
	case 'enables':
	case 'disables':
		$action = 'enables';
		break;
	default:
		xivo_go($_HTML->url('service/ipbx'));
}

include(dirname(__FILE__).'/musiconhold/'.$action.'.php');

$_HTML->assign('act',$act);
$_HTML->assign('cat',$cat);
$_HTML->assign('element',$element);
$_HTML->assign('info',$info);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/general_settings/musiconhold');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/general_settings/musiconhold/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
