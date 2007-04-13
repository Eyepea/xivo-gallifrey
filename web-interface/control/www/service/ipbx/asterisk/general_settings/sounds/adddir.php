<?php

$info = array();

if(isset($_QR['fm_send']) === true
&& ($info['dirname'] = $sounds->chk_value('dirname',$_QRY->get_qr('dirname'))) !== false)
{
	if($sounds->add_dir($info['dirname']) === true)
		xivo_go($_HTML->url('service/ipbx/general_settings/sounds'),$param);
}

$_HTML->assign('info',$info);

?>
