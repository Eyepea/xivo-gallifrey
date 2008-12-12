<?php

xivo::load_class('xivo_accesswebservice',XIVO_PATH_OBJECT,null,false);
$_AWS = new xivo_accesswebservice();

$access = $_AWS->chk_http_access('call_management','incall');

include(dirname(__FILE__).'/../restricted.php');

switch($_QRY->get_qs('act'))
{
	case 'add':
		$appincall = &$ipbx->get_application('incall');
		$status = $appincall->import_json() === true ? 201 : 400;

		$http = new xivo_http();
		$http->set_status($status);
		$http->send(true);
		break;
	case 'list':
	default:
		$appincall = &$ipbx->get_application('incall',null,false);

		if(($incall = $appincall->get_incalls_list()) === false)
		{
			xivo::load_class('xivo_http');
			$http = new xivo_http();
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('incall',$incall);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/call_management/incall');
}

?>
