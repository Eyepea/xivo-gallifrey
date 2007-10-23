<?php

$param['page'] = $page;

$info = array();

if(($info['directory'] = $sounds->get_dir($dir)) === false)
	$_QRY->go($_HTML->url('service/ipbx/general_settings/sounds'),'act=listdir');

if(($values = xivo_issa_val('files',$_QR)) === false)
	$_QRY->go($_HTML->url('service/ipbx/general_settings/sounds'),$param);

$nb = count($values);

for($i = 0;$i < $nb;$i++)
{
	if(($info['file'] = $sounds->get(strval($values[$i]),$info['directory']['dirname'])) === false)
		continue;

	$file = $info['directory']['dirname'].XIVO_SEP_DIR.$info['file']['filename'];
	
	$sounds->delete($file);
}

$_QRY->go($_HTML->url('service/ipbx/general_settings/sounds'),$param);

?>
