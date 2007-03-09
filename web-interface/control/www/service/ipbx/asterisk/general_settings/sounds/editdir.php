<?php

$info = array();

if(isset($_QR['id']) === false || ($info = $sounds->get_dir($_QR['id'])) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/sounds'),'act=listdir');

$id = $info['dirname'];

if(isset($_QR['fm_send']) === true
&& ($info['dirname'] = $sounds->chk_value('dirname',$_QRY->get_qr('dirname'))) !== false)
{
	if($sounds->edit_dir($id,$info['dirname']) === true)
		xivo_go($_HTML->url('service/ipbx/general_settings/sounds'),'act=listdir');
}

$_HTML->assign('info',$info);
$_HTML->assign('id',$id);

?>
