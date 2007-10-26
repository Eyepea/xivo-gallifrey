<?php

$param['page'] = $page;

$info = array();

if(($info['directory'] = $sounds->get_dir($dir)) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),'act=listdir');

if(isset($_QR['id']) === false || ($info['file'] = $sounds->get($_QR['id'],$info['directory']['dirname'])) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);

xivo::load_class('xivo_file');

$file = new xivo_file();

if(($file->download($info['file']['path'])) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);

die();

?>
