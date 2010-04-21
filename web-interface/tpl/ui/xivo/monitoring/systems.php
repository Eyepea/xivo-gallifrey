<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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

$telephony = $this->get_var('telephony');
$grpundef = $this->get_var('grpundef');

$sysinfo = $this->get_var('sysinfo');
$devstats = $this->get_var('devstats');
$cpustats = $this->get_var('cpustats');
$netstats = $this->get_var('netstats');

$cputotalpercent = 0;
$load = $cputotal = $cpuuser = $cpusystem = $cpuwait = '-';

if(dwho_issa('system',$sysinfo) === true):
	if(dwho_issa('load',$sysinfo['system']) === true):
		$load = vsprintf('%.2f %.2f %.2f',$sysinfo['system']['load']);
	endif;

	if(dwho_issa('cpu',$sysinfo['system']) === true):
		$cputotalpercent = array_sum($sysinfo['system']['cpu']);
		$cputotal = $this->bbf('number_percent',$cputotalpercent);
		$cpuuser = $this->bbf('number_percent',$sysinfo['system']['cpu']['user']);
		$cpusystem = $this->bbf('number_percent',$sysinfo['system']['cpu']['system']);
		$cpuwait = $this->bbf('number_percent',$sysinfo['system']['cpu']['wait']);
	endif;
endif;

?>
<div id="leftinfo">
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
									     dwho_calc_time('second',
											    $this->get_var('uptime'),
											    '%d%H%M%s'));?></td>
			</tr>
			<tr class="l-infos-1on2">
				<td class="td-left txt-left"><?=$this->bbf('sysinfos_loadaverage');?></td>
				<td class="td-right txt-right"><?=$load?></td>
			</tr>
		</table>
	</div>
	<div id="devstats">
		<table border="0" cellpadding="0" cellspacing="0">
			<tr class="sb-top">
				<th colspan="6" class="th-left th-right"><?=$this->bbf('sysinfos_device');?></th>
			</tr>
			<tr class="l-subth">
				<td><?=$this->bbf('sysinfos_col_partition');?></td>
				<td colspan="2"><?=$this->bbf('sysinfos_col_percent');?></td>
				<td><?=$this->bbf('sysinfos_col_free');?></td>
				<td><?=$this->bbf('sysinfos_col_used');?></td>
				<td class="td-right"><?=$this->bbf('sysinfos_col_total');?></td>
			</tr>
<?php
	if(is_array($devstats) === true && ($nb = count($devstats)) > 0):
		for($i = 0;$i < $nb;$i++):
			$ref = &$devstats[$i]['block'];
			$total = $ref['total'] * $ref['size'];
			$free = $ref['free'] * $ref['size'];
			$used = $total - $free;
			$devtotal = dwho_size_iec($total);
			$devfree = dwho_size_iec($free);
			$devused = dwho_size_iec($used);

			if($total > 0):
				$devpercent = ($used / $total * 100);
			else:
				$devpercent = 0;
			endif;
?>
			<tr class="l-infos-<?=(($i % 2) + 1)?>on2">
				<td title="<?=dwho_alttitle($devstats[$i]['name']);?>">
					<?=dwho_htmlen(dwho_trunc($devstats[$i]['name'],20,'...',false));?>
				</td>
				<td class="gauge">
					<div><div style="width: <?=round($devpercent);?>px;">&nbsp;</div></div>
				</td>
				<td class="gaugepercent txt-right"><?=$this->bbf('number_percent',$devpercent);?></td>
				<td class="txt-right"><?=$this->bbf('size_iec_'.$devfree[1],$devfree[0]);?></td>
				<td class="txt-right"><?=$this->bbf('size_iec_'.$devused[1],$devused[0]);?></td>
				<td class="td-right txt-right"><?=$this->bbf('size_iec_'.$devtotal[1],$devtotal[0]);?></td>
			</tr>
<?php
		endfor;
	else:
?>
			<tr class="l-infos-1on2">
				<td colspan="6" class="td-single"><?=$this->bbf('sysinfos_no-devstats');?></td>
			</tr>
<?php
	endif;
?>
		</table>
	</div>
</div>
<div id="cpustats">
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
<div id="netstats">
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
if(is_array($netstats) === true):
	$i = 0;
	foreach($netstats as $devname => $stats):
		$rx_bytes = dwho_size_iec($stats['statistics']['rx']['bytes']);
		$tx_bytes = dwho_size_iec($stats['statistics']['tx']['bytes']);
		$total_errs = dwho_size_iec($stats['statistics']['total']['errs']);
		$total_drop = dwho_size_iec($stats['statistics']['total']['drop']);
?>
		<tr class="l-infos-<?=(($i++ % 2) + 1)?>on2">
			<td title="<?=dwho_alttitle($devname);?>"><?=dwho_htmlen(dwho_trunc($devname,20,'...',false));?></td>
			<td class="txt-right"><?=$this->bbf('size_iec_'.$rx_bytes[1],$rx_bytes[0]);?></td>
			<td class="txt-right"><?=$this->bbf('size_iec_'.$tx_bytes[1],$tx_bytes[0]);?></td>
			<td class="txt-right"><?=$stats['statistics']['total']['errs']?></td>
			<td class="td-right txt-right"><?=$stats['statistics']['total']['drop']?></td>
		</tr>
<?php
	endforeach;
else:
?>
		<tr class="l-infos-1on2">
			<td colspan="5" class="td-single"><?=$this->bbf('sysinfos_no-netstats');?></td>
		</tr>
<?php
endif;
?>
	</table>
</div>
<div class="clearboth"></div>