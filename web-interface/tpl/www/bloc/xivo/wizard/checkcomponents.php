<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

$form = &$this->get_module('form');

$fulldriver = array();

if(($fulldriver['driver'] = (string) $this->get_var('network','driver')) === ''):
	$fulldriver['driver'] = '-';
elseif(($driverversion = (string) $this->get_var('network','driverversion')) !== ''):
	$fulldriver['driverversion'] = $driverversion;
endif;

$packages = $this->get_var('packages');

?>
<div id="memstats" class="sb-list">
<?php
	$this->file_include('bloc/xivo/monitoring/memstats');
?>
</div>

<div id="network" class="sb-list">
	<table border="0" cellpadding="0" cellspacing="0">
		<tr class="sb-top">
			<th colspan="2" class="th-left th-right"><?=$this->bbf('sysinfos_network');?></th>
		</tr>
		<tr class="l-infos-1on2">
			<td class="td-left txt-left"><?=$this->bbf('network_iface');?></td>
			<td class="td-right txt-right"><?=dwho_htmlen($this->get_var_default('network','iface','-'));?></td>
		</tr>
		<tr class="l-infos-2on2">
			<td class="td-left txt-left"><?=$this->bbf('network_vendor');?></td>
			<td class="td-right txt-right"><?=dwho_htmlen($this->get_var_default('network','vendor','-'));?></td>
		</tr>
		<tr class="l-infos-1on2">
			<td class="td-left txt-left"><?=$this->bbf('network_driver');?></td>
			<td class="td-right txt-right"><?=dwho_htmlen($this->bbf('network_driver_info',$fulldriver));?></td>
		</tr>
		<tr class="l-infos-2on2">
			<td class="td-left txt-left"><?=$this->bbf('network_macaddress');?></td>
			<td class="td-right txt-right"><?=dwho_htmlen($this->get_var_default('network','macaddress','-'));?></td>
		</tr>
		<tr class="l-infos-1on2">
			<td class="td-left txt-left"><?=$this->bbf('network_ipaddress');?></td>
			<td class="td-right txt-right"><?=dwho_htmlen($this->get_var_default('network','ipaddress','-'));?></td>
		</tr>
		<tr class="l-infos-2on2">
			<td class="td-left txt-left"><?=$this->bbf('network_speed');?></td>
			<td class="td-right txt-right"><?=dwho_htmlen($this->get_var_default('network','speed','-'));?></td>
		</tr>
		<tr class="l-infos-1on2">
			<td class="td-left txt-left"><?=$this->bbf('network_autonegotiation');?></td>
			<td class="td-right txt-right"><?=dwho_htmlen($this->bbf('network_autonegotiation_info',
										 $this->get_var_default('network',
													'autonegotiation',
													'-')));?></td>
		</tr>
	</table>
</div>

<div id="packages" class="sb-list">
<?php
	$this->file_include('bloc/xivo/wizard/checkcomponents/packages',
			    array('package_level' => 'depends'));

	$this->file_include('bloc/xivo/wizard/checkcomponents/packages',
			    array('package_level' => 'recommends'));
?>
</div>
<?php
	echo	$form->button(array('name'	=> 'verify',
				    'value'	=> $this->bbf('fm_bt-verify'),
				    'id'	=> 'it-verify'));
?>
