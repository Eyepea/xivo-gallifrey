<?php

require_once('xivo.php');

$cat = isset($_QR['cat']) === true ? $_QR['cat'] : '';

if(xivo_user::chk_authorize('root') === false)
	xivo_go($_HTML->url('xivo'));

switch($cat)
{
	case 'edit':
		if(isset($_QR['id']) === false || ($info = $_USR->get($_QR['id'])) === false)
			xivo_go($_HTML->url('xivo/configuration'),'cat=list');

		if(isset($_QR['fm_send']) === true)
		{
			if($_USR->edit($info['meta'],$_QR) === true)
			{
				if($_USR->get_infos('id') === $_QR['id'])
					$_USR->load_by_id($_QR['id']);

				xivo_go($_HTML->url('xivo/configuration'),'cat=list');
			}
			else
				$_HTML->assign('info',$info);
		}
		else
			$_HTML->assign('info',$info);
		break;
	case 'policy':
		if(isset($_QR['id']) === false || ($info = $_USR->get($_QR['id'])) === false
		|| xivo_user::chk_authorize('admin',$info['meta']) === false)
			xivo_go($_HTML->url('xivo/configuration'),'cat=list');

		if(isset($_QR['fm_send']) === true)
		{
			$_USR->call_policy('edit',array($_QR));
			xivo_go($_HTML->url('xivo/configuration'),'cat=list');
		}
		else if(($user_tree = $_USR->call_policy('get_user',array($info['id']))) !== false)
		{
			$_HTML->assign('info',$info);
			$_HTML->assign('tree',$user_tree);
		}
		else xivo_go($_HTML->url('xivo/configuration'),'cat=list');

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->add_js('xivo/configuration/policy/policy.js.php','foot');
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
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/xivo/configuration');

$_HTML->assign('cat',$cat);
$_HTML->set_struct('xivo/configuration');
$_HTML->display('index');

?>
