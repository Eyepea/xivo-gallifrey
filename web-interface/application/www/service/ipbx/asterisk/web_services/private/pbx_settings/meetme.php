<?php

include(dirname(__FILE__).'/../private.php');

$appmeetme = &$ipbx->get_application('meetme',null,false);

switch($_QRY->get_qs('act'))
{
	case 'list':
	default:
		if(($meetme = $appmeetme->get_meetme_list()) === false)
		{
			xivo::load_class('xivo_http');
			$http = new xivo_http();
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('meetme',$meetme);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/meetme');
}

?>
