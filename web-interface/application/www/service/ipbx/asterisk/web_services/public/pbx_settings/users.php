<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';

switch($act)
{
	case 'list':
	default:
		$protocols = array(XIVO_SRE_IPBX_AST_PROTO_SIP,
				   XIVO_SRE_IPBX_AST_PROTO_IAX);

		$appuser = &$ipbx->get_application('user',null,false);

		if(($users = $appuser->get_users_list($protocols)) === false)
			xivo_die('no-data');

		$_HTML->set_var('users',$users);
}

$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/users');

?>
