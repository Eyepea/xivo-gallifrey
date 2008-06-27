<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$callback = isset($_QR['callback']) === true ? $_QR['callback'] : '';

$dhtml = &$_HTML->get_module('dhtml');

switch($act)
{
	case 'view':
		if(xivo_user::chk_acl('pbx_settings','users') === false)
			$dhtml->ajs_die('Error/403');

		$appvoicemail = &$ipbx->get_application('voicemail');

		if(isset($_QR['id']) === false
		|| ($info = $appvoicemail->get($_QR['id'])) === false)
			$dhtml->ajs_die('Error/404');
		else if(($callback = $dhtml->chk_function_name($callback)) === false)
			$dhtml->ajs_die('Error/Invalid callback function');

		$_HTML->set_var('info',$info);
		$_HTML->set_var('callback',$callback);
		break;
	default:
		$dhtml->ajs_die('Error/404');
}

$json = &$_HTML->get_module('json');
$json->display();

?>
