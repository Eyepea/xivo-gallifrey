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

$element = $this->get_var('element');
$info = $this->get_var('info');

$amember = $this->get_var('amember');

$queues = $this->get_var('queues');
$qmember = $this->get_var('qmember');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_agentgroup_name'),
				  'name'	=> 'agentgroup[name]',
				  'labelid'	=> 'agentgroup-name',
				  'size'	=> 15,
				  'default'	=> $element['agentgroup']['name']['default'],
				  'value'	=> $info['agentgroup']['name']));

	if($amember['list'] !== false):
?>
<div id="agentlist" class="fm-paragraph fm-multilist">
	<p><label id="lb-agentlist" for="it-agentlist"><?=$this->bbf('fm_agents');?></label></p>
	<div class="slt-outlist">
<?php
		echo	$form->select(array('name'	=> 'agentlist',
					    'label'	=> false,
					    'id'	=> 'it-agentlist',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false,
					    'browse'	=> 'agentfeatures',
					    'key'	=> 'identity',
					    'altkey'	=> 'id'),
				      $amember['list']);
?>
	</div>

	<div class="inout-list">
		<a href="#"
		   onclick="dwho.form.move_selected('it-agentlist','it-agent');
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_inagent');?>">
			<?=$url->img_html('img/site/button/arrow-left.gif',
					  $this->bbf('bt_inagent'),
					  'class="bt-inlist" id="bt-inagent" border="0"');?></a><br />
		<a href="#"
		   onclick="dwho.form.move_selected('it-agent','it-agentlist');
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_outagent');?>">
			<?=$url->img_html('img/site/button/arrow-right.gif',
					  $this->bbf('bt_outagent'),
					  'class="bt-outlist" id="bt-outagent" border="0"');?></a>
	</div>

	<div class="slt-inlist">
<?php
		echo	$form->select(array('name'	=> 'agent-select[]',
					    'label'	=> false,
					    'id'	=> 'it-agent',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false,
					    'browse'	=> 'agentfeatures',
					    'key'	=> 'identity',
					    'altkey'	=> 'id'),
				      $amember['slt']);
?>
	</div>
</div>
<div class="clearboth"></div>
<?php
	endif;
?>
	<div class="fm-paragraph fm-description">
		<p>
			<label id="lb-agentgroup-description" for="it-agentgroup-description"><?=$this->bbf('fm_agentgroup_description');?></label>
		</p>
		<?=$form->textarea(array('paragraph'	=> false,
					 'label'	=> false,
					 'name'		=> 'agentgroup[description]',
					 'id'		=> 'it-agentgroup-description',
					 'cols'		=> 60,
					 'rows'		=> 5,
					 'default'	=> $element['agentgroup']['description']['default']),
				   $info['agentgroup']['description']);?>
	</div>
</div>

<div id="sb-part-last" class="b-nodisplay">
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
			<th class="th-right"><?=$this->bbf('col_queue-penalty');?></th>
		</tr>
<?php
		foreach($queues as $value):
			$name = $value['name'];

			if(dwho_issa($value['id'],$qmember['info']) === true):
				$class = '';
				$value['member'] = $qmember['info'][$value['id']];
				$penalty = intval($value['member']['penalty']);
			else:
				$class = ' b-nodisplay';
				$value['member'] = null;
				$penalty = '';
			endif;

		echo	'<tr id="queue-',$name,'" class="fm-paragraph',$class,'">',"\n",
			'<td class="td-left">',$name,'</td>',"\n",
			'<td class="td-right">',
			$form->select(array('paragraph'	=> false,
					    'name'	=> 'queue['.$name.'][penalty]',
					    'id'	=> false,
					    'label'	=> false,
					    'default'	=> $element['qmember']['penalty']['default'],
					    'selected'	=> $penalty),
				      $element['qmember']['penalty']['value']),
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
</div>
