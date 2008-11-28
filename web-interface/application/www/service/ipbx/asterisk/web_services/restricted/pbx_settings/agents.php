<?php

xivo::load_class('xivo_accesswebservice',XIVO_PATH_OBJECT,null,false);
$_AWS = new xivo_accesswebservice();

$access = $_AWS->chk_http_access('pbx_settings','agents');

include(dirname(__FILE__).'/../restricted.php');

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
