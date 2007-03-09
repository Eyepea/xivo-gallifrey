<?php

$total = 0;
$cats = false;

if($cat === '')
	$cat = false;

if(($infos = $musiconhold->get_category($cat)) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/musiconhold'),'act=listdir');

if(($files = $infos['dir']['files']) !== false)
{
	$total = count($files);
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('key' => 'name'));
	usort($files,array(&$sort,'str_usort'));	
}

$_HTML->assign('pager',xivo_calc_page($page,20,$total));
$_HTML->assign('list',$files);

?>
