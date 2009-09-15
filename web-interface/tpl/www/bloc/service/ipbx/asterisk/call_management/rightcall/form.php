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

$info = $this->get_var('info');
$element = $this->get_var('element');
$context_list = $this->get_var('context_list');

$rcalluser = $this->get_var('rcalluser');
$rcallgroup = $this->get_var('rcallgroup');
$rcallincall = $this->get_var('rcallincall');
$rcalloutcall = $this->get_var('rcalloutcall');
$rcallexten = $this->get_var('rcallexten');

if($this->get_var('fm_save') === false):
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_rightcall_name'),
				  'name'	=> 'rightcall[name]',
				  'labelid'	=> 'rightcall-name',
				  'size'	=> 15,
				  'default'	=> $element['rightcall']['name']['default'],
				  'value'	=> $info['rightcall']['name']));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_rightcall_context'),
				    'name'	=> 'rightcall[context]',
				    'labelid'	=> 'rightcall-context',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['rightcall']['context']['default'],
				    'value'	=> $info['rightcall']['context']),
			      $context_list);
else:
	echo	'<div id="fd-rightcall-context" class="txt-center">',
		$url->href_html($this->bbf('create_context'),
				'service/ipbx/system_management/context',
				'act=add'),
		'</div>';
endif;

	echo	$form->text(array('desc'	=> $this->bbf('fm_rightcall_passwd'),
				  'name'	=> 'rightcall[passwd]',
				  'labelid'	=> 'rightcall-passwd',
				  'size'	=> 15,
				  'default'	=> $element['rightcall']['passwd']['default'],
				  'value'	=> $info['rightcall']['passwd'])),

		$form->select(array('desc'	=> $this->bbf('fm_rightcall_authorization'),
				    'name'	=> 'rightcall[authorization]',
				    'bbf'	=> array('concatvalue','fm_rightcall_authorization-opt-'),
				    'labelid'	=> 'authorization',
				    'value'	=> $info['rightcall']['authorization'],
				    'default'	=> $element['rightcall']['authorization']['default']),
			      $element['rightcall']['authorization']['value']);
?>
<div id="extenlist" class="fm-field fm-multilist">
	<p>
		<label id="lb-exten" for="it-exten"><?=$this->bbf('fm_rightcallexten_exten');?></label>
	</p>
	<div class="slt-list">
		<?=$form->select(array('name'		=> 'rightcallexten[]',
				       'label'		=> false,
				       'id'		=> 'it-exten',
				       'key'		=> true,
				       'altkey'		=> 'exten',
				       'multiple'	=> true,
				       'size'		=> 5,
				       'field'		=> false),
				 $rcallexten);?>
		<div class="bt-adddelete">
			<a href="#"
			   onclick="xivo_fm_select_add_exten('it-exten',
			   				     prompt('<?=$dhtml->escape($this->bbf('rightcallexten_add-extension'));?>'));
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_addexten');?>">
				<?=$url->img_html('img/site/button/mini/blue/add.gif',
						  $this->bbf('bt_addexten'),
						  'class="bt-addlist" id="bt-addexten" border="0"');?></a><br />
			<a href="#"
			   onclick="xivo_fm_select_delete_entry('it-exten');
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_deleteexten');?>">
				<?=$url->img_html('img/site/button/mini/orange/delete.gif',
						  $this->bbf('bt_deleteexten'),
						  'class="bt-deletelist" id="bt-deleteexten" border="0"');?></a>
		</div>
	</div>
</div>
<div class="clearboth"></div>

<div class="fm-field fm-description">
	<p>
		<label id="lb-rightcall-description" for="it-rightcall-description"><?=$this->bbf('fm_rightcall_description');?></label>
	</p>
	<?=$form->textarea(array('field'	=> false,
				 'label'	=> false,
				 'name'		=> 'rightcall[description]',
				 'id'		=> 'it-rightcall-description',
				 'cols'		=> 60,
				 'rows'		=> 5,
				 'default'	=> $element['rightcall']['description']['default']),
			   $info['rightcall']['description']);?>
</div>
</div>

<div id="sb-part-rightcalluser" class="b-nodisplay">
<?php
	if($rcalluser['list'] !== false):
?>
	<div id="userlist" class="fm-field fm-multilist">
		<div class="slt-outlist">
			<?=$form->select(array('name'		=> 'userlist',
					       'label'		=> false,
					       'id'		=> 'it-userlist',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false),
					 $rcalluser['list']);?>
		</div>

		<div class="inout-list">
			<a href="#"
			   onclick="xivo_fm_move_selected('it-userlist','it-user');
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_inuser');?>">
				<?=$url->img_html('img/site/button/row-left.gif',
						  $this->bbf('bt_inuser'),
						  'class="bt-inlist" id="bt-inuser" border="0"');?></a><br />
			<a href="#"
			   onclick="xivo_fm_move_selected('it-user','it-userlist');
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_outuser');?>">
				<?=$url->img_html('img/site/button/row-right.gif',
						  $this->bbf('bt_outuser'),
						  'class="bt-outlist" id="bt-outuser" border="0"');?></a>
		</div>

		<div class="slt-inlist">
			<?=$form->select(array('name'		=> 'rightcalluser[]',
					       'label'		=> false,
					       'id'		=> 'it-user',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false),
					 $rcalluser['slt']);?>
		</div>
	</div>
	<div class="clearboth"></div>
<?php
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_user'),
					'service/ipbx/pbx_settings/users',
					'act=add'),
			'</div>';
	endif;
?>
</div>

<div id="sb-part-rightcallgroup" class="b-nodisplay">
<?php
	if($rcallgroup['list'] !== false):
?>
	<div id="grouplist" class="fm-field fm-multilist">
		<div class="slt-outlist">
			<?=$form->select(array('name'		=> 'grouplist',
					       'label'		=> false,
					       'id'		=> 'it-grouplist',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false),
					 $rcallgroup['list']);?>
		</div>

		<div class="inout-list">
			<a href="#"
			   onclick="xivo_fm_move_selected('it-grouplist','it-group');
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_ingroup');?>">
				<?=$url->img_html('img/site/button/row-left.gif',
						  $this->bbf('bt_ingroup'),
						  'class="bt-inlist" id="bt-ingroup" border="0"');?></a><br />
			<a href="#"
			   onclick="xivo_fm_move_selected('it-group','it-grouplist');
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_outgroup');?>">
				<?=$url->img_html('img/site/button/row-right.gif',
						  $this->bbf('bt_outgroup'),
						  'class="bt-outlist" id="bt-outgroup" border="0"');?></a>
		</div>

		<div class="slt-inlist">
			<?=$form->select(array('name'		=> 'rightcallgroup[]',
					       'label'		=> false,
					       'id'		=> 'it-group',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false),
					 $rcallgroup['slt']);?>
		</div>
	</div>
	<div class="clearboth"></div>
<?php
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_group'),
					'service/ipbx/pbx_settings/groups',
					'act=add'),
			'</div>';
	endif;
?>
</div>
<div id="sb-part-rightcallincall" class="b-nodisplay">
<?php
	if($rcallincall['list'] !== false):
?>
	<div id="incalllist" class="fm-field fm-multilist">
		<div class="slt-outlist">
			<?=$form->select(array('name'		=> 'incalllist',
					       'label'		=> false,
					       'id'		=> 'it-incalllist',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false),
					 $rcallincall['list']);?>
		</div>

		<div class="inout-list">
			<a href="#"
			   onclick="xivo_fm_move_selected('it-incalllist','it-incall');
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_inincall');?>">
				<?=$url->img_html('img/site/button/row-left.gif',
						  $this->bbf('bt_inincall'),
						  'class="bt-inlist" id="bt-inincall" border="0"');?></a><br />
			<a href="#"
			   onclick="xivo_fm_move_selected('it-incall','it-incalllist');
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_outincall');?>">
				<?=$url->img_html('img/site/button/row-right.gif',
						  $this->bbf('bt_outincall'),
						  'class="bt-outlist" id="bt-outincall" border="0"');?></a>
		</div>

		<div class="slt-inlist">
			<?=$form->select(array('name'		=> 'rightcallincall[]',
					       'label'		=> false,
					       'id'		=> 'it-incall',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false),
					 $rcallincall['slt']);?>
		</div>
	</div>
	<div class="clearboth"></div>
<?php
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_incall'),
					'service/ipbx/call_management/incall',
					'act=add'),
			'</div>';
	endif;
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	if($rcalloutcall['list'] !== false):
?>
	<div id="outcalllist" class="fm-field fm-multilist">
		<div class="slt-outlist">
			<?=$form->select(array('name'		=> 'outcalllist',
					       'label'		=> false,
					       'id'		=> 'it-outcalllist',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false),
					 $rcalloutcall['list']);?>
		</div>

		<div class="inout-list">
			<a href="#"
			   onclick="xivo_fm_move_selected('it-outcalllist','it-outcall');
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_inoutcall');?>">
				<?=$url->img_html('img/site/button/row-left.gif',
						  $this->bbf('bt_inoutcall'),
						  'class="bt-inlist" id="bt-inoutcall" border="0"');?></a><br />
			<a href="#"
			   onclick="xivo_fm_move_selected('it-outcall','it-outcalllist');
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_outoutcall');?>">
				<?=$url->img_html('img/site/button/row-right.gif',
						  $this->bbf('bt_outoutcall'),
						  'class="bt-outlist" id="bt-outoutcall" border="0"');?></a>
		</div>

		<div class="slt-inlist">
			<?=$form->select(array('name'		=> 'rightcalloutcall[]',
					       'label'		=> false,
					       'id'		=> 'it-outcall',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false),
					 $rcalloutcall['slt']);?>
		</div>
	</div>
	<div class="clearboth"></div>
<?php
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_outcall'),
					'service/ipbx/call_management/outcall',
					'act=add'),
			'</div>';
	endif;
?>
</div>
