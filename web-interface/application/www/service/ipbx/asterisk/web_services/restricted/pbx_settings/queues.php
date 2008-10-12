<?php

$_SERVER['REMOTE_ADDR'] = '127.0.0.1';

if(isset($_SERVER['REMOTE_ADDR']) === false
|| $_SERVER['REMOTE_ADDR'] !== '127.0.0.1')
	xivo_die('Error/403');

$appqueue = &$ipbx->get_application('queue',null,false);

if(($queues = $appqueue->get_queues_list()) === false)
	xivo_die('no-data');

$_HTML->set_var('queues',$queues);

$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/queues');

?>
