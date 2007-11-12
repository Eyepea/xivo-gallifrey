<?php

$total = 0;

if(($peers = $trunkiax->get_all()) !== false)
{
	$total = count($peers);
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('key' => 'name'));
	usort($peers,array(&$sort,'str_usort'));
}

$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
$_HTML->set_var('list',$peers);

?>
