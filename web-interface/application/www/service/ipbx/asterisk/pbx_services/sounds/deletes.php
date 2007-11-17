<?php

$param['page'] = $page;

$info = array();

if(($info['directory'] = $sounds->get_dir($dir)) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),'act=listdir');

if(($values = xivo_issa_val('files',$_QR)) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);

$dialstatus = &$ipbx->get_module('dialstatus');

$dialstatus_where = array();
$dialstatus_where['type'] = 'sound';

$nb = count($values);

for($i = 0;$i < $nb;$i++)
{
	if(($info['file'] = $sounds->get(strval($values[$i]),$info['directory']['dirname'])) === false)
		continue;

	$file = $info['directory']['dirname'].XIVO_SEP_DIR.$info['file']['filename'];
	
	if($sounds->delete($file) === false)
		continue;

	$dialstatus_where['typeval'] = $info['file']['dirpath'].XIVO_SEP_DIR.$info['file']['basename'];

	$dialstatus->unlinked_where($dialstatus_where);
}

$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);

?>
