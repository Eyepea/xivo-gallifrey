<?php

$act = isset($_QR['act']) === true ? $_QR['act']  : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'edit':
		if(isset($_QR['id']) === false
		|| ($info = $_USR->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/user'),$param);

		if(isset($_QR['fm_send']) === true
		&& $_USR->edit($info['meta'],$_QR) !== false)
		{
			if(xivo_ulongint($_USR->get_info('id')) === xivo_ulongint($info['id']))
				$_USR->load_by_id($info['id']);

			$_QRY->go($_HTML->url('xivo/configuration/manage/user'),$param);
		}

		$_HTML->set_var('info',$info);
		break;
	case 'acl':
		if(isset($_QR['id']) === false
		|| ($info = $_USR->get($_QR['id'])) === false
		|| xivo_user::chk_authorize('admin',$info['meta']) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/user'),$param);

		$user_acl = $_USR->get_acl();

		if(isset($_QR['fm_send']) === true)
		{
			$user_acl->edit($_QR);
			$_QRY->go($_HTML->url('xivo/configuration/manage/user'),$param);
		}
		else if(($tree = $user_acl->get_access_tree($info['id'])) !== false)
		{
			$_HTML->set_var('info',$info);
			$_HTML->set_var('tree',$tree);
		}
		else $_QRY->go($_HTML->url('xivo/configuration/manage/user'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = 20;

		$order = array();
		$order['login'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $_USR->get_all(null,true,$order,$limit);
		$total = $_USR->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_HTML->url('xivo/configuration/manage/user'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
		break;
}

$_HTML->set_var('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');

$_HTML->set_bloc('main','xivo/configuration/manage/user/'.$act);
$_HTML->set_struct('xivo/configuration');
$_HTML->display('index');

?>
