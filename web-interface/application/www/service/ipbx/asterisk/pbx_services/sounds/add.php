<?php

$info = array();

$info['filename'] = '';
$info['dirname'] = $dir;

if(isset($_QR['fm_send'],$_QR['dirname']) === false
|| ($info['directory'] = $sounds->get_dir($_QR['dirname'])) === false
|| ($fileuploaded = $sounds->get_upload('filename')) === false
|| ($info['dirname'] = $sounds->chk_value('dirname',$info['directory']['dirname'])) === false
|| ($info['filename'] = $sounds->chk_value('filename',$fileuploaded['name'])) === false)
{
	if(isset($fileuploaded) === true && is_array($fileuploaded) === true)
		xivo_file::rm($fileuploaded['tmp_name']);
}
else
{
	$filename = $info['dirname'].XIVO_SEP_DIR.$fileuploaded['name'];

	if($sounds->add($filename,$fileuploaded['tmp_name']) === true)
	{
		$param['dir'] = $info['dirname'];
		$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);
	}
}

$_HTML->set_var('info',$info);
$_HTML->set_var('option',$sounds->get_option());

?>
