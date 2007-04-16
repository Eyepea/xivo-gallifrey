<?php

$param['page'] = $page;

if(isset($_QR['id']) === false
|| ($infos = $musiconhold->get_category($_QR['id'],null)) === false)
	xivo_go($_HTML->url('service/ipbx/general_settings/musiconhold'),$param);

if($musiconhold->delete_category($infos['cat']['category']) !== false)
{
	$generalsip = &$ipbx->get_module('generalsip');
	$generalsip->edit_where(array('var_name' => 'musicclass','var_val' => $infos['cat']['category']),array('commented' => true,'var_val' => ''));

	$ufeatures = &$ipbx->get_module('userfeatures');
	$ufeatures->edit_where(array('musiconhold' => $infos['cat']['category']),array('musiconhold' => ''));

	$mfeatures = &$ipbx->get_module('meetmefeatures');
	$mfeatures->edit_where(array('musiconhold' => $infos['cat']['category']),array('musiconhold' => ''));
}

xivo_go($_HTML->url('service/ipbx/general_settings/musiconhold'),$param);

?>
