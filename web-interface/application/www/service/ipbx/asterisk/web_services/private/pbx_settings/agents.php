<?php

include(dirname(__FILE__).'/../private.php');

$appagent = &$ipbx->get_application('agent',null,false);

switch($_QRY->get_qs('act'))
{
	case 'list':
	default:
		if(($agents = $appagent->get_agents_list()) === false)
		{
			xivo::load_class('xivo_http');
			$http = new xivo_http();
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('agents',$agents);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/agents');
}

?>
