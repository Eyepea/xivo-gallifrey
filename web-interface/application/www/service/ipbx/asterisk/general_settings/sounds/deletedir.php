<?php

$param['page'] = $page;

$info = array();

if(isset($_QR['id']) === false || ($info = $sounds->get_dir($_QR['id'])) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/sounds'),$param);

$sounds->delete_dir($info['dirname']);

xivo_go($_HTML->url('service/ipbx/general_settings/sounds'),$param);

?>
