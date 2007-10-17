<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;
$search = isset($_QR['search']) === true ? strval($_QR['search']) : '';

$info = array();

$param = array();
$param['act'] = 'list';

if($search !== '')
	$param['search'] = $search;

switch($act)
{
	case 'add':
		$appincall = &$ipbx->get_application('incall');

		$result = null;

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('incall',$_QR) === false
			|| xivo_issa('extenumbers',$_QR) === false)
				break;

			if($appincall->set_add($_QR) === false
			|| $appincall->add() === false)
			{
				$result = $appincall->get_result();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		}
		while(false);

		if(isset($result['incall']) === true)
		{
			$result['incall']['endcall'] = '';
			$result['incall']['user'] = '';
			$result['incall']['group'] = '';
			$result['incall']['queue'] = '';
			$result['incall']['meetme'] = '';
			$result['incall']['schedule'] = '';
			$result['incall']['application'] = '';
			$result['incall']['custom'] = '';

			if(isset($result['incall'][$result['incall']['type']]) === true)
				$result['incall'][$result['incall']['type']] = $result['incall']['typeval'];
		}

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/incall.js');

		$_HTML->assign('incall',$result['incall']);
		$_HTML->assign('extenumbers',$result['extenumbers']);
		$_HTML->assign('list',$appincall->get_destination_list());
		$_HTML->assign('element',$appincall->get_elements());
		break;
	case 'edit':
		$appincall = &$ipbx->get_application('incall');

		if(isset($_QR['id']) === false || ($info = $appincall->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);

		$result = null;
		$return = &$info;

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('incall',$_QR) === false
			|| xivo_issa('extenumbers',$_QR) === false)
				break;

			$return = &$result;

			if($appincall->set_edit($_QR) === false
			|| $appincall->edit() === false)
			{
				$result = $appincall->get_result();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		}
		while(false);

		if(isset($return['incall']) === true)
		{
			$return['incall']['endcall'] = '';
			$return['incall']['user'] = '';
			$return['incall']['group'] = '';
			$return['incall']['queue'] = '';
			$return['incall']['meetme'] = '';
			$return['incall']['schedule'] = '';
			$return['incall']['application'] = '';
			$return['incall']['custom'] = '';
			$return['incall']['linked'] = $info['incall']['linked'];

			if(isset($return['incall'][$return['incall']['type']]) === true)
				$return['incall'][$return['incall']['type']] = $return['incall']['typeval'];
		}

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/incall.js');

		$_HTML->assign('id',$info['incall']['id']);
		$_HTML->assign('incall',$return['incall']);
		$_HTML->assign('extenumbers',$return['extenumbers']);
		$_HTML->assign('list',$appincall->get_destination_list());
		$_HTML->assign('element',$appincall->get_elements());
		break;
	case 'delete':
		$param['page'] = $page;

		$appincall = &$ipbx->get_application('incall');

		if(isset($_QR['id']) === false || $appincall->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);

		$appincall->delete();

		$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('incalls',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);

		$appincall = &$ipbx->get_application('incall');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appincall->get($values[$i]) === false)
				continue;

			$appincall->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('incalls',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);

		$appincall = &$ipbx->get_application('incall');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appincall->get($values[$i]) === false)
				continue;

			if($act === 'disables')
				$appincall->disable();
			else
				$appincall->enable();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		break;
	default:
		$act = 'list';
		$total = 0;

		if($search !== '')
			$list = $ipbx->get_incall_search($search);
		else
			$list = $ipbx->get_incall_list();

		if($list !== false)
		{
			$total = count($list);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'extenumbers','key' => 'exten'));
			usort($list,array(&$sort,'str_usort'));
		}

		$_HTML->assign('pager',xivo_calc_page($page,20,$total));
		$_HTML->assign('list',$list);
		$_HTML->assign('search',$search);
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/incall');

$_HTML->assign('act',$act);
$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/incall/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
