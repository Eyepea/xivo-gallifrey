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

$info = $this->get_var('info');
$element = $this->get_var('element');

$groups = $this->get_var('groups');
$gmember = $this->get_var('gmember');

$queues = $this->get_var('queues');
$qmember = $this->get_var('qmember');

?>

<fieldset id="fld-group">
	<legend><?=$this->bbf('fld-callgroup');?></legend>
<?php
	if(is_array($groups) === true && empty($groups) === false):
?>
<div id="grouplist" class="fm-paragraph fm-multilist">
	<div class="slt-outlist">
<?php
		echo	   $form->select(array('name'		=> 'grouplist',
					       'label'		=> false,
					       'id'		=> 'it-grouplist',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'paragraph'	=> false,
					       'key'		=> 'name',
					       'altkey'		=> 'name'),
					 $gmember['list']);
?>
	</div>

	<div class="inout-list">
		<a href="#"
		   onclick="xivo_ast_user_ingroup();
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_ingroup');?>">
			<?=$url->img_html('img/site/button/arrow-left.gif',
					  $this->bbf('bt_ingroup'),
					  'class="bt-inlist" id="bt-ingroup" border="0"');?></a><br />
		<a href="#"
		   onclick="xivo_ast_user_outgroup();
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_outgroup');?>">
			<?=$url->img_html('img/site/button/arrow-right.gif',
					  $this->bbf('bt_outgroup'),
					  'class="bt-outlist" id="bt-outgroup" border="0"');?></a>
	</div>

	<div class="slt-inlist">
<?php
		echo	$form->select(array('name'	=> 'group-select[]',
					    'label'	=> false,
					    'id'	=> 'it-group',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false,
					    'key'	=> 'name',
					    'altkey'	=> 'name'),
				      $gmember['slt']);
?>
	</div>
</div>
<div class="clearboth"></div>

<div class="sb-list">
	<table cellspacing="0" cellpadding="0" border="0">
		<tr class="sb-top">
			<th class="th-left"><?=$this->bbf('col_group-name');?></th>
			<th class="th-center"><?=$this->bbf('col_group-channel');?></th>
			<th class="th-right"><?=$this->bbf('col_group-calllimit');?></th>
		</tr>
<?php
		foreach($groups as $value):
			$name = $value['name'];

			if(dwho_issa($value['id'],$gmember['info']) === true):
				$class = '';
				$value['member'] = $gmember['info'][$value['id']];
				$calllimit = intval($value['member']['call-limit']);
			else:
				$class = ' b-nodisplay';
				$value['member'] = null;
				$calllimit = '';
			endif;

		echo	'<tr id="group-',$name,'" class="fm-paragraph',$class,'">',"\n",
			'<td class="td-left">',$name,'</td>',"\n",
			'<td>',
			$form->select(array('paragraph'	=> false,
					    'name'	=> 'group['.$name.'][chantype]',
					    'id'	=> false,
					    'label'	=> false,
					    'key'	=> false,
					    'default'	=> $element['qmember']['chantype']['default'],
					    'selected'	=> $value['member']['channel']),
				      $element['qmember']['chantype']['value']),
			'</td>',"\n",
			'<td class="td-right">',
			$form->select(array('paragraph'	=> false,
					    'name'	=> 'group['.$name.'][call-limit]',
					    'id'	=> false,
					    'label'	=> false,
					    'default'	=> $element['qmember']['call-limit']['default'],
					    'selected'	=> $calllimit),
				      $element['qmember']['call-limit']['value']),
			'</td>',"\n",
			'</tr>',"\n";
		endforeach;
?>
		<tr id="no-group"<?=(empty($gmember['slt']) === false ? ' class="b-nodisplay"' : '')?>>
			<td colspan="3" class="td-single"><?=$this->bbf('no_group');?></td>
		</tr>
	</table>
</div>
<?php
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_group'),
					'service/ipbx/pbx_settings/groups',
					'act=add'),
			'</div>';
	endif;
?>
</fieldset>

<fieldset id="fld-queue">
	<legend><?=$this->bbf('fld-queuegroup');?></legend>
<?php
	if(is_array($queues) === true && empty($queues) === false):
?>
<div id="queuelist" class="fm-paragraph fm-multilist">
	<div class="slt-outlist">
<?php
		echo	$form->select(array('name'	=> 'queuelist',
					    'label'	=> false,
					    'id'	=> 'it-queuelist',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false,
					    'key'	=> 'name',
					    'altkey'	=> 'name'),
				      $qmember['list']);
?>
	</div>

	<div class="inout-list">
		<a href="#"
		   onclick="xivo_ast_inqueue();
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_inqueue');?>">
			<?=$url->img_html('img/site/button/arrow-left.gif',
					  $this->bbf('bt_inqueue'),
					  'class="bt-inlist" id="bt-inqueue" border="0"');?></a><br />
		<a href="#"
		   onclick="xivo_ast_outqueue();
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_outqueue');?>">
			<?=$url->img_html('img/site/button/arrow-right.gif',
					  $this->bbf('bt_outqueue'),
					  'class="bt-outlist" id="bt-outqueue" border="0"');?></a>

	</div>

	<div class="slt-inlist">
<?php
		echo	$form->select(array('name'	=> 'queue-select[]',
					    'label'	=> false,
					    'id'	=> 'it-queue',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false,
					    'key'	=> 'name',
					    'altkey'	=> 'name'),
				      $qmember['slt']);
?>
	</div>
</div>
<div class="clearboth"></div>

<div class="sb-list">
	<table cellspacing="0" cellpadding="0" border="0">
		<tr class="sb-top">
			<th class="th-left"><?=$this->bbf('col_queue-name');?></th>
			<th class="th-center"><?=$this->bbf('col_queue-channel');?></th>
			<th class="th-center"><?=$this->bbf('col_queue-penalty');?></th>
			<th class="th-right"><?=$this->bbf('col_queue-calllimit');?></th>
		</tr>
<?php
		foreach($queues as $value):
			$name = $value['name'];

			if(dwho_issa($value['id'],$qmember['info']) === true):
				$class = '';
				$value['member'] = $qmember['info'][$value['id']];
				$calllimit = intval($value['member']['call-limit']);
				$penalty = intval($value['member']['penalty']);
			else:
				$class = ' b-nodisplay';
				$value['member'] = null;
				$penalty = $calllimit = '';
			endif;

		echo	'<tr id="queue-',$name,'" class="fm-paragraph',$class,'">',"\n",
			'<td class="td-left">',$name,'</td>',"\n",
			'<td>',
			$form->select(array('paragraph'	=> false,
					    'name'	=> 'queue['.$name.'][chantype]',
					    'id'	=> false,
					    'label'	=> false,
					    'key'	=> false,
					    'default'	=> $element['qmember']['chantype']['default'],
					    'selected'	=> $value['member']['channel']),
				      $element['qmember']['chantype']['value']),
			'</td>',"\n",
			'<td>',
			$form->select(array('paragraph'	=> false,
					    'name'	=> 'queue['.$name.'][penalty]',
					    'id'	=> false,
					    'label'	=> false,
					    'default'	=> $element['qmember']['penalty']['default'],
					    'selected'	=> $penalty),
				      $element['qmember']['penalty']['value']),
			'</td>',"\n",
			'<td class="td-right">',
			$form->select(array('paragraph'	=> false,
					    'name'	=> 'queue['.$name.'][call-limit]',
					    'id'	=> false,
					    'label'	=> false,
					    'default'	=> $element['qmember']['call-limit']['default'],
					    'selected'	=> $calllimit),
				      $element['qmember']['call-limit']['value']),
			'</td>',"\n",
			'</tr>',"\n";
		endforeach;
?>
		<tr id="no-queue"<?=(empty($qmember['slt']) === false ? ' class="b-nodisplay"' : '')?>>
			<td colspan="4" class="td-single"><?=$this->bbf('no_queue');?></td>
		</tr>
	</table>
</div>
<?php
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_queue'),
					'service/ipbx/pbx_settings/queues',
					'act=add'),
			'</div>';
	endif;
?>
</fieldset>
