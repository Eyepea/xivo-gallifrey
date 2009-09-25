<?php

$monitoring = &$_SRE->get('monitoring');

if(isset($_QR['service'],$_QR['action']) === true)
{
	switch($_QR['action'])
	{
		case 'start':
			$monitoring->start_service($_QR['service']);
			$_QRY->go($_TPL->url('xivo'));
			break;
		case 'stop':
			$monitoring->stop_service($_QR['service']);
			$_QRY->go($_TPL->url('xivo'));
			break;
		case 'restart':
			$monitoring->restart_service($_QR['service']);
			$_QRY->go($_TPL->url('xivo'));
			break;
		case 'monitor':
			$monitoring->enable_monitor($_QR['service']);
			$_QRY->go($_TPL->url('xivo'));
			break;
		case 'unmonitor':
			$monitoring->disable_monitor($_QR['service']);
			$_QRY->go($_TPL->url('xivo'));
			break;
		default:
	}
}

$mon_telephony = $monitoring->get_group('telephony');
$mon_grpundef = $monitoring->get_group_undefined();
$devinfo = $monitoring->get_device();

$_SYSINFO = new dwho_sysinfo();

dwho::load_class('dwho_sort');
$sort = new dwho_sort(array('key' => 'name'));

if(is_array($mon_telephony) === true)
	usort($mon_telephony,array(&$sort,'strnat_usort'));

if(is_array($mon_grpundef) === true)
	usort($mon_grpundef,array(&$sort,'strnat_usort'));

if(is_array($devinfo) === true)
	usort($devinfo,array(&$sort,'strnat_usort'));

$_TPL->set_var('sysinfo',$monitoring->get_system());
$_TPL->set_var('uptime',$_SYSINFO->uptime());
$_TPL->set_var('cpuinfo',$_SYSINFO->cpuinfo());
$_TPL->set_var('devinfo',$devinfo);
$_TPL->set_var('meminfo',$_SYSINFO->meminfo(true));
$_TPL->set_var('netinfo',$_SYSINFO->netinfo());
$_TPL->set_var('mon_telephony',$mon_telephony);
$_TPL->set_var('mon_grpundef',$mon_grpundef);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));

$_TPL->set_bloc('main','xivo/index');
$_TPL->set_struct('xivo/index');
$_TPL->display('simple');

?>
