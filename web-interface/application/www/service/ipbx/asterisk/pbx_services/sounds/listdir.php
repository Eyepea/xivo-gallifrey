<?php

$total = 0;

if(($dirs = $sounds->get_list_dirs_files()) !== false)
{
	$total = count($dirs);
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('key' => 'dirname'));
	usort($dirs,array(&$sort,'str_usort'));	
}

$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
$_HTML->set_var('list',$dirs);

?>
