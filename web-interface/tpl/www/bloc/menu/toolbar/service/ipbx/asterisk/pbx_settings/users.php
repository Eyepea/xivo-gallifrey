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
$dhtml = &$this->get_module('dhtml');

$act = $this->get_var('act');

$search = (string) $this->get_var('search');
$context = (string) $this->get_var('context');

$toolbar_js = array();
$toolbar_js[] = 'var xivo_toolbar_fm_search = \''.$dhtml->escape($search).'\';';
$toolbar_js[] = 'var xivo_toolbar_form_name = \'fm-users-list\';';
$toolbar_js[] = 'var xivo_toolbar_form_list = \'users[]\';';
$toolbar_js[] = 'var xivo_toolbar_adv_menu_delete_confirm = \''.$dhtml->escape($this->bbf('toolbar_adv_menu_delete_confirm')).'\';';

$dhtml->write_js($toolbar_js);

$context_js = $dhtml->escape($context);

?>
<script type="text/javascript" src="<?=$this->file_time($this->url('js/xivo_toolbar.js'));?>"></script>

<form action="#" method="post" accept-charset="utf-8">
<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),

		$form->hidden(array('name'	=> 'act',
				    'value'	=> 'list'));
?>
	<div class="fm-paragraph">
<?php
		echo	$form->text(array('name'	=> 'search',
					  'id'		=> 'it-toolbar-search',
					  'size'	=> 20,
					  'paragraph'	=> false,
					  'value'	=> $search,
					  'default'	=> $this->bbf('toolbar_fm_search'))),

			$form->image(array('name'	=> 'submit',
					   'id'		=> 'it-toolbar-subsearch',
					   'src'	=> $url->img('img/menu/top/toolbar/bt-search.gif'),
					   'paragraph'	=> false,
					   'alt'	=> $this->bbf('toolbar_fm_search'))),

			$form->select(array('name'	=> 'context',
					    'id'	=> 'it-toolbar-context',
					    'paragraph'	=> false,
					    'empty'	=> $this->bbf('toolbar_fm_context'),
					    'selected'	=> $context),
				      $this->get_var('contexts'),
				      'style="margin-left: 20px;"');
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
		<li><?=$url->href_html($this->bbf('toolbar_add_menu_add'),
				       'service/ipbx/pbx_settings/users',
				       'act=add');?></li>
		<li><?=$url->href_html($this->bbf('toolbar_add_menu_import-file'),
				       'service/ipbx/pbx_settings/users',
				       'act=import');?></li>
	</ul>
</div><?php

if($act === 'list'):
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

<script type="text/javascript">
dwho.dom.set_onload(function()
{
	dwho.dom.remove_event('click',
			      dwho_eid('toolbar-advanced-menu-delete'),
			      xivo_toolbar_fn_adv_menu_delete);

	dwho.dom.add_event('click',
			   dwho_eid('toolbar-advanced-menu-delete'),
			   function(e)
			   {
				if(dwho_is_function(e.preventDefault) === true)
					e.preventDefault();

				if(confirm(xivo_toolbar_adv_menu_delete_confirm) === true)
				{
					if(dwho_is_undef(dwho.fm[xivo_toolbar_form_name]['search']) === false)
						dwho.fm[xivo_toolbar_form_name]['search'].value = xivo_toolbar_fm_search;

					if(dwho_is_undef(dwho.fm[xivo_toolbar_form_name]['context']) === false)
						dwho.fm[xivo_toolbar_form_name]['context'].value = '<?=$context_js?>';

					dwho.fm[xivo_toolbar_form_name]['act'].value = 'deletes';
					dwho.fm[xivo_toolbar_form_name].submit();
				}
			   });
});
</script>
<?php

endif;

?>
<script type="text/javascript">
dwho.dom.set_onload(function()
{
	dwho.dom.add_event('change',
			   dwho_eid('it-toolbar-context'),
			   function(e)
			   {
				if(xivo_toolbar_fm_search === ''
				&& dwho_has_len(dwho.form.text_helper['it-toolbar-search']) === false)
					this.form['search'].value = '';

				this.form.submit();
			   });
});
</script>
