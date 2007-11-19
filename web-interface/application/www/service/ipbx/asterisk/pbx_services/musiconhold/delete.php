<?php

$param['page'] = $page;

if(isset($_QR['id']) === false
|| ($infos = $musiconhold->get_category($_QR['id'],null)) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_services/musiconhold'),$param);

$musiconhold->delete_category($infos['cat']['category']);

$_QRY->go($_HTML->url('service/ipbx/pbx_services/musiconhold'),$param);

?>
