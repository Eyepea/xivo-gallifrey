<?php

$appmeetme = &$ipbx->get_application('meetme',null,false);

switch($_QRY->get_qs('act'))
{
	case 'list':
	default:
		if(($meetme = $appmeetme->get_meetme_list()) === false)
			xivo_die('no-data');

		$_HTML->set_var('meetme',$meetme);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/meetme');
}

?>
