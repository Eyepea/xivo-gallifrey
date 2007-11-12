<?php

$total = 0;

if(($files = $configfiles->get_list()) !== false)
{
	$total = count($files);
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort();
	usort($files,array(&$sort,'str_usort'));
}

$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
$_HTML->set_var('list',$files);

?>
