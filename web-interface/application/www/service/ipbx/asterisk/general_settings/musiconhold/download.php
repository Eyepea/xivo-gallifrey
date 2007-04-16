<?php

$param['page'] = $page;

if(($infos = $musiconhold->get_category($cat)) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/musiconhold'),'act=list');

if(isset($_QR['id']) === false || ($info = $musiconhold->get_file($_QR['id'],$infos['cat']['category'])) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/musiconhold'),$param);

xivo::load_class('xivo_file');

$file = new xivo_file();

if(($file->download($info['path'])) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/musiconhold'),$param);

die();

?>
