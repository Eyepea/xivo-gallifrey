<?php

$info['filename'] = '';
$info['category'] = $cat;

if(isset($_QR['fm_send'],$_QR['category']) === false
|| ($infos = $musiconhold->get_category($_QR['category'])) === false
|| ($fileuploaded = $musiconhold->get_upload('filename')) === false
|| ($info['category'] = $musiconhold->chk_value('category',$infos['cat']['category'])) === false
|| ($info['filename'] = $musiconhold->chk_value('filename',$fileuploaded['name'])) === false)
{
	if(isset($fileuploaded) === true && is_array($fileuploaded) === true)
		xivo_file::rm($fileuploaded['tmp_name']);
}
else
{
	$filename = xivo_file::joinpath($info['category'],$fileuploaded['name']);

	if($musiconhold->add_file($filename,$fileuploaded['tmp_name']) === true)
	{
		$param['cat'] = $info['category'];
		$_QRY->go($_HTML->url('service/ipbx/pbx_services/musiconhold'),$param);
	}
}

$_HTML->set_var('info',$info);
$_HTML->set_var('option',$musiconhold->get_option());

?>
