<?php

if(isset($_QR['id']) === false || ($info = $configfiles->get($_QR['id'])) === false)
	xivo_go($_HTML->url('service/ipbx/system_management/configfiles'),$param);

if(isset($_QR['fm_send'],$_QR['content']) === true)
{
	if($configfiles->edit($info['name'],$_QRY->get_uqr('content')) !== false)
		xivo_go($_HTML->url('service/ipbx/system_management/configfiles'),$param);

	$info['content'] = $_QR['content'];
}

$_HTML->assign('info',$info);

?>
