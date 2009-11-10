<?php

#
# XiVO Web-Interface
# Copyright (C) 2009  Proformatique <technique@proformatique.com>
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

$toolbar_js = array();
$toolbar_js[] = 'var xivo_toolbar_form_name = \'fm-netiface-list\';';
$toolbar_js[] = 'var xivo_toolbar_form_list = \'netiface[]\';';
$toolbar_js[] = 'var xivo_toolbar_adv_menu_delete_confirm = \''.$dhtml->escape($this->bbf('toolbar_adv_menu_delete_confirm')).'\';';

$dhtml->write_js($toolbar_js);

?>
<script type="text/javascript" src="<?=$this->file_time($this->url('js/xivo_toolbar.js'));?>"></script>
<?php

echo	$url->href_html($url->img_html('img/menu/top/toolbar/bt-add.gif',
				       $this->bbf('toolbar_opt_add'),
				       'id="toolbar-bt-add"
					border="0"'),
			'xivo/configuration/network/interface',
			'act=add',
			null,
			$this->bbf('toolbar_opt_add'));

if($this->get_var('act') === 'list'):
	echo	$url->img_html('img/menu/top/toolbar/bt-more.gif',
			       $this->bbf('toolbar_opt_advanced'),
			       'id="toolbar-bt-advanced"
				border="0"');
?>
<div class="sb-advanced-menu">
	<ul id="toolbar-advanced-menu">
		<li>
			<a href="#" id="toolbar-advanced-menu-enable"><?=$this->bbf('toolbar_adv_menu_enable');?></a>
		</li>
		<li>
			<a href="#" id="toolbar-advanced-menu-disable"><?=$this->bbf('toolbar_adv_menu_disable');?></a>
		</li>
		<li>
			<a href="#" id="toolbar-advanced-menu-select-all"><?=$this->bbf('toolbar_adv_menu_select-all');?></a>
		</li>
		<li>
			<a href="#" id="toolbar-advanced-menu-delete"><?=$this->bbf('toolbar_adv_menu_delete');?></a>
		</li>
	</ul>
</div>
<?php

endif;

?>
