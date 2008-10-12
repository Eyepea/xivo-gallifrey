<?php

$_SERVER['REMOTE_ADDR'] = '127.0.0.1';

if(isset($_SERVER['REMOTE_ADDR']) === false
|| $_SERVER['REMOTE_ADDR'] !== '127.0.0.1')
	xivo_die('Error/403');

$appgroup = &$ipbx->get_application('group',null,false);

if(($groups = $appgroup->get_groups_list()) === false)
	xivo_die('no-data');

$_HTML->set_var('groups',$groups);

$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/groups');

?>
