<?php

$act = isset($_QR['act']) === true ? $_QR['act']  : '';

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'edit':
		if(isset($_QR['id']) === false || ($info = $_USR->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('xivo/configuration/users'),$param);

		if(isset($_QR['fm_send']) === true
		&& $_USR->edit($info['meta'],$_QR) === true)
		{
			if((int) $_USR->get_info('id') === (int) $_QR['id'])
				$_USR->load_by_id($_QR['id']);

			$_QRY->go($_HTML->url('xivo/configuration/users'),$param);
		}

		$_HTML->set_var('info',$info);
		break;
	case 'acl':
		if(isset($_QR['id']) === false || ($info = $_USR->get($_QR['id'])) === false
		|| xivo_user::chk_authorize('admin',$info['meta']) === false)
			$_QRY->go($_HTML->url('xivo/configuration/users'),$param);

		if(isset($_QR['fm_send']) === true)
		{
			$_USR->call_acl('edit',array($_QR));
			$_QRY->go($_HTML->url('xivo/configuration/users'),$param);
		}
		else if(($user_tree = $_USR->call_acl('get_user',array($info['id']))) !== false)
		{
			$_HTML->set_var('info',$info);
			$_HTML->set_var('tree',$user_tree);
		}
		else $_QRY->go($_HTML->url('xivo/configuration/users'),$param);
		break;
	default:
		$act = 'list';
		$_HTML->set_var('list',$_USR->get_list());
		break;
}

$_HTML->set_var('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');

$_HTML->set_bloc('main','xivo/configuration/users/'.$act);
$_HTML->set_struct('xivo/configuration');
$_HTML->display('index');

?>
