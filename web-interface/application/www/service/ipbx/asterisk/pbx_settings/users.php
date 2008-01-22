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

$contexts = false;

switch($act)
{
	case 'add':
	case 'edit':
		$appuser = &$ipbx->get_application('user');
		$contexts = $appuser->get_all_context();

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

		$appuser = &$ipbx->get_application('user',null,false);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appuser->get($values[$i]) === false)
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
		$contexts = $appuser->get_all_context();

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
		$appuser = &$ipbx->get_application('user');
		$contexts = $appuser->get_all_context();

		$act = 'list';
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$order = array();
		$order['firstname'] = SORT_ASC;
		$order['lastname'] = SORT_ASC;

		$limit = array();
		$limit[0] = ($page - 1) * $nbbypage;
		$limit[1] = $nbbypage;

		if($search !== '')
			$list = $appuser->get_users_search($search,null,$order,$limit);
		else if($context !== '')
			$list = $appuser->get_users_context($context,null,$order,$limit);
		else
			$list = $appuser->get_users_list(null,null,$order,$limit);

		$total = $appuser->get_cnt();

		if($list === false && $total > 0)
		{
			$param['page'] = $page - 1;
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
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
