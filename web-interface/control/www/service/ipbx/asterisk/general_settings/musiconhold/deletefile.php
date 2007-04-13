<?php

$param['page'] = $page;

if(($infos = $musiconhold->get_category($cat)) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/musiconhold'),'act=list');

if(isset($_QR['id']) === false || ($info = $musiconhold->get_file($_QR['id'],$infos['cat']['category'])) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/musiconhold'),$param);

$file = $infos['cat']['category'].XIVO_SEP_DIR.$info['filename'];

$musiconhold->delete_file($file);

xivo_go($_HTML->url('service/ipbx/general_settings/musiconhold'),$param);

?>
