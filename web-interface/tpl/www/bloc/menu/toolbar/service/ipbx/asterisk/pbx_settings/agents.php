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
$group = $this->get_var('group');

$param = array('act' => 'addagent');

if($act !== 'list' && $act !== 'add'):
	$param['group'] = $group;
else:
	$group = '';
endif;

if($act === 'list'):
	$toolbar_js = array();
	$toolbar_js[] = 'var xivo_toolbar_form_name = \'fm-agentgroups-list\';';
	$toolbar_js[] = 'var xivo_toolbar_form_list = \'agentgroups[]\';';
	$toolbar_js[] = 'var xivo_toolbar_adv_menu_delete_confirm = \''.$dhtml->escape($this->bbf('toolbar_adv_menu_delete_confirm-agentgroup')).'\';';

	$dhtml->write_js($toolbar_js);
elseif($act === 'listagent'):
	$toolbar_js = array();
	$toolbar_js[] = 'var xivo_toolbar_form_name = \'fm-agents-list\';';
	$toolbar_js[] = 'var xivo_toolbar_form_list = \'agents[]\';';
	$toolbar_js[] = 'var xivo_toolbar_adv_menu_delete_confirm = \''.$dhtml->escape($this->bbf('toolbar_adv_menu_delete_confirm-agent')).'\';';

	$dhtml->write_js($toolbar_js);
endif;

?>
<script type="text/javascript" src="<?=$this->file_time($this->url('js/xivo_toolbar.js'));?>"></script>

<form action="#" method="post" accept-charset="utf-8">
<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),

		$form->hidden(array('name'	=> 'act',
				    'value'	=> $act));
?>
	<div class="fm-paragraph">
<?php
		echo	$form->select(array('name'	=> 'group',
					    'id'	=> 'it-toolbar-group',
					    'paragraph'	=> false,
					    'browse'	=> 'agentgroup',
					    'empty'	=> $this->bbf('toolbar_fm_group'),
					    'key'	=> 'name',
					    'altkey'	=> 'id',
					    'selected'	=> $group),
				      $this->get_var('agentgroup_list'));
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
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add-group'),
				       'service/ipbx/pbx_settings/agents',
				       'act=add');?></li>
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add-agent'),
				       'service/ipbx/pbx_settings/agents',
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
</div><?php

elseif($act === 'listagent'):
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
			      dwho_eid('toolbar-advanced-menu-enable'),
			      xivo_toolbar_fn_adv_menu_enable);

	dwho.dom.add_event('click',
			   dwho_eid('toolbar-advanced-menu-enable'),
			   function(e)
			   {
				if(dwho_is_function(e.preventDefault) === true)
					e.preventDefault();

				dwho.fm[xivo_toolbar_form_name]['act'].value = 'enableagents';
				dwho.fm[xivo_toolbar_form_name].submit();
			   });

	dwho.dom.remove_event('click',
			      dwho_eid('toolbar-advanced-menu-disable'),
			      xivo_toolbar_fn_adv_menu_disable);

	dwho.dom.add_event('click',
			   dwho_eid('toolbar-advanced-menu-disable'),
			   function(e)
			   {
				if(dwho_is_function(e.preventDefault) === true)
					e.preventDefault();

				dwho.fm[xivo_toolbar_form_name]['act'].value = 'disableagents';
				dwho.fm[xivo_toolbar_form_name].submit();
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
			   dwho_eid('it-toolbar-group'),
			   function()
			   {
				this.form['act'].value = 'list';

				if(this.value !== '')
					this.form['act'].value += 'agent';

				this.form.submit();
			   });
});
</script>
