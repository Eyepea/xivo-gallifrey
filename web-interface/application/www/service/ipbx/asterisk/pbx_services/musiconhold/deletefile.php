<?php

$param['page'] = $page;

if(($infos = $musiconhold->get_category($cat)) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_services/musiconhold'),'act=list');

if(isset($_QR['id']) === false || ($info = $musiconhold->get_file($_QR['id'],$infos['cat']['category'])) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_services/musiconhold'),$param);

$file = $infos['cat']['category'].XIVO_SEP_DIR.$info['filename'];

$musiconhold->delete_file($file);

$_QRY->go($_HTML->url('service/ipbx/pbx_services/musiconhold'),$param);

?>
