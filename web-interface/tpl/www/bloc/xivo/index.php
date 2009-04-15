<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

$sysinfo = $this->get_var('sysinfo');
$cpuinfo = $this->get_var('cpuinfo');
$meminfo = $this->get_var('meminfo');
$netinfo = $this->get_var('netinfo');
$telephony = $this->get_var('telephony');
$grpundef = $this->get_var('grpundef');

$memtotal = xivo_size_iec($meminfo['memtotal']);
$memfree = xivo_size_iec($meminfo['memfree']);
$memused = xivo_size_iec($meminfo['memused']);

if($meminfo['memtotal'] > 0):
	$mempercent = ($meminfo['memused'] / $meminfo['memtotal'] * 100);
else:
	$mempercent = 0;
endif;

$swaptotal = xivo_size_iec($meminfo['swaptotal']);
$swapfree = xivo_size_iec($meminfo['swapfree']);
$swapused = xivo_size_iec($meminfo['swapused']);

if($meminfo['swaptotal'] > 0):
	$swappercent = ($meminfo['swapused'] / $meminfo['swaptotal'] * 100);
else:
	$swappercent = 0;
endif;

$this->set_var('memtotal',$meminfo['memtotal']);

$cputotalpercent = 0;
$load = $cputotal = $cpuuser = $cpusystem = $cpuwait = '-';

if(xivo_issa('system',$sysinfo) === true):
	if(xivo_issa('load',$sysinfo['system']) === true):
		$load = vsprintf('%.2f %.2f %.2f',$sysinfo['system']['load']);
	endif;

	if(xivo_issa('cpu',$sysinfo['system']) === true):
		$cputotalpercent = array_sum($sysinfo['system']['cpu']);
		$cputotal = $this->bbf('number_percent',$cputotalpercent);
		$cpuuser = $this->bbf('number_percent',$sysinfo['system']['cpu']['user']);
		$cpusystem = $this->bbf('number_percent',$sysinfo['system']['cpu']['system']);
		$cpuwait = $this->bbf('number_percent',$sysinfo['system']['cpu']['wait']);
	endif;
endif;

?>
<div id="system-infos" class="b-infos">
	<h3 class="sb-top xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center"><?=$this->bbf('title_content_name');?></span>
		<span class="span-right">&nbsp;</span>
	</h3>
	<div class="sb-content sb-list">
		<div id="sysinfo">
			<table border="0" cellpadding="0" cellspacing="0">
				<tr class="sb-top">
					<th colspan="2" class="th-left th-right"><?=$this->bbf('sysinfos_system');?></th>
				</tr>
				<tr class="l-infos-1on2">
					<td class="td-left txt-left"><?=$this->bbf('sysinfos_servername');?></td>
					<td class="td-right txt-right"><?=php_uname('n');?></td>
				</tr>
				<tr class="l-infos-2on2">
					<td class="td-left txt-left"><?=$this->bbf('sysinfos_os');?></td>
					<td class="td-right txt-right"><?=php_uname('s');?></td>
				</tr>
				<tr class="l-infos-1on2">
					<td class="td-left txt-left"><?=$this->bbf('sysinfos_kernel');?></td>
					<td class="td-right txt-right"><?=php_uname('r');?></td>
				</tr>
				<tr class="l-infos-2on2">
					<td class="td-left txt-left"><?=$this->bbf('sysinfos_ipaddr');?></td>
					<td class="td-right txt-right"><?=$_SERVER['SERVER_ADDR']?></td>
				</tr>
				<tr class="l-infos-1on2">
					<td class="td-left txt-left"><?=$this->bbf('sysinfos_dnsaddr');?></td>
					<td class="td-right txt-right"><?=gethostbyaddr($_SERVER['SERVER_ADDR']);?></td>
				</tr>
				<tr class="l-infos-2on2">
					<td class="td-left txt-left"><?=$this->bbf('sysinfos_uptime');?></td>
					<td class="td-right txt-right"><?=$this->bbf('sysinfos_uptime-duration',
										     xivo_calc_time('second',
										     		    $this->get_var('uptime'),
												    '%d%H%M%s'));?></td>
				</tr>
				<tr class="l-infos-1on2">
					<td class="td-left txt-left"><?=$this->bbf('sysinfos_loadaverage');?></td>
					<td class="td-right txt-right"><?=$load?></td>
				</tr>
			</table>
		</div>
		<div id="cpuinfo">
			<table border="0" cellpadding="0" cellspacing="0">
				<tr class="sb-top">
					<th colspan="5" class="th-left th-right"><?=$this->bbf('sysinfos_cpu');?></th>
				</tr>
				<tr class="l-subth">
					<td colspan="2"><?=$this->bbf('sysinfos_col_percent');?></td>
					<td><?=$this->bbf('sysinfos_col_user');?></td>
					<td><?=$this->bbf('sysinfos_col_system');?></td>
					<td class="td-right"><?=$this->bbf('sysinfos_col_wait');?></td>
				</tr>
				<tr class="l-infos-1on2">
					<td class="gauge">
						<div><div style="width: <?=round($cputotalpercent);?>px;">&nbsp;</div></div>
					</td>
					<td class="gaugepercent txt-right"><?=$cputotal?></td>
					<td class="txt-right"><?=$cpuuser?></td>
					<td class="txt-right"><?=$cpusystem?></td>
					<td class="td-right txt-right"><?=$cpuwait?></td>
				</tr>
			</table>
		</div>
		<div id="meminfo">
			<table border="0" cellpadding="0" cellspacing="0">
				<tr class="sb-top">
					<th colspan="6" class="th-left th-right"><?=$this->bbf('sysinfos_memory');?></th>
				</tr>
				<tr class="l-subth">
					<td><?=$this->bbf('sysinfos_col_type');?></td>
					<td colspan="2"><?=$this->bbf('sysinfos_col_percent');?></td>
					<td><?=$this->bbf('sysinfos_col_free');?></td>
					<td><?=$this->bbf('sysinfos_col_used');?></td>
					<td class="td-right"><?=$this->bbf('sysinfos_col_total');?></td>
				</tr>
				<tr class="l-infos-1on2">
					<td><?=$this->bbf('sysinfos_physical-memory');?></td>
					<td class="gauge">
						<div><div style="width: <?=round($mempercent);?>px;">&nbsp;</div></div>
					</td>
					<td class="gaugepercent txt-right"><?=$this->bbf('number_percent',$mempercent);?></td>
					<td class="txt-right"><?=$this->bbf('size_iec_'.$memfree[1],$memfree[0]);?></td>
					<td class="txt-right"><?=$this->bbf('size_iec_'.$memused[1],$memused[0]);?></td>
					<td class="td-right txt-right"><?=$this->bbf('size_iec_'.$memtotal[1],$memtotal[0]);?></td>
				</tr>
				<tr class="l-infos-2on2">
					<td><?=$this->bbf('sysinfos_swap-partition');?></td>
					<td class="gauge">
						<div><div style="width: <?=round($swappercent);?>px;">&nbsp;</div></div>
					</td>
					<td class="gaugepercent txt-right"><?=$this->bbf('number_percent',$swappercent);?></td>
					<td class="txt-right"><?=$this->bbf('size_iec_'.$swapfree[1],$swapfree[0]);?></td>
					<td class="txt-right"><?=$this->bbf('size_iec_'.$swapused[1],$swapused[0]);?></td>
					<td class="td-right txt-right"><?=$this->bbf('size_iec_'.$swaptotal[1],$swaptotal[0]);?></td>
				</tr>
			</table>
		</div>
		<div id="netinfo">
			<table border="0" cellpadding="0" cellspacing="0">
				<tr class="sb-top">
					<th colspan="5" class="th-left th-right"><?=$this->bbf('sysinfos_network');?></th>
				</tr>
				<tr class="l-subth">
					<td><?=$this->bbf('sysinfos_col_interface');?></td>
					<td><?=$this->bbf('sysinfos_col_receive');?></td>
					<td><?=$this->bbf('sysinfos_col_transmit');?></td>
					<td><?=$this->bbf('sysinfos_col_error');?></td>
					<td class="td-right"><?=$this->bbf('sysinfos_col_drop');?></td>
				</tr>
<?php
		if(is_array($netinfo) === true):
			$i = 0;
			foreach($netinfo as $devname => $stats):
				$rx_bytes = xivo_size_iec($stats['rx']['bytes']);
				$tx_bytes = xivo_size_iec($stats['tx']['bytes']);
				$total_errs = xivo_size_iec($stats['total']['errs']);
				$total_drop = xivo_size_iec($stats['total']['drop']);
?>
				<tr class="l-infos-<?=(($i++ % 2) + 1)?>on2">
					<td><?=xivo_trunc(xivo_htmlen($devname),20,'...',false);?></td>
					<td class="txt-right"><?=$this->bbf('size_iec_'.$rx_bytes[1],$rx_bytes[0]);?></td>
					<td class="txt-right"><?=$this->bbf('size_iec_'.$tx_bytes[1],$tx_bytes[0]);?></td>
					<td class="txt-right"><?=$stats['total']['errs']?></td>
					<td class="td-right txt-right"><?=$stats['total']['drop']?></td>
				</tr>
<?php
			endforeach;
		else:
?>
				<tr class="l-infos-1on2">
					<td colspan="5" class="td-single"><?=$this->bbf('sysinfos_no-netinfo');?></td>
				</tr>
<?php
		endif;
?>
			</table>
		</div>
		<div class="clearboth"></div>
<?php
	$this->file_include('bloc/xivo/monitoring/group',
			    array('group_name'	=> 'mon_server'));

	$this->file_include('bloc/xivo/monitoring/group',
			    array('group_name'	=> 'mon_telephony'));

	$this->file_include('bloc/xivo/monitoring/group',
			    array('group_name'	=> 'mon_grpundef'));
?>
	</div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>
