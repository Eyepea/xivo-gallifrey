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

$packages = $this->get_var('packages');
$level = $this->get_var('package_level'); 

if(is_array($packages) === true && empty($packages) === false):

?>
<table border="0" cellpadding="0" cellspacing="0" class="<?=$level?>">
	<tr class="sb-top">
		<th colspan="3" class="th-left th-right"><?=$this->bbf('package_level',$level);?></th>
	</tr>
<?php
	reset($packages);

	while(list($key) = each($packages)):
		$this->file_include('bloc/xivo/wizard/checkcomponents/packages_infos',
				    array('package_category'	=> $key));
	endwhile;
?>
</table>
<?php

endif;

?>
