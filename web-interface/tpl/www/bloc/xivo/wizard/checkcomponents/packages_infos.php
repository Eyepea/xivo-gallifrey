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

$category = $this->get_var('package_category');
$level = $this->get_var('package_level');
$package = $this->get_var('packages',$category,$level);

?>
	<tr class="l-subth">
		<td colspan="3" class="td-single"><?=$this->bbf('package_category',$category);?></td>
	</tr>
	<tr class="l-subth l-colhead">
		<td><?=$this->bbf('package_col_name');?></td>
		<td><?=$this->bbf('package_col_installedversion');?></td>
		<td class="td-right"><?=$this->bbf('package_col_status');?></td>
	</tr>
<?php
if(is_array($package) === true && empty($package) === false):
	$i = 0;
	foreach($package as $name => $info):
?>
	<tr class="l-infos-<?=(($i++ % 2) + 1)?>on2 status-<?=$info['status']?>">
		<td class="td-left txt-center col-package-name"><?=dwho_htmlen($name);?></td>
		<td class="td-center txt-center col-package-installedversion"><?=dwho_htmlen($info['installedversion']);?></td>
		<td class="td-right txt-center col-package-status"><?=$this->bbf('package_status',$info['status']);?></td>
	</tr>
<?php
	endforeach;
else:
?>
	<tr class="l-infos-1on2">
		<td colspan="3" class="td-single"><?=$this->bbf('no_package',$level);?></td>
	</tr>
<?php
endif;
?>
