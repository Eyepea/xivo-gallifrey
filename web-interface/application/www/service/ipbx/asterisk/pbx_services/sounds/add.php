<?php

$info = array();

$info['filename'] = '';
$info['dirname'] = $dir;

$option = $sounds->get_option();

do
{
	if(isset($_QR['fm_send'],$_QR['dirname']) === false
	|| ($info['directory'] = $sounds->get_dir($_QR['dirname'])) === false)
		break;

	$file = new xivo_file();

	if($file->get_upload('filename',$option['file']) === false)
		break;

	$info['dirname'] = $info['directory']['dirname'];

	if(($info['dirname'] = $sounds->chk_value('dirname',$info['dirname'])) === false
	|| ($info['filename'] = $sounds->chk_value('filename',$file->info['name'])) === false)
	{
		xivo_file::rm($file->info['tmp_name']);
		break;
	}

	$filename = $info['dirname'].XIVO_SEP_DIR.$file->info['name'];

	if($sounds->add($filename,$file->info['tmp_name']) === true)
	{
		$param['dir'] = $info['dirname'];
		$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);
	}
}
while(false);
	
$_HTML->assign('info',$info);
$_HTML->assign('option',$option);

?>
