<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;
$search = isset($_QR['search']) === true ? strval($_QR['search']) : '';
$context = isset($_QR['context']) === true ? strval($_QR['context']) : '';

$param = array();
$param['act'] = 'list';

if($search !== '')
	$param['search'] = $search;
else if($context !== '')
	$param['context'] = $context;

$ufeatures = &$ipbx->get_module('userfeatures');

if(($contexts = $ufeatures->get_all_context()) !== false)
	ksort($contexts);

switch($act)
{
	case 'add':
	case 'edit':
		include(dirname(__FILE__).'/users/'.$act.'.php');
		break;
	case 'delete':
		$param['page'] = $page;

		$appuser = &$ipbx->get_application('user');

		if(isset($_QR['id']) === false || $appuser->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

		$appuser->delete();

		$ipbx->discuss('xivo[userlist,update]');

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('users',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

		$appuser = &$ipbx->get_application('user');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appuser->get($values[$i]) === false)
				continue;

			$appuser->delete();
		}

		$ipbx->discuss('xivo[userlist,update]');

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('users',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

		$appuser = &$ipbx->get_application('user');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appuser->get($values[$i],null,false) === false)
				continue;

			if($act === 'disables')
				$appuser->disable();
			else
				$appuser->enable();
		}

		$ipbx->discuss('xivo[userlist,update]');

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);
		break;
	case 'import':
		$appuser = &$ipbx->get_application('user');

		if(isset($_QR['fm_send']) === true)
		{
			$appuser->import_csv();
			$ipbx->discuss('xivo[userlist,update]');
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);
		}

		$_HTML->set_var('import_file',$appuser->get_config_import_file());
		break;
	case 'list':
	default:
		$act = 'list';
		$total = 0;

		if($search !== '')
			$users = $ipbx->get_users_search($search);
		else if($context !== '')
			$users = $ipbx->get_users_context($context);
		else
			$users = $ipbx->get_users_list();

		if($users !== false)
		{
			$total = count($users);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'ufeatures','key' => 'fullname'));
			usort($users,array(&$sort,'str_usort'));
		}

		$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
		$_HTML->set_var('list',$users);
		$_HTML->set_var('search',$search);
		$_HTML->set_var('context',$context);
}

$_HTML->set_var('act',$act);
$_HTML->set_var('contexts',$contexts);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_settings/users');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_settings/users/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
