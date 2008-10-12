<?php

$_SERVER['REMOTE_ADDR'] = '127.0.0.1';

if(isset($_SERVER['REMOTE_ADDR']) === false
|| $_SERVER['REMOTE_ADDR'] !== '127.0.0.1')
	xivo_die('Error/403');

$protocols = array(XIVO_SRE_IPBX_AST_PROTO_SIP,
		   XIVO_SRE_IPBX_AST_PROTO_IAX);

$appuser = &$ipbx->get_application('user',null,false);

if(($users = $appuser->get_users_list($protocols)) === false)
	xivo_die('no-data');

$_HTML->set_var('users',$users);

$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/users');

?>
