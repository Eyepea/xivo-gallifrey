<?php

$total = 0;

if($search === '')
	$users = $ipbx->get_users_list();
else
	$users = $ipbx->get_users_search($search);

if($users !== false)
{
	$total = count($users);
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('browse' => 'sort','key' => 'name'));
	usort($users,array(&$sort,'str_usort'));
}

$_HTML->assign('pager',xivo_calc_page($page,20,$total));
$_HTML->assign('list',$users);
$_HTML->assign('search',$search);
$_HTML->assign('ract',$act);

$act = 'list';

?>
