<?php

include(dirname(__FILE__).'/../private.php');

$appvoicemail = &$ipbx->get_application('voicemail',null,false);

switch($_QRY->get_qs('act'))
{
	case 'list':
	default:
		if(($voicemail = $appvoicemail->get_voicemail_list()) === false)
		{
			xivo::load_class('xivo_http');
			$http = new xivo_http();
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('voicemail',$voicemail);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/voicemail');
}

?>
