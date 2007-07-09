<?php

if(isset($_QR['id']) === false
|| ($infos = $musiconhold->get_category($_QR['id'],null)) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/musiconhold'),$param);

$info = $infos['cat'];
$id = $info['category'];

do
{
	if(isset($_QR['fm_send']) === false)
		break;

	if(($result = $musiconhold->chk_values($_QR,true,true)) === false
	|| ($result['mode'] === 'custom' && (string) $result['application'] === '') === true)
	{
		$info = $musiconhold->get_filter_result();
		break;
	}

	if($musiconhold->edit_category($id,$result) !== false)
		xivo_go($_HTML->url('service/ipbx/general_settings/musiconhold'),$param);

} while(false);

$_HTML->assign('id',$id);
$element = $musiconhold->get_element();

?>
