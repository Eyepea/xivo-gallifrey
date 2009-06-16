<?php

$monitoring = &$_SRE->get('monitoring');

if(isset($_QR['service'],$_QR['action']) === true)
{
	switch($_QR['action'])
	{
		case 'start':
			$monitoring->start_service($_QR['service']);
			$_QRY->go($_HTML->url('xivo'));
			break;
		case 'stop':
			$monitoring->stop_service($_QR['service']);
			$_QRY->go($_HTML->url('xivo'));
			break;
		case 'restart':
			$monitoring->restart_service($_QR['service']);
			$_QRY->go($_HTML->url('xivo'));
			break;
		case 'monitor':
			$monitoring->enable_monitor($_QR['service']);
			$_QRY->go($_HTML->url('xivo'));
			break;
		case 'unmonitor':
			$monitoring->disable_monitor($_QR['service']);
			$_QRY->go($_HTML->url('xivo'));
			break;
		default:
	}
}

$mon_telephony = $monitoring->get_group('telephony');
$mon_grpundef = $monitoring->get_group_undefined();
$devinfo = $monitoring->get_device();

$_SYSINFO = new xivo_sysinfo();

xivo::load_class('xivo_sort');
$sort = new xivo_sort(array('key' => 'name'));

if(is_array($mon_telephony) === true)
	usort($mon_telephony,array(&$sort,'strnat_usort'));

if(is_array($mon_grpundef) === true)
	usort($mon_grpundef,array(&$sort,'strnat_usort'));

if(is_array($devinfo) === true)
	usort($devinfo,array(&$sort,'strnat_usort'));

$_HTML->set_var('sysinfo',$monitoring->get_system());
$_HTML->set_var('uptime',$_SYSINFO->uptime());
$_HTML->set_var('cpuinfo',$_SYSINFO->cpuinfo());
$_HTML->set_var('devinfo',$devinfo);
$_HTML->set_var('meminfo',$_SYSINFO->meminfo(true));
$_HTML->set_var('netinfo',$_SYSINFO->netinfo());
$_HTML->set_var('mon_telephony',$mon_telephony);
$_HTML->set_var('mon_grpundef',$mon_grpundef);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));

$_HTML->set_bloc('main','xivo/index');
$_HTML->set_struct('xivo/index');
$_HTML->display('simple');

?>
