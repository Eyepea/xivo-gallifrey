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
$dir = $this->get_var('dir');
$search = (string) $this->get_var('search');

$param = array('act' => 'add');

if($act !== 'listdir' && $act !== 'adddir'):
	$param['dir'] = $dir;
else:
	$dir = '';
endif;

$toolbar_js = array();
$toolbar_js[] = 'var xivo_toolbar_fm_search = \''.$dhtml->escape($search).'\';';
$toolbar_js[] = 'var xivo_toolbar_form_name = \'fm-files-list\';';
$toolbar_js[] = 'var xivo_toolbar_form_list = \'files[]\';';
$toolbar_js[] = 'var xivo_toolbar_adv_menu_delete_confirm = \''.$dhtml->escape($this->bbf('toolbar_adv_menu_delete_confirm')).'\';';

$dhtml->write_js($toolbar_js);

if($dir === '' && $search === ''):
	$dirjs = '';
else:
	$dirjs = $dhtml->escape($dir);
endif;

?>
<script type="text/javascript" src="<?=$this->file_time($this->url('js/xivo_toolbar.js'));?>"></script>

<form action="#" method="post" id="fm-sounds-toolbar" accept-charset="utf-8">
<?php
	echo	$form->hidden(array('name'	=> XIVO_SESS_NAME,
				    'value'	=> XIVO_SESS_ID)),

		$form->hidden(array('name'	=> 'act',
				    'value'	=> 'list'));
?>
	<div class="fm-field">
<?php

if($act === 'list'):
		echo	$form->text(array('name'	=> 'search',
					  'id'		=> 'it-toolbar-search',
					  'size'	=> 20,
					  'field'	=> false,
					  'value'	=> $search,
					  'default'	=> $this->bbf('toolbar_fm_search'))),

			$form->image(array('name'	=> 'submit',
					   'id'		=> 'it-subsearch',
					   'src'	=> $url->img('img/menu/top/toolbar/bt-search.gif'),
					   'field'	=> false,
					   'alt'	=> $this->bbf('toolbar_fm_search')));
endif;

		echo	$form->select(array('name'	=> 'dir',
					    'id'	=> 'it-toolbar-directory',
					    'key'	=> false,
					    'field'	=> false,
					    'empty'	=> $this->bbf('toolbar_fm_directory'),
					    'value'	=> $dir),
				      $this->get_var('list_dirs'),
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
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add-directory'),
				       'service/ipbx/pbx_services/sounds',
				       'act=adddir');?></li>
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add-file'),
				       'service/ipbx/pbx_services/sounds',
				       $param);?></li>
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
			<a href="#" id="toolbar-advanced-menu-select-all"><?=$this->bbf('toolbar_adv_menu_select-all');?></a>
		</li>
		<li>
			<a href="#" id="toolbar-advanced-menu-delete"><?=$this->bbf('toolbar_adv_menu_delete');?></a>
		</li>
	</ul>
</div>

<script type="text/javascript">
xivo.dom.set_onload(function()
{
	xivo.dom.remove_event('click',
			      xivo_eid('toolbar-advanced-menu-delete'),
			      xivo_toolbar_fn_adv_menu_delete);

	xivo.dom.add_event('click',
			   xivo_eid('toolbar-advanced-menu-delete'),
			   function(e)
			   {
				if(xivo_is_function(e.preventDefault) === true)
					e.preventDefault();

				if(confirm(xivo_toolbar_adv_menu_delete_confirm) === true)
				{
					if(xivo_is_undef(xivo_fm[xivo_toolbar_form_name]['search']) === false)
						xivo_fm[xivo_toolbar_form_name]['search'].value = xivo_toolbar_fm_search;

					if(xivo_is_undef(xivo_fm[xivo_toolbar_form_name]['dir']) === false)
						xivo_fm[xivo_toolbar_form_name]['dir'].value = '<?=$dirjs;?>';

					xivo_fm[xivo_toolbar_form_name]['act'].value = 'deletes';
					xivo_fm[xivo_toolbar_form_name].submit();
				}
			   });
});
</script>
<?php

endif;

?>
<script type="text/javascript">
xivo.dom.set_onload(function()
{
	xivo.dom.add_event('change',
			   xivo_eid('it-toolbar-directory'),
			   function()
			   {
<?php
	if($act === 'list'):
?>
				this.form['search'].value = '';
<?php
	endif;
?>
				this.form['act'].value = 'list';

				if(this.value === '')
					this.form['act'].value += 'dir';

				this.form.submit();
			   });
});
</script>
