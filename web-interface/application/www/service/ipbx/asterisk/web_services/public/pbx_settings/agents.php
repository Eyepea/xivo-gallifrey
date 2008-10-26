<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';

$appagent = &$ipbx->get_application('agent',null,false);

switch($act)
{
	case 'list':
	default:
		if(($agents = $appagent->get_agents_list()) === false)
			xivo_die('no-data');

		$_HTML->set_var('agents',$agents);
}

$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/agents');

?>
