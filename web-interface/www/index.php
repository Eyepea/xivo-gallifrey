<?php

require_once('xivo.php');

if(xivo_user::is_valid() === true)
	$_QRY->go($_HTML->url('xivo'));

$_LANG = &xivo_gat::load_get('language',XIVO_PATH_OBJECTCONF);

if(isset($_QR['language'],$_LANG[$_QR['language']]) === true)
	$language = $_QR['language'];
else
	$language = '';

if(isset($_QR['login'],$_QR['password']) === true
&& $_USR->load_by_authent($_QR['login'],$_QR['password'],$language) === true)
	$_QRY->go($_HTML->url('xivo'));

$_HTML->set_var('language',xivo_array_intersect_key($_LANG,xivo_i18n::get_language_translated_list()));
$_HTML->set_struct('home/login');
$_HTML->display('center');

?>
