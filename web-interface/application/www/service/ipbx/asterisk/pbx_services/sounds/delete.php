<?php

$param['page'] = $page;

$info = array();

if(($info['directory'] = $sounds->get_dir($dir)) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),'act=listdir');

if(isset($_QR['id']) === false || ($info['file'] = $sounds->get($_QR['id'],$info['directory']['dirname'])) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);

$file = $info['directory']['dirname'].XIVO_SEP_DIR.$info['file']['filename'];

$sounds->delete($file);

$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);

?>
