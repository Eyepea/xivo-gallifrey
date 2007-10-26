<?php

do
{
	if(isset($_QR['fm_send']) === false)
		break;

	unset($_QR['filename']);

	if(($result = $musiconhold->chk_values($_QR)) === false
	|| ($result['mode'] === 'custom' && (string) $result['application'] === '') === true)
	{
		$info = $musiconhold->get_filter_result();
		break;
	}

	if($musiconhold->add_category($result) !== false)
		$_QRY->go($_HTML->url('service/ipbx/pbx_services/musiconhold'),$param);
}
while(false);

$element = $musiconhold->get_element();

?>
