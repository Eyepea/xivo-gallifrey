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
$context_list = $this->get_var('context_list');

$umember = $this->get_var('umember');

$queues = $this->get_var('queues');
$qmember = $this->get_var('qmember');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_agentfeatures_firstname'),
				  'name'	=> 'agentfeatures[firstname]',
				  'labelid'	=> 'agentfeatures-firstname',
				  'size'	=> 15,
				  'default'	=> $element['agentfeatures']['firstname']['default'],
				  'value'	=> $info['agentfeatures']['firstname'])),

		$form->text(array('desc'	=> $this->bbf('fm_agentfeatures_lastname'),
				  'name'	=> 'agentfeatures[lastname]',
				  'labelid'	=> 'agentfeatures-lastname',
				  'size'	=> 15,
				  'default'	=> $element['agentfeatures']['lastname']['default'],
				  'value'	=> $info['agentfeatures']['lastname'])),

		$form->text(array('desc'	=> $this->bbf('fm_agentfeatures_number'),
				  'name'	=> 'agentfeatures[number]',
				  'labelid'	=> 'agentfeatures-number',
				  'size'	=> 15,
				  'default'	=> $element['agentfeatures']['number']['default'],
				  'value'	=> $info['agentfeatures']['number'])),

		$form->text(array('desc'	=> $this->bbf('fm_agentfeatures_passwd'),
				  'name'	=> 'agentfeatures[passwd]',
				  'labelid'	=> 'agentfeatures-passwd',
				  'size'	=> 15,
				  'default'	=> $element['agentfeatures']['passwd']['default'],
				  'value'	=> $info['agentfeatures']['passwd']));

		if($context_list !== false):
			echo	$form->select(array('desc'	=> $this->bbf('fm_agentfeatures_context'),
						    'name'	=> 'agentfeatures[context]',
						    'labelid'	=> 'agentfeatures-context',
						    'key'	=> 'identity',
						    'altkey'	=> 'name',
						    'default'	=> $element['agentfeatures']['context']['default'],
						    'selected'	=> $info['agentfeatures']['context']),
					      $context_list);
		else:
			echo	'<div id="fd-agentfeatures-context" class="txt-center">',
				$url->href_html($this->bbf('create_context'),
						'service/ipbx/system_management/context',
						'act=add'),
				'</div>';
		endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_agentfeatures_language'),
				    'name'	=> 'agentfeatures[language]',
				    'labelid'	=> 'agentfeatures-language',
				    'key'	=> false,
				    'default'	=> $element['agentfeatures']['language']['default'],
				    'selected'	=> $info['agentfeatures']['language']),
			      $element['agentfeatures']['language']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_agentfeatures_numgroup'),
				    'name'	=> 'agentfeatures[numgroup]',
				    'labelid'	=> 'agentfeatures-numgroup',
				    'browse'	=> 'agentgroup',
				    'key'	=> 'name',
				    'altkey'	=> 'id',
				    'default'	=> $this->get_var('group'),
				    'selected'	=> $info['agentfeatures']['numgroup']),
			      $this->get_var('agentgroup_list'));

	if(($moh_list = $this->get_var('moh_list')) !== false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_agentoptions_musiconhold'),
					    'name'	=> 'agentoptions[musiconhold]',
					    'labelid'	=> 'agentoptions-musiconhold',
					    'key'	=> 'category',
					    'default'	=> $element['agentoptions']['musiconhold']['default'],
					    'selected'	=> $info['agentoptions']['musiconhold']),
				      $moh_list);
	endif;
?>
</div>

<div id="sb-part-user" class="b-nodisplay">
<?php
	if($umember['list'] !== false):
?>
<div id="userlist" class="fm-paragraph fm-multilist">
	<div class="slt-outlist">
<?php
		echo	$form->select(array('name'	=> 'userlist',
					    'label'	=> false,
					    'id'	=> 'it-userlist',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false,
					    'key'	=> 'identity',
					    'altkey'	=> 'id'),
				      $umember['list']);
?>
	</div>

	<div class="inout-list">
		<a href="#"
		   onclick="dwho.form.move_selected('it-userlist','it-user');
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_inuser');?>">
			<?=$url->img_html('img/site/button/row-left.gif',
					  $this->bbf('bt_inuser'),
					  'class="bt-inlist" id="bt-inuser" border="0"');?></a><br />
		<a href="#"
		   onclick="dwho.form.move_selected('it-user','it-userlist');
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_outuser');?>">
			<?=$url->img_html('img/site/button/row-right.gif',
					  $this->bbf('bt_outuser'),
					  'class="bt-outlist" id="bt-outuser" border="0"');?></a>
	</div>

	<div class="slt-inlist">
<?php
		echo	$form->select(array('name'	=> 'user-select[]',
					    'label'	=> false,
					    'id'	=> 'it-user',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false,
					    'key'	=> 'identity',
					    'altkey'	=> 'id'),
				      $umember['slt']);
?>
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

<div id="sb-part-queue" class="b-nodisplay">
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
			<?=$url->img_html('img/site/button/row-left.gif',
					  $this->bbf('bt_inqueue'),
					  'class="bt-inlist" id="bt-inqueue" border="0"');?></a><br />
		<a href="#"
		   onclick="xivo_ast_outqueue();
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_outqueue');?>">
			<?=$url->img_html('img/site/button/row-right.gif',
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

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_agentfeatures_silent'),
				      'name'	=> 'agentfeatures[silent]',
				      'labelid'	=> 'agentfeatures-silent',
				      'default'	=> $element['agentfeatures']['silent']['default'],
				      'checked'	=> $info['agentfeatures']['silent'])),

		$form->select(array('desc'	=> $this->bbf('fm_agentoptions_ackcall'),
				    'name'	=> 'agentoptions[ackcall]',
				    'labelid'	=> 'agentoptions-ackcall',
				    'key'	=> false,
				    'bbf'	=> 'fm_agentoptions_ackcall-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['agentoptions']['ackcall']['default'],
				    'selected'	=> $info['agentoptions']['ackcall']),
			      $element['agentoptions']['ackcall']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_agentoptions_endcall'),
				      'name'	=> 'agentoptions[endcall]',
				      'labelid'	=> 'agentoptions-endcall',
				      'default'	=> $element['agentoptions']['endcall']['default'],
				      'checked' => $info['agentoptions']['endcall'])),

		$form->select(array('desc'	=> $this->bbf('fm_agentoptions_autologoff'),
				    'name'	=> 'agentoptions[autologoff]',
				    'labelid'	=> 'agentoptions-autologoff',
				    'key'	=> false,
				    'bbf'	=> 'fm_agentoptions_autologoff-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['agentoptions']['autologoff']['default'],
				    'selected'	=> $info['agentoptions']['autologoff']),
			      $element['agentoptions']['autologoff']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_agentoptions_autologoffunavail'),
				      'name'	=> 'agentoptions[autologoffunavail]',
				      'labelid'	=> 'agentoptions-autologoffunavail',
				      'default'	=> $element['agentoptions']['autologoffunavail']['default'],
				      'checked' => $info['agentoptions']['autologoffunavail'])),

		$form->select(array('desc'	=> $this->bbf('fm_agentoptions_wrapuptime'),
				    'name'	=> 'agentoptions[wrapuptime]',
				    'labelid'	=> 'agentoptions-wrapuptime',
				    'key'	=> false,
				    'bbf'	=> 'fm_agentoptions_wrapuptime-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'millisecond',
									'format'	=> '%s')),
				    'selected'	=> $info['agentoptions']['wrapuptime'],
				    'default'	=> $element['agentoptions']['wrapuptime']['default']),
			      $element['agentoptions']['wrapuptime']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_agentoptions_maxlogintries'),
				    'name'	=> 'agentoptions[maxlogintries]',
				    'labelid'	=> 'agentoptions-maxlogintries',
				    'key'	=> false,
				    'bbf'	=> 'fm_agentoptions_maxlogintries-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['agentoptions']['maxlogintries']['default'],
				    'selected'	=> $info['agentoptions']['maxlogintries']),
			      $element['agentoptions']['maxlogintries']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_agentoptions_updatecdr'),
				      'name'	=> 'agentoptions[updatecdr]',
				      'labelid'	=> 'agentoptions-updatecdr',
				      'default'	=> $element['agentoptions']['updatecdr']['default'],
				      'checked' => $info['agentoptions']['updatecdr'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_agentoptions_recordagentcalls'),
				      'name'	=> 'agentoptions[recordagentcalls]',
				      'labelid'	=> 'agentoptions-recordagentcalls',
				      'default'	=> $element['agentoptions']['recordagentcalls']['default'],
				      'checked' => $info['agentoptions']['recordagentcalls'])),

		$form->select(array('desc'	=> $this->bbf('fm_agentoptions_recordformat'),
				    'name'	=> 'agentoptions[recordformat]',
				    'labelid'	=> 'agentoptions-recordformat',
				    'key'	=> false,
				    'bbf'	=> 'ast_format_name_info',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['agentoptions']['recordformat']['default'],
				    'selected'	=> $info['agentoptions']['recordformat']),
			      $element['agentoptions']['recordformat']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_agentoptions_urlprefix'),
				  'name'	=> 'agentoptions[urlprefix]',
				  'labelid'	=> 'agentoptions-urlprefix',
				  'size'	=> 15,
				  'default'	=> $element['agentoptions']['urlprefix']['default'],
				  'value'	=> $info['agentoptions']['urlprefix'])),

		$form->select(array('desc'	=> $this->bbf('fm_agentoptions_custom_beep'),
				    'name'	=> 'agentoptions[custom_beep]',
				    'labelid'	=> 'agentoptions-custom-beep',
				    'empty'	=> $this->bbf('fm_agentoptions_custom-beep-opt','default'),
				    'default'	=> $element['agentoptions']['custom_beep']['default'],
				    'selected'	=> $info['agentoptions']['custom_beep']),
			      $this->get_var('beep_list')),

		$form->select(array('desc'	=> $this->bbf('fm_agentoptions_goodbye'),
				    'name'	=> 'agentoptions[goodbye]',
				    'labelid'	=> 'agentoptions-goodbye',
				    'empty'	=> $this->bbf('fm_agentoptions_goodbye-opt','default'),
				    'default'	=> $element['agentoptions']['goodbye']['default'],
				    'selected'	=> $info['agentoptions']['goodbye']),
			      $this->get_var('goodbye_list'));
?>
	<div class="fm-paragraph fm-description">
		<p>
			<label id="lb-agentfeatures-description" for="it-agentfeatures-description"><?=$this->bbf('fm_agentfeatures_description');?></label>
		</p>
		<?=$form->textarea(array('paragraph'	=> false,
					 'label'	=> false,
					 'name'		=> 'agentfeatures[description]',
					 'id'		=> 'it-agentfeatures-description',
					 'cols'		=> 60,
					 'rows'		=> 5,
					 'default'	=> $element['agentfeatures']['description']['default']),
				   $info['agentfeatures']['description']);?>
	</div>
</div>
