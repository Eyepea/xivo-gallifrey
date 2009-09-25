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

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$act = $this->get_var('act');
$cat = $this->get_var('cat');

$param = array('act' => 'addfile');

if($act !== 'list' && $act !== 'add'):
	$param['cat'] = $cat;
else:
	$cat = '';
endif;

?>
<script type="text/javascript" src="<?=$this->file_time($this->url('js/xivo_toolbar.js'));?>"></script>

<form action="#" method="post" accept-charset="utf-8">
<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),

		$form->hidden(array('name'	=> 'act',
				    'value'	=> 'list'));
?>
	<div class="fm-field">
<?php
		echo	$form->select(array('name'	=> 'cat',
					    'id'	=> 'it-toolbar-category',
					    'key'	=> true,
					    'altkey'	=> 'category',
					    'field'	=> false,
					    'empty'	=> $this->bbf('toolbar_fm_category'),
					    'value'	=> $cat),
				      $this->get_var('list_cats'),
				      'onchange="this.form[\'act\'].value = this.value === \'\'
									    ? \'list\'
									    : \'listfile\';
						 this.form.submit();"');
?>
	</div>
</form>
<?php
	echo	$url->img_html('img/menu/top/toolbar/bt-add.gif',
			       $this->bbf('toolbar_opt_add'),
			       'id="toolbar-bt-add"
				border="0"');
?>
<div class="sb-advanced-menu">
	<ul id="toolbar-add-menu">
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add-category'),
				       'service/ipbx/pbx_services/musiconhold',
				       'act=add');?></li>
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add-file'),
				       'service/ipbx/pbx_services/musiconhold',
				       $param);?></li>
	</ul>
</div>

<script type="text/javascript">
dwho.dom.set_onload(function()
{
	dwho.dom.add_event('change',
			   dwho_eid('it-toolbar-category'),
			   function()
			   {
				this.form['act'].value = 'list';

				if(this.value !== '')
					this.form['act'].value += 'file';

				this.form.submit();
			   });
});
</script>
