<?php

$total = 0;

if(($files = $configfiles->get_list()) !== false)
{
	$total = count($files);
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort();
	usort($files,array(&$sort,'str_usort'));
}

$_HTML->assign('pager',xivo_calc_page($page,20,$total));
$_HTML->assign('list',$files);

?>
