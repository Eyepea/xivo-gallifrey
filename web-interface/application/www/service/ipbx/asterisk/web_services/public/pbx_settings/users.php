<?php

switch($_QRY->get_qs('act'))
{
	case 'add':
		$appuser = &$ipbx->get_application('user');
		$appuser->import_json();
		die();
		break;
	case 'list':
	default:
		$appuser = &$ipbx->get_application('user',null,false);

		if(($users = $appuser->get_users_list()) === false)
			xivo_die('no-data');

		$_HTML->set_var('users',$users);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/users');
}

?>
