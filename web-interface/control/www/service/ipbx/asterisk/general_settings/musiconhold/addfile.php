<?php

$info['filename'] = '';
$info['category'] = $cat;

$option = $musiconhold->get_option();

do
{
	if(isset($_QR['fm_send'],$_QR['category']) === false
	|| ($infos = $musiconhold->get_category($_QR['category'])) === false)
		break;

	xivo::load_class('xivo_file');

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
		xivo_go($_HTML->url('service/ipbx/general_settings/musiconhold'),'act=listfile&cat='.$info['category']);
}
while(false);

$_HTML->assign('info',$info);
$_HTML->assign('option',$option);

?>
