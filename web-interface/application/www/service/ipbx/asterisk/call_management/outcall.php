<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$outcall = &$ipbx->get_module('outcall');
$extenumbers = &$ipbx->get_module('extenumbers');

$info = $result = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':

		break;
	case 'edit':

		break;
	case 'delete':

		break;
	case 'deletes':

		break;
	case 'disables':
	case 'enables':

		break;
	default:
		$act = 'list';
		$total = 0;
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/call_management/outcall');

$_HTML->assign('act',$act);
$_HTML->assign('bloc','call_management/outcall/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
