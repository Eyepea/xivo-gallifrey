<?php

require_once('xivo.php');

if(xivo_user::chk_authorize('root') === false)
	$_QRY->go($_HTML->url('xivo'));

$cat = isset($_QR['cat']) === true ? $_QR['cat'] : '';

$param = array('cat' => 'list');

switch($cat)
{
	case 'edit':
		if(isset($_QR['id']) === false || ($info = $_USR->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('xivo/configuration'),$param);

		if(isset($_QR['fm_send']) === true)
		{
			if($_USR->edit($info['meta'],$_QR) === true)
			{
				if($_USR->get_info('id') === $_QR['id'])
					$_USR->load_by_id($_QR['id']);

				$_QRY->go($_HTML->url('xivo/configuration'),$param);
			}
			else
				$_HTML->assign('info',$info);
		}
		else
			$_HTML->assign('info',$info);
		break;
	case 'acl':
		if(isset($_QR['id']) === false || ($info = $_USR->get($_QR['id'])) === false
		|| xivo_user::chk_authorize('admin',$info['meta']) === false)
			$_QRY->go($_HTML->url('xivo/configuration'),$param);

		if(isset($_QR['fm_send']) === true)
		{
			$_USR->call_acl('edit',array($_QR));
			$_QRY->go($_HTML->url('xivo/configuration'),$param);
		}
		else if(($user_tree = $_USR->call_acl('get_user',array($info['id']))) !== false)
		{
			$_HTML->assign('info',$info);
			$_HTML->assign('tree',$user_tree);
		}
		else $_QRY->go($_HTML->url('xivo/configuration'),$param);
		break;
	default:
	case 'list':
		$cat = 'list';
		$_HTML->assign('list',$_USR->get_list());
		break;
//	default:
//		$cat = 'index';
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');

$_HTML->assign('cat',$cat);
$_HTML->set_struct('xivo/configuration');
$_HTML->display('index');

?>
