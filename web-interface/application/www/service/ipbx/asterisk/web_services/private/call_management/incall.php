<?php

include(dirname(__FILE__).'/../private.php');

$appincall = &$ipbx->get_application('incall',null,false);

switch($_QRY->get_qs('act'))
{
	case 'list':
	default:
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
