<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';

$appqueue = &$ipbx->get_application('queue',null,false);

switch($act)
{
	case 'list':
	default:
		if(($queues = $appqueue->get_queues_list()) === false)
			xivo_die('no-data');

		$_HTML->set_var('queues',$queues);
}

$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/queues');

?>
