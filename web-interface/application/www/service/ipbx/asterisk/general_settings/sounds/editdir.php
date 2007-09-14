<?php

$info = array();

if(isset($_QR['id']) === false || ($info = $sounds->get_dir($_QR['id'])) === false)
	$_QRY->go($_HTML->url('service/ipbx/general_settings/sounds'),$param);

$id = $info['dirname'];

if(isset($_QR['fm_send']) === true
&& ($info['dirname'] = $sounds->chk_value('dirname',$_QRY->get_qr('dirname'))) !== false)
{
	if($sounds->edit_dir($id,$info['dirname']) === true)
		$_QRY->go($_HTML->url('service/ipbx/general_settings/sounds'),$param);
}

$_HTML->assign('info',$info);
$_HTML->assign('id',$id);

?>
