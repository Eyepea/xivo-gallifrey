<?php

$total = 0;
$dirs = false;

if($dir === '')
	$dir = false;

if(($dirs = $sounds->get_dir($dir)) === false)
	$_QRY->go($_HTML->url('service/ipbx/general_settings/sounds'),'act=listdir');

if(($dirs = $dirs['files']) !== false)
{
	$total = count($dirs);
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('key' => 'name'));
	usort($dirs,array(&$sort,'str_usort'));	
}

$_HTML->assign('pager',xivo_calc_page($page,20,$total));
$_HTML->assign('list',$dirs);
$_HTML->assign('dir',$dir);

?>
