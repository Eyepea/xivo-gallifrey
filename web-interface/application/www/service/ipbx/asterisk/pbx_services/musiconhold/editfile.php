<?php

$param['page'] = $page;

if(($infos = $musiconhold->get_category($cat)) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_services/musiconhold'),'act=list');

$cat = $info['category'] = $infos['cat']['category'];

if(isset($_QR['id']) === false || ($info['file'] = $musiconhold->get_file($_QR['id'],$infos['cat']['category'])) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_services/musiconhold'),$param);

$info['filename'] = $info['file']['basename'];
$id = $info['file']['filename'];

do
{
	if(isset($_QR['fm_send'],$_QR['filename'],$_QR['category']) === false
	|| ($infos = $musiconhold->get_category($_QR['category'])) === false)
		break;

	$info['category'] = $infos['cat']['category'];
	$info['filename'] = strval($_QR['filename']).'.'.$info['file']['extension'];

	if(($info['category'] = $musiconhold->chk_value('category',$info['category'])) === false
	|| ($info['filename'] = $musiconhold->chk_value('filename',$info['filename'])) === false)
		break;

	$filename = xivo_file::joinpath($info['file']['dirname'],$info['file']['filename']);
	$newfilename = xivo_file::joinpath($info['category'],$info['filename']);

	if($musiconhold->edit_file($filename,$newfilename) === true)
		$_QRY->go($_HTML->url('service/ipbx/pbx_services/musiconhold'),$param);

	$info['filename'] = $info['file']['basename'];
}
while(false);

$_HTML->set_var('id',$id);
$_HTML->set_var('info',$info);

?>
