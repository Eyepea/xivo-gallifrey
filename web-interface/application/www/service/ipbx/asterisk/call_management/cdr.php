<?php

$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;
$act = isset($_QR['act']) === false || $_QR['act'] !== 'exportcsv' ? 'search' : 'exportcsv';

$cdr = &$ipbx->get_module('cdr');
$element = $cdr->get_element();

$total = 0;
$nb_page = 30;

$info = null;
$result = false;

if(isset($_QR['fm_send']) === true || isset($_QR['search']) === true)
{
	if(($info = $cdr->chk_values($_QR,false)) === false)
		$info = $cdr->get_filter_result();
	else
	{
		if($act === 'exportcsv')
			$limit = null;
		else
		{
			$limit = array();
			$limit[0] = ($page - 1) * $nb_page;
			$limit[1] = $nb_page;
		}

		if(($result = $cdr->search($info,'calldate',$limit)) !== false && $result !== null)
			$total = $cdr->get_cnt();

		if($result === false)
			$info = $result = null;

		$_HTML->assign('pager',xivo_calc_page($page,$nb_page,$total));
	}

	if($act === 'exportcsv' && $info !== null)
		$info['amaflagsmeta'] = $info['amaflags'] !== '' ? $cdr->amaflags_meta($info['amaflags']) : '';
}

$_HTML->assign('total',$total);
$_HTML->assign('element',$element);
$_HTML->assign('info',$info);
$_HTML->assign('result',$result);
$_HTML->assign('act',$act);

if($act === 'exportcsv')
{
	if($result === false)
		xivo_go($_HTML->url('service/ipbx/call_management/cdr'));

	$_HTML->display('/bloc/service/ipbx/'.$ipbx->get_name().'/call_management/cdr/exportcsv',false,true);
	die();
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/call_management/cdr');

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$_HTML->assign('bloc','call_management/cdr/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
