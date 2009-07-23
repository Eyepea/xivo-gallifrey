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
				  'name'	=> 'afeatures[firstname]',
				  'labelid'	=> 'afeatures-firstname',
				  'size'	=> 15,
				  'default'	=> $element['afeatures']['firstname']['default'],
				  'value'	=> $info['afeatures']['firstname'])),

		$form->text(array('desc'	=> $this->bbf('fm_agentfeatures_lastname'),
				  'name'	=> 'afeatures[lastname]',
				  'labelid'	=> 'afeatures-lastname',
				  'size'	=> 15,
				  'default'	=> $element['afeatures']['lastname']['default'],
				  'value'	=> $info['afeatures']['lastname'])),

		$form->text(array('desc'	=> $this->bbf('fm_agentfeatures_number'),
				  'name'	=> 'afeatures[number]',
				  'labelid'	=> 'afeatures-number',
				  'size'	=> 15,
				  'default'	=> $element['afeatures']['number']['default'],
				  'value'	=> $info['afeatures']['number'])),

		$form->text(array('desc'	=> $this->bbf('fm_agentfeatures_passwd'),
				  'name'	=> 'afeatures[passwd]',
				  'labelid'	=> 'afeatures-passwd',
				  'size'	=> 15,
				  'default'	=> $element['afeatures']['passwd']['default'],
				  'value'	=> $info['afeatures']['passwd']));

		if($context_list !== false):
			echo	$form->select(array('desc'	=> $this->bbf('fm_agentfeatures_context'),
						    'name'	=> 'afeatures[context]',
						    'labelid'	=> 'afeatures-context',
						    'key'	=> 'identity',
						    'altkey'	=> 'name',
						    'default'	=> $element['afeatures']['context']['default'],
						    'value'	=> $info['afeatures']['context']),
					      $context_list);
		else:
			echo	'<div id="fd-afeatures-context" class="txt-center">',
				$url->href_html($this->bbf('create_context'),
						'service/ipbx/system_management/context',
						'act=add'),
				'</div>';
		endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_agentfeatures_language'),
				    'name'	=> 'afeatures[language]',
				    'labelid'	=> 'afeatures-language',
				    'key'	=> false,
				    'default'	=> $element['afeatures']['language']['default'],
				    'value'	=> $info['afeatures']['language']),
			      $element['afeatures']['language']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_agentfeatures_numgroup'),
				    'name'	=> 'afeatures[numgroup]',
				    'labelid'	=> 'afeatures-numgroup',
				    'browse'	=> 'agentgroup',
				    'key'	=> 'name',
				    'altkey'	=> 'id',
				    'default'	=> $this->get_var('group'),
				    'value'	=> $info['afeatures']['numgroup']),
			      $this->get_var('agentgroup_list'));

	if(($moh_list = $this->get_var('moh_list')) !== false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_agentoptions_musiconhold'),
					    'name'	=> 'agentoptions[musiconhold]',
					    'labelid'	=> 'agentoptions-musiconhold',
					    'key'	=> 'category',
					    'default'	=> $element['agentoptions']['musiconhold']['default'],
					    'value'	=> $info['agentoptions']['musiconhold']),
				      $moh_list);
	endif;
?>
</div>

<div id="sb-part-user" class="b-nodisplay">
<?php
	if($umember['list'] !== false):
?>
<div id="userlist" class="fm-field fm-multilist">
	<div class="slt-outlist">
<?php
		echo	$form->select(array('name'	=> 'userlist',
					    'label'	=> false,
					    'id'	=> 'it-userlist',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'field'	=> false,
					    'key'	=> 'identity',
					    'altkey'	=> 'id'),
				      $umember['list']);
?>
	</div>

	<div class="inout-list">
		<a href="#"
		   onclick="xivo_fm_move_selected('it-userlist','it-user');
			    return(xivo_free_focus());"
		   title="<?=$this->bbf('bt_inuser');?>">
			<?=$url->img_html('img/site/button/row-left.gif',
					  $this->bbf('bt_inuser'),
					  'class="bt-inlist" id="bt-inuser" border="0"');?></a><br />
		<a href="#"
		   onclick="xivo_fm_move_selected('it-user','it-userlist');
			    return(xivo_free_focus());"
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
					    'field'	=> false,
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
<div id="queuelist" class="fm-field fm-multilist">
	<div class="slt-outlist">
<?php
		echo	$form->select(array('name'	=> 'queuelist',
					    'label'	=> false,
					    'id'	=> 'it-queuelist',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'field'	=> false,
					    'key'	=> 'name',
					    'altkey'	=> 'name'),
				      $qmember['list']);
?>
	</div>

	<div class="inout-list">
		<a href="#"
		   onclick="xivo_ast_inqueue();
			    return(xivo_free_focus());"
		   title="<?=$this->bbf('bt_inqueue');?>">
			<?=$url->img_html('img/site/button/row-left.gif',
					  $this->bbf('bt_inqueue'),
					  'class="bt-inlist" id="bt-inqueue" border="0"');?></a><br />
		<a href="#"
		   onclick="xivo_ast_outqueue();
			    return(xivo_free_focus());"
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
					    'field'	=> false,
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

			if(xivo_issa($value['id'],$qmember['info']) === true):
				$class = '';
				$value['member'] = $qmember['info'][$value['id']];
				$penalty = intval($value['member']['penalty']);
			else:
				$class = ' b-nodisplay';
				$value['member'] = null;
				$penalty = '';
			endif;

		echo	'<tr id="queue-',$name,'" class="fm-field',$class,'">',"\n",
			'<td class="td-left">',$name,'</td>',"\n",
			'<td class="td-right">',
			$form->select(array('field'	=> false,
					    'name'	=> 'queue['.$name.'][penalty]',
					    'id'	=> false,
					    'label'	=> false,
					    'default'	=> $element['qmember']['penalty']['default'],
					    'value'	=> $penalty),
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
				      'name'	=> 'afeatures[silent]',
				      'labelid'	=> 'afeatures-silent',
				      'default'	=> $element['afeatures']['silent']['default'],
				      'checked'	=> $info['afeatures']['silent'])),

		$form->select(array('desc'	=> $this->bbf('fm_agentoptions_ackcall'),
				    'name'	=> 'agentoptions[ackcall]',
				    'labelid'	=> 'agentoptions-ackcall',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_agentoptions_ackcall-opt'),
				    'default'	=> $element['agentoptions']['ackcall']['default'],
				    'value'	=> $info['agentoptions']['ackcall']),
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
				    'bbf'	=> array('mixkey','fm_agentoptions_autologoff-opt'),
				    'default'	=> $element['agentoptions']['autologoff']['default'],
				    'value'	=> $info['agentoptions']['autologoff']),
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
				    'bbf_opt'	=> array('argmode'	=> 'paramkey',
				    			 'time'		=> array(
							 		'from'		=> 'millisecond',
							 		'format'	=> '%s')),
				    'value'	=> $info['agentoptions']['wrapuptime'],
				    'default'	=> $element['agentoptions']['wrapuptime']['default']),
			      $element['agentoptions']['wrapuptime']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_agentoptions_maxlogintries'),
				    'name'	=> 'agentoptions[maxlogintries]',
				    'labelid'	=> 'agentoptions-maxlogintries',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_agentoptions_maxlogintries-opt'),
				    'default'	=> $element['agentoptions']['maxlogintries']['default'],
				    'value'	=> $info['agentoptions']['maxlogintries']),
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
				    'bbf'	=> 'ast_format_name_info-',
				    'default'	=> $element['agentoptions']['recordformat']['default'],
				    'value'	=> $info['agentoptions']['recordformat']),
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
				    'empty'	=> $this->bbf('fm_agentoptions_custom-beep-opt(default)'),
				    'default'	=> $element['agentoptions']['custom_beep']['default'],
				    'value'	=> $info['agentoptions']['custom_beep']),
			      $this->get_var('beep_list')),

		$form->select(array('desc'	=> $this->bbf('fm_agentoptions_goodbye'),
				    'name'	=> 'agentoptions[goodbye]',
				    'labelid'	=> 'agentoptions-goodbye',
				    'empty'	=> $this->bbf('fm_agentoptions_goodbye-opt(default)'),
				    'default'	=> $element['agentoptions']['goodbye']['default'],
				    'value'	=> $info['agentoptions']['goodbye']),
			      $this->get_var('goodbye_list'));
?>
	<div class="fm-field fm-description">
		<p>
			<label id="lb-afeatures-description" for="it-afeatures-description"><?=$this->bbf('fm_agentfeatures_description');?></label>
		</p>
		<?=$form->textarea(array('field'	=> false,
					 'label'	=> false,
					 'name'		=> 'afeatures[description]',
					 'id'		=> 'it-afeatures-description',
					 'cols'		=> 60,
					 'rows'		=> 5,
					 'default'	=> $element['afeatures']['description']['default']),
				   $info['afeatures']['description']);?>
	</div>
</div>
