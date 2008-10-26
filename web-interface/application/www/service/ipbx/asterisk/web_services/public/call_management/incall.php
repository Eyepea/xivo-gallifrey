<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';

$appincall = &$ipbx->get_application('incall',null,false);

switch($act)
{
	case 'list':
	default:
		if(($incall = $appincall->get_incalls_list()) === false)
			xivo_die('no-data');

		$_HTML->set_var('incall',$incall);
}

$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/call_management/incall');

?>
