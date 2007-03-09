<?php

$total = 0;

if(($dirs = $sounds->get_list_dirs_files()) !== false)
{
	$total = count($dirs);
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('key' => 'name'));
	usort($dirs,array(&$sort,'str_usort'));	
}

$_HTML->assign('pager',xivo_calc_page($page,20,$total));
$_HTML->assign('list',$dirs);

?>
