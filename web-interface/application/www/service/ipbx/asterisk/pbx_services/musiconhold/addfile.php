<?php

$info['filename'] = '';
$info['category'] = $cat;

$option = $musiconhold->get_option();

do
{
	if(isset($_QR['fm_send'],$_QR['category']) === false
	|| ($infos = $musiconhold->get_category($_QR['category'])) === false)
		break;

	$file = new xivo_file();

	if($file->get_upload('filename',$option['file']) === false)
		break;

	$info['category'] = $infos['cat']['category'];

	if(($info['category'] = $musiconhold->chk_value('category',$info['category'])) === false
	|| ($info['filename'] = $musiconhold->chk_value('filename',$file->info['name'])) === false)
	{
		xivo_file::rm($file->info['tmp_name']);
		break;
	}

	$filename = $info['category'].XIVO_SEP_DIR.$file->info['name'];

	if($musiconhold->add_file($filename,$file->info['tmp_name']) === true)
	{
		$param['cat'] = $info['category'];
		$_QRY->go($_HTML->url('service/ipbx/pbx_services/musiconhold'),$param);
	}
}
while(false);

$_HTML->set_var('info',$info);
$_HTML->set_var('option',$option);

?>
