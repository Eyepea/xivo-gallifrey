<?php

$info = array();

if(($info['directory'] = $sounds->get_dir($dir)) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/sounds'),'act=listdir');

if(isset($_QR['id']) === false || ($info['file'] = $sounds->get($_QR['id'],$info['directory']['dirname'])) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/sounds'),'act=list&dir='.$info['directory']['dirname']);

$file = $info['directory']['dirname'].XIVO_SEP_DIR.$info['file']['filename'];

$sounds->delete($file);

xivo_go($_HTML->url('service/ipbx/general_settings/sounds'),'act=list&dir='.$info['directory']['dirname']);

?>
