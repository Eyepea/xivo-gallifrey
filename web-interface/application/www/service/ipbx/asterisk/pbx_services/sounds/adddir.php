<?php

$info = array();

if(isset($_QR['fm_send']) === true
&& ($info['dirname'] = $sounds->chk_value('dirname',$_QRY->get_qr('dirname'))) !== false)
{
	if($sounds->add_dir($info['dirname']) === true)
		$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);
}

$_HTML->assign('info',$info);

?>
