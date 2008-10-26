<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';

$appgroup = &$ipbx->get_application('group',null,false);

switch($act)
{
	case 'list':
	default:
		if(($groups = $appgroup->get_groups_list()) === false)
			xivo_die('no-data');

		$_HTML->set_var('groups',$groups);
}

$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/groups');

?>
