<?php

$phonebook = &$ipbx->get_appcustom('webservices','phonebook');

if(isset($_SERVER['REMOTE_ADDR']) === false
|| $phonebook->chk_host_access($_SERVER['REMOTE_ADDR']) === false)
	xivo_die('Error/403');

$vendor = isset($_QR['vendor']) === true ? $phonebook->chk_vendor($_QR['vendor']) : false;

if($vendor === false && ($vendor = $phonebook->get_vendor_from_useragent()) === false)
	xivo_die('Error/Invalid Vendor and User-Agent');

$bloc = '/bloc/service/ipbx/'.$ipbx->get_name().'/web_services/phonebook';

if(isset($_QR['name']) === false || xivo_haslen($_QR['name']) === false)
{
	if($vendor === 'snom')
		$bloc .= '/snom/input';
	else
		$bloc .= '/'.$vendor.'/directory';

	$_HTML->display($bloc);
	die();
}

$rs = array();

if(($rsp = $phonebook->get_phonebook_search($_QR['name'])) !== false)
	$rs = $rsp;

if(($rsu = $phonebook->get_user_search($_QR['name'],null,false)) !== false)
	$rs = array_merge($rs,$rsu);

$url = $_HTML->url('service/ipbx/web_services/phonebook/local').
       '?'.'name='.$_QR['name'].'&vendor='.$vendor;

if(($rss = $phonebook->get_phonebook_search_from_server($url,false)) !== false)
	$rs = array_merge($rs,$rsu,$rss);

xivo::load_class('xivo::sort');
$sort = new xivo_sort(array('key' => 'identity'));
usort($rs,array(&$sort,'str_usort'));

$_HTML->assign('list',$rs);
$_HTML->display($bloc.'/'.$vendor.'/directory');
die();

?>
