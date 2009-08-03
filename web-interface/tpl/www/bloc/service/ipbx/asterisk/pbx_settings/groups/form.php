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
$user = $this->get_var('user');
$rightcall = $this->get_var('rightcall');
$moh_list = $this->get_var('moh_list');
$context_list = $this->get_var('context_list');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first" class="b-nodisplay">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_groupfeatures_name'),
				  'name'	=> 'groupfeatures[name]',
				  'labelid'	=> 'groupfeatures-name',
				  'size'	=> 15,
				  'default'	=> $element['groupfeatures']['name']['default'],
				  'value'	=> $info['groupfeatures']['name'])),

		$form->text(array('desc'	=> $this->bbf('fm_groupfeatures_number'),
				  'name'	=> 'groupfeatures[number]',
				  'labelid'	=> 'groupfeatures-number',
				  'size'	=> 15,
				  'default'	=> $element['groupfeatures']['number']['default'],
				  'value' => $info['groupfeatures']['number'])),

		$form->select(array('desc'	=> $this->bbf('fm_queue_strategy'),
				    'name'	=> 'queue[strategy]',
				    'labelid'	=> 'queue-strategy',
				    'key'	=> false,
				    'bbf'	=> 'fm_queue_strategy-opt-',
				    'default'	=> $element['queue']['strategy']['default'],
				    'value'	=> $info['queue']['strategy']),
			      $element['queue']['strategy']['value']);

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_groupfeatures_context'),
				    'name'	=> 'groupfeatures[context]',
				    'labelid'	=> 'groupfeatures-context',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['groupfeatures']['context']['default'],
				    'value'	=> $info['groupfeatures']['context']),
			      $context_list);
else:
	echo	'<div id="fd-groupfeatures-context" class="txt-center">',
		$url->href_html($this->bbf('create_context'),
				'service/ipbx/system_management/context',
				'act=add'),
		'</div>';
endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_groupfeatures_timeout'),
				    'name'	=> 'groupfeatures[timeout]',
				    'labelid'	=> 'groupfeatures-timeout',
				    'bbf'	=> array('mixkey','fm_groupfeatures_timeout-opt'),
				    'key'	=> false,
				    'default'	=> $element['groupfeatures']['timeout']['default'],
				    'value'	=> $info['groupfeatures']['timeout']),
			      $element['groupfeatures']['timeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_queue_timeout'),
				    'name'	=> 'queue[timeout]',
				    'labelid'	=> 'queue-timeout',
				    'bbf'	=> array('mixkey','fm_queue_timeout-opt'),
				    'key'	=> false,
				    'default'	=> $element['queue']['timeout']['default'],
				    'value'	=> (isset($info['queue']['timeout']) === true ? (int) $info['queue']['timeout'] : null)),
			      $element['queue']['timeout']['value']);

if($moh_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_queue_musiconhold'),
				    'name'	=> 'queue[musiconhold]',
				    'labelid'	=> 'queue-musiconhold',
				    'key'	=> 'category',
				    'empty'	=> true,
				    'invalid'	=> ($this->get_var('act') === 'edit'),
				    'default'	=> ($this->get_var('act') === 'add' ? $element['queue']['musiconhold']['default'] : null),
				    'value'	=> $info['queue']['musiconhold']),
			      $moh_list);
endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_callerid_mode'),
				    'name'	=> 'callerid[mode]',
				    'labelid'	=> 'callerid-mode',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('paramkey','fm_callerid_mode-opt'),
				    'default'	=> $element['callerid']['mode']['default'],
				    'value'	=> $info['callerid']['mode']),
			      $element['callerid']['mode']['value'],
			      'onchange="xivo_ast_chg_callerid_mode(this);"'),

		$form->text(array('desc'	=> '&nbsp;',
				  'name'	=> 'callerid[callerdisplay]',
				  'labelid'	=> 'callerid-callerdisplay',
				  'size'	=> 15,
				  'notag'	=> false,
				  'default'	=> $element['callerid']['callerdisplay']['default'],
				  'value'	=> $info['callerid']['callerdisplay'])),

		$form->text(array('desc'	=> $this->bbf('fm_groupfeatures_preprocess-subroutine'),
				  'name'	=> 'groupfeatures[preprocess_subroutine]',
				  'labelid'	=> 'groupfeatures-preprocess-subroutine',
				  'size'	=> 15,
				  'default'	=> $element['groupfeatures']['preprocess_subroutine']['default'],
				  'value'	=> $info['groupfeatures']['preprocess_subroutine']));
?>
</div>

<div id="sb-part-user" class="b-nodisplay">
<?php
	if($user['list'] !== false):
?>
	<div id="userlist" class="fm-field fm-multilist">
		<div class="slt-outlist">
			<?=$form->select(array('name'		=> 'userlist',
					       'label'		=> false,
					       'id'		=> 'it-userlist',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false,
					       'key'		=> 'identity',
					       'altkey'		=> 'id'),
					 $user['list']);?>
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
			<?=$form->select(array('name'		=> 'user[]',
					       'label'		=> false,
					       'id'		=> 'it-user',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false,
					       'key'		=> 'identity',
					       'altkey'		=> 'id'),
					 $user['slt']);?>
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

<div id="sb-part-application" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_groupfeatures_transfer-user'),
				      'name'	=> 'groupfeatures[transfer_user]',
				      'labelid'	=> 'groupfeatures-transfer-user',
				      'default'	=> $element['groupfeatures']['transfer_user']['default'],
				      'checked'	=> $info['groupfeatures']['transfer_user'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_groupfeatures_transfer-call'),
				      'name'	=> 'groupfeatures[transfer_call]',
				      'labelid'	=> 'groupfeatures-transfer-call',
				      'default'	=> $element['groupfeatures']['transfer_call']['default'],
				      'checked'	=> $info['groupfeatures']['transfer_call'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_groupfeatures_write-caller'),
				      'name'	=> 'groupfeatures[write_caller]',
				      'labelid'	=> 'groupfeatures-write-caller',
				      'default'	=> $element['groupfeatures']['write_caller']['default'],
				      'checked' => $info['groupfeatures']['write_caller'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_groupfeatures_write-calling'),
				      'name'	=> 'groupfeatures[write_calling]',
				      'labelid'	=> 'groupfeatures-write-calling',
				      'default'	=> $element['groupfeatures']['write_calling']['default'],
				      'checked'	=> $info['groupfeatures']['write_calling']));
?>
</div>

<div id="sb-part-rightcall" class="b-nodisplay">
<?php
	if($rightcall['list'] !== false):
?>
	<div id="rightcalllist" class="fm-field fm-multilist">
		<div class="slt-outlist">
			<?=$form->select(array('name'		=> 'rightcalllist',
					       'label'		=> false,
					       'id'		=> 'it-rightcalllist',
					       'browse'		=> 'rightcall',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false),
					 $rightcall['list']);?>
		</div>

		<div class="inout-list">
			<a href="#"
			   onclick="xivo_fm_move_selected('it-rightcalllist','it-rightcall');
			            return(xivo_free_focus());"
			   title="<?=$this->bbf('bt_inrightcall');?>">
			   	<?=$url->img_html('img/site/button/row-left.gif',
						  $this->bbf('bt_inrightcall'),
						  'class="bt-inlist" id="bt-inrightcall" border="0"');?></a><br />
			<a href="#"
			   onclick="xivo_fm_move_selected('it-rightcall','it-rightcalllist');
			   	    return(xivo_free_focus());"
			   title="<?=$this->bbf('bt_outrightcall');?>">
			   	<?=$url->img_html('img/site/button/row-right.gif',
						  $this->bbf('bt_outrightcall'),
						  'class="bt-outlist" id="bt-outrightcall" border="0"');?></a>
		</div>

		<div class="slt-inlist">
			<?=$form->select(array('name'		=> 'rightcall[]',
					       'label'		=> false,
					       'id'		=> 'it-rightcall',
					       'browse'		=> 'rightcall',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false),
					 $rightcall['slt']);?>
		</div>
	</div>
	<div class="clearboth"></div>
<?php
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_rightcall'),
					'service/ipbx/call_management/rightcall',
					'act=add'),
			'</div>';
	endif;
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
	<fieldset id="fld-dialaction-noanswer">
		<legend><?=$this->bbf('fld-dialaction-noanswer');?></legend>
<?php
		$this->file_include('bloc/service/ipbx/asterisk/dialaction/all',
				    array('event'	=> 'noanswer'));
?>
	</fieldset>

	<fieldset id="fld-dialaction-busy">
		<legend><?=$this->bbf('fld-dialaction-busy');?></legend>
<?php
		$this->file_include('bloc/service/ipbx/asterisk/dialaction/all',
				    array('event'	=> 'busy'));
?>
	</fieldset>

	<fieldset id="fld-dialaction-congestion">
		<legend><?=$this->bbf('fld-dialaction-congestion');?></legend>
<?php
		$this->file_include('bloc/service/ipbx/asterisk/dialaction/all',
				    array('event'	=> 'congestion'));
?>
	</fieldset>

	<fieldset id="fld-dialaction-chanunavail">
		<legend><?=$this->bbf('fld-dialaction-chanunavail');?></legend>
<?php
		$this->file_include('bloc/service/ipbx/asterisk/dialaction/all',
				    array('event'	=> 'chanunavail'));
?>
	</fieldset>
</div>
