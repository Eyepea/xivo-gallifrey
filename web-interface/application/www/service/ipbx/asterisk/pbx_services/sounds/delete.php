<?php

$param['page'] = $page;

$info = array();

if(($info['directory'] = $sounds->get_dir($dir)) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),'act=listdir');

if(isset($_QR['id']) === false || ($info['file'] = $sounds->get($_QR['id'],$info['directory']['dirname'])) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);

$file = $info['directory']['dirname'].XIVO_SEP_DIR.$info['file']['filename'];

if($sounds->delete($file) !== false && ($dialstatus = &$ipbx->get_module('dialstatus')) !== false)
{
	$dialstatus_where = array();
	$dialstatus_where['type'] = 'sound';
	$dialstatus_where['typeval'] = $info['file']['dirpath'].XIVO_SEP_DIR.$info['file']['basename'];

	$dialstatus->unlinked_where($dialstatus_where);
}

$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);

?>
