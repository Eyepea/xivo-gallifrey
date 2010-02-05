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

$memstats = $this->get_var('memstats');

$memrealused = $memstats['memused'] - $memstats['buffers'] - $memstats['cached'];

$memtotal = dwho_size_iec($memstats['memtotal']);
$memfree = dwho_size_iec($memstats['memfree']);
$memused = dwho_size_iec($memrealused);
$membuffers = dwho_size_iec($memstats['buffers']);
$memcached = dwho_size_iec($memstats['cached']);

if($memstats['memtotal'] > 0):
	$memrealusedpercent = ($memrealused / $memstats['memtotal'] * 100);
	$memrealusedpcentrnd = round($memrealusedpercent);
	$membufferspercent = ($memstats['buffers'] / $memstats['memtotal'] * 100);
	$membufferspcentrnd = round($membufferspercent);
	$memcachedpercent = ($memstats['cached'] / $memstats['memtotal'] * 100);
	$memcachedpcentrnd = round($memcachedpercent);
else:
	$memrealusedpercent = $memrealusedpcentrnd = 0;
	$membufferspercent = $membufferspcentrnd = 0;
	$memcachedpercent = $memcachedpcentrnd = 0;
endif;

$memusedpcentrnd = $memrealusedpcentrnd + $membufferspcentrnd + $memcachedpcentrnd;

$swaptotal = dwho_size_iec($memstats['swaptotal']);
$swapfree = dwho_size_iec($memstats['swapfree']);
$swapused = dwho_size_iec($memstats['swapused']);

if($memstats['swaptotal'] > 0):
	$swappercent = ($memstats['swapused'] / $memstats['swaptotal'] * 100);
else:
	$swappercent = 0;
endif;

?>
<table border="0" cellpadding="0" cellspacing="0">
	<tr class="sb-top">
		<th colspan="8" class="th-left th-right"><?=$this->bbf('sysinfos_memory');?></th>
	</tr>
	<tr class="l-subth">
		<td><?=$this->bbf('sysinfos_col_type');?></td>
		<td colspan="2"><?=$this->bbf('sysinfos_col_percent');?></td>
		<td><?=$this->bbf('sysinfos_col_free');?></td>
		<td><?=$this->bbf('sysinfos_col_used');?></td>
		<td><?=$this->bbf('sysinfos_col_buffers');?></td>
		<td><?=$this->bbf('sysinfos_col_cached');?></td>
		<td class="td-right"><?=$this->bbf('sysinfos_col_total');?></td>
	</tr>
	<tr class="l-infos-1on2">
		<td><?=$this->bbf('sysinfos_physical-memory');?></td>
		<td class="gauge">
			<div><div style="width: <?=$memusedpcentrnd?>px;">
				<img src="/img/z.gif"
				     width="<?=$memrealusedpcentrnd?>"
				     height="10"
				     alt="<?=$this->bbf('sysinfos_used-memory',$memrealusedpercent);?>" /><img
						src="/img/z.gif"
						width="<?=$membufferspcentrnd?>"
						height="10"
						alt="<?=$this->bbf('sysinfos_buffers-memory',$membufferspercent);?>"
						style="background-color: #5dbc00;" /><img
							src="/img/z.gif"
							width="<?=$memcachedpcentrnd?>"
							height="10"
							alt="<?=$this->bbf('sysinfos_cached-memory',$memcachedpercent);?>"
							style="background-color: #f96101;" />
			</div></div>
		</td>
		<td class="gaugepercent txt-right"><?=$this->bbf('number_percent',$memrealusedpercent);?></td>
		<td class="txt-right"><?=$this->bbf('size_iec_'.$memfree[1],$memfree[0]);?></td>
		<td class="txt-right"><?=$this->bbf('size_iec_'.$memused[1],$memused[0]);?></td>
		<td class="txt-right"><?=$this->bbf('size_iec_'.$membuffers[1],$membuffers[0]);?></td>
		<td class="txt-right"><?=$this->bbf('size_iec_'.$memcached[1],$memcached[0]);?></td>
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
		<td class="txt-right">-</td>
		<td class="txt-right">-</td>
		<td class="td-right txt-right"><?=$this->bbf('size_iec_'.$swaptotal[1],$swaptotal[0]);?></td>
	</tr>
</table>
