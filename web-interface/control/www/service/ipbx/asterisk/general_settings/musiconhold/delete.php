<?php

if(isset($_QR['id']) === false
|| ($infos = $musiconhold->get_category($_QR['id'],null)) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/musiconhold'),'act=list');

$musiconhold->delete_by_category($infos['cat']['category']);

xivo_go($_HTML->url('service/ipbx/general_settings/musiconhold'),'act=list');

?>
