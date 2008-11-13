<?php

$appagent = &$ipbx->get_application('agent',null,false);

switch($_QRY->get_qs('act'))
{
	case 'list':
	default:
		if(($agents = $appagent->get_agents_list()) === false)
			xivo_die('no-data');

		$_HTML->set_var('agents',$agents);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/agents');
}

?>
