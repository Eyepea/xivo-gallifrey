<?php

$total = 0;

if(($peers = $trunksip->get_all()) !== false)
{
	$total = count($peers);
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('key' => 'callerid'));
	usort($peers,array(&$sort,'str_usort'));
}

$_HTML->assign('pager',xivo_calc_page($page,20,$total));
$_HTML->assign('list',$peers);

?>
