<?php

$appgroup = &$ipbx->get_application('group',null,false);

switch($_QRY->get_qs('act'))
{
	case 'list':
	default:
		if(($groups = $appgroup->get_groups_list()) === false)
			xivo_die('no-data');

		$_HTML->set_var('groups',$groups);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/groups');
}

?>
