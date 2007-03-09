<?php

$total = 0;

if($list_cats !== false)
	$total = count($list_cats);

$_HTML->assign('pager',xivo_calc_page($page,20,$total));

?>
