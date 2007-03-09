<?php

require_once('xivo.php');

$cat = isset($_QR['cat']) === true ? $_QR['cat'] : '';

switch($cat)
{
	case 'edit':

		break;
	case 'view':
	default:
		$cat = 'view';

		break;
}
$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
//$menu->set_left('left/xivo/user');

$_HTML->assign('cat',$cat);
$_HTML->set_struct('xivo/user');
$_HTML->display('index');

?>
