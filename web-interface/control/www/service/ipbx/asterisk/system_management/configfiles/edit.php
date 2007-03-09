<?php

if(isset($_QR['id']) === false || ($info = $configfiles->get($_QR['id'])) === false)
	xivo_go($_HTML->url('service/ipbx/system_management/configfiles'),'act=list');

if(isset($_QR['fm_send'],$_QR['content']) === true)
{
	if($configfiles->edit($info['name'],$_QRY->get_uqr('content')) !== false)
		xivo_go($_HTML->url('service/ipbx/system_management/configfiles'),'act=list');

	$info['content'] = $_QR['content'];
}

$_HTML->assign('info',$info);

?>
