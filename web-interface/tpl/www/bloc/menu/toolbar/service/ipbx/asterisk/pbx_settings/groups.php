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

$url = &$this->get_module('url');
$dhtml = &$this->get_module('dhtml');

echo	$url->href_html($url->img_html('img/menu/top/toolbar/bt-add.gif',
				       $this->bbf('toolbar_opt_add'),
				       'border="0"'),
			'service/ipbx/pbx_settings/groups',
			'act=add',
			null,
			$this->bbf('toolbar_opt_add'));

if($this->get_var('act') === 'list'):
	echo	$url->img_html('img/menu/top/toolbar/bt-more.gif',
			       $this->bbf('toolbar_opt_advanced'),
			       'border="0"
				onmouseover="xivo_eid(\'advanced-menu\').style.display = \'block\';"
				onmouseout="xivo_eid(\'advanced-menu\').style.display = \'none\';"');
?>
<div class="sb-advanced-menu">
	<ul id="advanced-menu"
	    onmouseover="this.style.display = 'block';"
	    onmouseout="this.style.display = 'none';">
		<li>
			<a href="#"
			   onclick="xivo_fm['fm-group-list']['act'].value = 'enables';
				    xivo_fm['fm-group-list'].submit();">
				<?=$this->bbf('toolbar_adv_menu_enable');?></a>
		</li>
		<li>
			<a href="#"
			   onclick="xivo_fm['fm-group-list']['act'].value = 'disables';
				    xivo_fm['fm-group-list'].submit();">
				<?=$this->bbf('toolbar_adv_menu_disable');?></a>
		</li>
		<li>
			<a href="#"
			   onclick="this.tmp = xivo_fm['fm-group-list']['act'].value;
				    xivo_fm['fm-group-list']['act'].value = 'deletes';
				    return(confirm('<?=$dhtml->escape($this->bbf('toolbar_adv_menu_delete_confirm'));?>')
					   ? xivo_fm['fm-group-list'].submit()
					   : xivo_fm['fm-group-list']['act'] = this.tmp);">
				<?=$this->bbf('toolbar_adv_menu_delete');?></a>
		</li>
	</ul>
</div>
<?php

endif;

?>
