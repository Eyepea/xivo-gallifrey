<?php

$info = array();

if(($info['directory'] = $sounds->get_dir($dir)) === false)
	$_QRY->go($_HTML->url('service/ipbx/general_settings/sounds'),'act=listdir');

$dir = $info['dirname'] = $info['directory']['dirname'];

if(isset($_QR['id']) === false || ($info['file'] = $sounds->get($_QR['id'],$info['dirname'])) === false)
	$_QRY->go($_HTML->url('service/ipbx/general_settings/sounds'),$param);

$info['filename'] = $info['file']['basename'];
$id = $info['file']['filename'];

do
{
	if(isset($_QR['fm_send'],$_QR['filename'],$_QR['dirname']) === false
	|| ($info['directory'] = $sounds->get_dir($_QR['dirname'])) === false)
		break;

	$info['dirname'] = $info['directory']['dirname'];
	$info['filename'] = strval($_QR['filename']).'.'.$info['file']['extension'];

	if(($info['dirname'] = $sounds->chk_value('dirname',$info['dirname'])) === false
	|| ($info['filename'] = $sounds->chk_value('filename',$info['filename'])) === false)
		break;

	$filename = $info['file']['dirname'].XIVO_SEP_DIR.$info['file']['filename'];
	$newfilename = $info['dirname'].XIVO_SEP_DIR.$info['filename'];

	if($sounds->edit($filename,$newfilename) === true)
	{
		$param['dir'] = $info['dirname'];
		$_QRY->go($_HTML->url('service/ipbx/general_settings/sounds'),$param);
	}

	$info['filename'] = $info['file']['basename'];
}
while(false);

$_HTML->assign('id',$id);
$_HTML->assign('info',$info);

?>
