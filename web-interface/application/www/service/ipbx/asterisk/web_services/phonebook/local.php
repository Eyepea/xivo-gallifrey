<?php

$phonebook = &$ipbx->get_appcustom('webservices','phonebook');

if(isset($_SERVER['REMOTE_ADDR']) === false
|| $phonebook->chk_host_access($_SERVER['REMOTE_ADDR']) === false)
	xivo_die('Error/403');

define('XIVO_PHONEBOOK_URL',$_HTML->url('service/ipbx/web_services/phonebook/search',true));

$vendor = isset($_QR['vendor']) === true ? $phonebook->chk_vendor($_QR['vendor']) : false;

if($vendor === false)
	xivo_die('Error/Invalid Vendor');

if(isset($_QR['name']) === false || xivo_haslen($_QR['name']) === false)
	xivo_die('Error/Invalid name');

$rs = array();

if(($rsp = $phonebook->get_phonebook_search($_QR['name'])) !== false)
	$rs = $rsp;

if(($rsu = $phonebook->get_user_search($_QR['name'],false)) !== false)
	$rs = array_merge($rs,$rsu);

if(($nb = count($rs)) === 0)
	xivo_die('no-data');

echo xivo_msg('beg-data')."\n";

for($i = 0;$i < $nb;$i++)
{
	$ref = &$rs[$i];

	echo	'"'.str_replace('"','""',$ref['name']).'"|'.
		'"'.str_replace('"','""',$ref['phone']).'"|'.
		'"'.str_replace('"','""',$ref['type']).'"'."\n";
}

xivo_die('end-data')."\n";

?>
