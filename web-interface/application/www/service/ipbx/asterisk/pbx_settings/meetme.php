<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$appmeetme = &$ipbx->get_application('meetme');

		$result = null;

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('meetmeroom',$_QR) === false
			|| xivo_issa('mfeatures',$_QR) === false)
				break;

			if($appmeetme->set_add($_QR) === false
			|| $appmeetme->add() === false)
			{
				$result = $appmeetme->get_result();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);
		}
		while(false);

		$_HTML->assign('info',$result);
		$_HTML->assign('moh_list',$appmeetme->get_musiconhold());
		$_HTML->assign('element',$appmeetme->get_elements());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'edit':
		$appmeetme = &$ipbx->get_application('meetme');

		if(isset($_QR['id']) === false || ($info = $appmeetme->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);

		$result = null;
		$return = &$info;

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('meetmeroom',$_QR) === false
			|| xivo_issa('mfeatures',$_QR) === false)
				break;

			$return = &$result;

			if($appmeetme->set_edit($_QR) === false
			|| $appmeetme->edit() === false)
			{
				$result = $appmeetme->get_result();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);
		}
		while(false);

		$_HTML->assign('id',$info['meetmeroom']['id']);
		$_HTML->assign('info',$return);
		$_HTML->assign('moh_list',$appmeetme->get_musiconhold());
		$_HTML->assign('element',$appmeetme->get_elements());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appmeetme = &$ipbx->get_application('meetme');

		if(isset($_QR['id']) === false || $appmeetme->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);

		$appmeetme->delete();

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('meetme',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);

		$appmeetme = &$ipbx->get_application('meetme');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appmeetme->get($values[$i]) === false)
				continue;

			$appmeetme->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('meetme',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);

		$appmeetme = &$ipbx->get_apprealstatic('meetme');
		$appmeetmeroom = &$appmeetme->get_module('room');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($act === 'disables')
				$appmeetmeroom->disable($values[$i]);
			else
				$appmeetmeroom->enable($values[$i]);
		}

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);
		break;
	default:
		$act = 'list';
		$total = 0;

		if(($list = $ipbx->get_meetme_list()) !== false)
		{
			$total = count($list);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'mfeatures','key' => 'name'));
			usort($list,array(&$sort,'str_usort'));
		}

		$_HTML->assign('pager',xivo_calc_page($page,20,$total));
		$_HTML->assign('list',$list);
}

$_HTML->assign('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_settings/meetme');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_settings/meetme/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
