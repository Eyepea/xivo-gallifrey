<?php

xivo::load_class('xivo_accesswebservice',XIVO_PATH_OBJECT,null,false);
$_AWS = new xivo_accesswebservice();

$access = $_AWS->chk_http_access('pbx_settings','users');

include(dirname(__FILE__).'/../restricted.php');

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
		{
			xivo::load_class('xivo_http');
			$http = new xivo_http();
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('users',$users);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/users');
}

?>
