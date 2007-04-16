<?php

$param['page'] = $page;

$info = array();

if(($info['directory'] = $sounds->get_dir($dir)) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/sounds'),'act=listdir');

if(isset($_QR['id']) === false || ($info['file'] = $sounds->get($_QR['id'],$info['directory']['dirname'])) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/sounds'),$param);

xivo::load_class('xivo_file');

$file = new xivo_file();

if(($file->download($info['file']['path'])) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/sounds'),$param);

die();

?>
