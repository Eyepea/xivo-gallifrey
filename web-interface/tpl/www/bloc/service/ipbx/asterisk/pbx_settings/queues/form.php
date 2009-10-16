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
$agentgroup = $this->get_var('agentgroup');
$agent = $this->get_var('agent');
$pannounce = $this->get_var('pannounce');
$moh_list = $this->get_var('moh_list');
$announce_list = $this->get_var('announce_list');
$context_list = $this->get_var('context_list');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_queuefeatures_name'),
				  'name'	=> 'queuefeatures[name]',
				  'labelid'	=> 'queuefeatures-name',
				  'size'	=> 15,
				  'default'	=> $element['queuefeatures']['name']['default'],
				  'value'	=> $info['queuefeatures']['name'])),

		$form->text(array('desc'	=> $this->bbf('fm_queuefeatures_number'),
				  'name'	=> 'queuefeatures[number]',
				  'labelid'	=> 'queuefeatures-number',
				  'size'	=> 15,
				  'default'	=> $element['queuefeatures']['number']['default'],
				  'value'	=> $info['queuefeatures']['number'])),

		$form->select(array('desc'	=> $this->bbf('fm_queue_strategy'),
				    'name'	=> 'queue[strategy]',
				    'labelid'	=> 'queue-strategy',
				    'key'	=> false,
				    'bbf'	=> 'fm_queue_strategy-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['queue']['strategy']['default'],
				    'selected'	=> $info['queue']['strategy']),
			      $element['queue']['strategy']['value']);

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_queuefeatures_context'),
				    'name'	=> 'queuefeatures[context]',
				    'labelid'	=> 'queuefeatures-context',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['queuefeatures']['context']['default'],
				    'selected'	=> $info['queuefeatures']['context']),
			      $context_list);
else:
	echo	'<div id="fd-queuefeatures-context" class="txt-center">',
		$url->href_html($this->bbf('create_context'),
				'service/ipbx/system_management/context',
				'act=add'),
		'</div>';
endif;

if($moh_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_queue_musiconhold'),
				    'name'	=> 'queue[musiconhold]',
				    'labelid'	=> 'queue-musiconhold',
				    'empty'	=> true,
				    'key'	=> 'category',
				    'invalid'	=> ($this->get_var('act') === 'edit'),
				    'default'	=> ($this->get_var('act') === 'add' ? $element['queue']['musiconhold']['default'] : null),
				    'selected'	=> $info['queue']['musiconhold']),
			      $moh_list);
endif;

if($announce_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_queue_announce'),
				    'name'	=> 'queue[announce]',
				    'labelid'	=> 'queue-announce',
				    'empty'	=> true,
				    'default'	=> $element['queue']['announce']['default'],
				    'selected'	=> $info['queue']['announce']),
			      $announce_list);
else:
	echo	'<div class="txt-center">',
		$url->href_html($this->bbf('add_announce'),
				'service/ipbx/pbx_services/sounds',
				array('act'	=> 'list',
				      'dir'	=> 'acd')),
		'</div>';
endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_callerid_mode'),
				    'name'	=> 'callerid[mode]',
				    'labelid'	=> 'callerid-mode',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_callerid_mode-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['callerid']['mode']['default'],
				    'selected'	=> $info['callerid']['mode']),
			      $element['callerid']['mode']['value'],
			      'onchange="xivo_ast_chg_callerid_mode(this);"'),

		$form->text(array('desc'	=> '&nbsp;',
				  'name'	=> 'callerid[callerdisplay]',
				  'labelid'	=> 'callerid-callerdisplay',
				  'size'	=> 15,
				  'notag'	=> false,
				  'default'	=> $element['callerid']['callerdisplay']['default'],
				  'value'	=> $info['callerid']['callerdisplay'])),

		$form->text(array('desc'	=> $this->bbf('fm_queuefeatures_preprocess-subroutine'),
				  'name'	=> 'queuefeatures[preprocess_subroutine]',
				  'labelid'	=> 'queuefeatures-preprocess-subroutine',
				  'size'	=> 15,
				  'default'	=> $element['queuefeatures']['preprocess_subroutine']['default'],
				  'value'	=> $info['queuefeatures']['preprocess_subroutine']));
?>
</div>

<div id="sb-part-announce" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_queue_announce-frequency'),
				    'name'	=> 'queue[announce-frequency]',
				    'labelid'	=> 'queue-announce-frequency',
				    'key'	=> false,
				    'bbf'	=> 'fm_queue_announce-frequency-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'default'	=> $element['queue']['announce-frequency']['default'],
				    'selected'	=> $info['queue']['announce-frequency']),
			      $element['queue']['announce-frequency']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_queue_announce-holdtime'),
				    'name'	=> 'queue[announce-holdtime]',
				    'labelid'	=> 'queue-announce-holdtime',
				    'key'	=> false,
				    'bbf'	=> 'fm_queue_announce-holdtime-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['queue']['announce-holdtime']['default'],
				    'selected'	=> $info['queue']['announce-holdtime']),
			      $element['queue']['announce-holdtime']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_queue_announce-round-seconds'),
				    'name'	=> 'queue[announce-round-seconds]',
				    'labelid'	=> 'queue-announce-round-seconds',
				    'key'	=> false,
				    'bbf'	=> 'fm_queue_announce-round-seconds-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['queue']['announce-round-seconds']['default'],
				    'selected'	=> $info['queue']['announce-round-seconds']),
			      $element['queue']['announce-round-seconds']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_queue_queue-youarenext'),
				    'name'	=> 'queue[queue-youarenext]',
				    'labelid'	=> 'queue-queue-youarenext',
				    'empty'	=> $this->bbf('fm_queue_queue-youarenext-opt','default'),
				    'default'	=> $element['queue']['queue-youarenext']['default'],
				    'selected'	=> $info['queue']['queue-youarenext']),
			      $announce_list),

		$form->select(array('desc'	=> $this->bbf('fm_queue_queue-thereare'),
				    'name'	=> 'queue[queue-thereare]',
				    'labelid'	=> 'queue-queue-thereare',
				    'empty'	=> $this->bbf('fm_queue_queue-thereare-opt','default'),
				    'default'	=> $element['queue']['queue-thereare']['default'],
				    'selected'	=> $info['queue']['queue-thereare']),
			      $announce_list),

		$form->select(array('desc'	=> $this->bbf('fm_queue_queue-callswaiting'),
				    'name'	=> 'queue[queue-callswaiting]',
				    'labelid'	=> 'queue-queue-callswaiting',
				    'empty'	=> $this->bbf('fm_queue_queue-callswaiting-opt','default'),
				    'default'	=> $element['queue']['queue-callswaiting']['default'],
				    'selected'	=> $info['queue']['queue-callswaiting']),
			      $announce_list),

		$form->select(array('desc'	=> $this->bbf('fm_queue_queue-holdtime'),
				    'name'	=> 'queue[queue-holdtime]',
				    'labelid'	=> 'queue-queue-holdtime',
				    'empty'	=> $this->bbf('fm_queue_queue-holdtime-opt','default'),
				    'default'	=> $element['queue']['queue-holdtime']['default'],
				    'selected'	=> $info['queue']['queue-holdtime']),
			      $announce_list),

		$form->select(array('desc'	=> $this->bbf('fm_queue_queue-minutes'),
				    'name'	=> 'queue[queue-minutes]',
				    'labelid'	=> 'queue-queue-minutes',
				    'empty'	=> $this->bbf('fm_queue_queue-minutes-opt','default'),
				    'default'	=> $element['queue']['queue-minutes']['default'],
				    'selected'	=> $info['queue']['queue-minutes']),
			      $announce_list),

		$form->select(array('desc'	=> $this->bbf('fm_queue_queue-seconds'),
				    'name'	=> 'queue[queue-seconds]',
				    'labelid'	=> 'queue-queue-seconds',
				    'empty'	=> $this->bbf('fm_queue_queue-seconds-opt','default'),
				    'default'	=> $element['queue']['queue-seconds']['default'],
				    'selected'	=> $info['queue']['queue-seconds']),
			      $announce_list),

		$form->select(array('desc'	=> $this->bbf('fm_queue_queue-thankyou'),
				    'name'	=> 'queue[queue-thankyou]',
				    'labelid'	=> 'queue-queue-thankyou',
				    'empty'	=> $this->bbf('fm_queue_queue-thankyou-opt','default'),
				    'default'	=> $element['queue']['queue-thankyou']['default'],
				    'selected'	=> $info['queue']['queue-thankyou']),
			      $announce_list),

		$form->select(array('desc'	=> $this->bbf('fm_queue_queue-lessthan'),
				    'name'	=> 'queue[queue-lessthan]',
				    'labelid'	=> 'queue-queue-lessthan',
				    'empty'	=> $this->bbf('fm_queue_queue-lessthan-opt','default'),
				    'default'	=> $element['queue']['queue-lessthan']['default'],
				    'selected'	=> $info['queue']['queue-lessthan']),
			      $announce_list),

		$form->select(array('desc'	=> $this->bbf('fm_queue_queue-reporthold'),
				    'name'	=> 'queue[queue-reporthold]',
				    'labelid'	=> 'queue-queue-reporthold',
				    'empty'	=> $this->bbf('fm_queue_queue-reporthold-opt','default'),
				    'default'	=> $element['queue']['queue-reporthold']['default'],
				    'selected'	=> $info['queue']['queue-reporthold']),
			      $announce_list);

	if(empty($announce_list) === false):
?>
<div id="pannouncelist" class="fm-paragraph fm-multilist">
	<p>
		<label id="lb-pannouncelist" for="it-pannouncelist" onclick="dwho_eid('it-pannouncelist').focus();">
			<?=$this->bbf('fm_queue_periodic-announce');?>
		</label>
	</p>
	<div class="slt-outlist">
<?php
		echo	$form->select(array('name'	=> 'pannouncelist',
					    'label'	=> false,
					    'id'	=> 'it-pannouncelist',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false),
				      $pannounce['list']);
?>
	</div>

	<div class="inout-list">
		<a href="#"
		   onclick="dwho.form.move_selected('it-pannouncelist',
						  'it-queue-periodic-announce');
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_inpannounce');?>">
			<?=$url->img_html('img/site/button/row-left.gif',
					  $this->bbf('bt_inpannounce'),
					  'class="bt-inlist" id="bt-inpannounce" border="0"');?></a><br />
		<a href="#"
		   onclick="dwho.form.move_selected('it-queue-periodic-announce',
						  'it-pannouncelist');
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_outpannounce');?>">
			<?=$url->img_html('img/site/button/row-right.gif',
					  $this->bbf('bt_outpannounce'),
					  'class="bt-outlist" id="bt-outpannounce" border="0"');?></a>
	</div>

	<div class="slt-inlist">
<?php
		echo	$form->select(array('name'	=> 'queue[periodic-announce][]',
					    'label'	=> false,
					    'id'	=> 'it-queue-periodic-announce',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false),
				      $pannounce['slt']);
?>
		<div class="bt-updown">
			<a href="#"
			   onclick="dwho.form.order_selected('it-queue-periodic-announce',1);
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_uppannounce');?>">
				<?=$url->img_html('img/site/button/row-up.gif',
						  $this->bbf('bt_uppannounce'),
						  'class="bt-uplist" id="bt-uppannounce" border="0"');?></a><br />
			<a href="#"
			   onclick="dwho.form.order_selected('it-queue-periodic-announce',-1);
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_downpannounce');?>">
				<?=$url->img_html('img/site/button/row-down.gif',
						  $this->bbf('bt_downpannounce'),
						  'class="bt-downlist" id="bt-downpannounce" border="0"');?></a>
		</div>
	</div>
</div>
<div class="clearboth"></div>
<?php
	else:
		echo	$form->select(array('desc'	=> $this->bbf('fm_queue_periodic-announce'),
					    'name'	=> 'queue[periodic-announce]',
					    'labelid'	=> 'queue-periodic-announce',
					    'empty'	=> $this->bbf('fm_queue_periodic-announce-opt','default'),
					    'default'	=> $element['queue']['periodic-announce']['default']));
	endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_queue_periodic-announce-frequency'),
				    'name'	=> 'queue[periodic-announce-frequency]',
				    'labelid'	=> 'queue-periodic-announce-frequency',
				    'key'	=> false,
				    'bbf'	=> 'fm_queue_announce-frequency-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'default'	=> $element['queue']['periodic-announce-frequency']['default'],
				    'selected'	=> $info['queue']['periodic-announce-frequency']),
			      $element['queue']['periodic-announce-frequency']['value']);
?>
</div>

<div id="sb-part-member" class="b-nodisplay">
	<fieldset id="fld-user">
		<legend><?=$this->bbf('fld-users');?></legend>
<?php
	if($user['list'] !== false):
?>
	<div id="userlist" class="fm-paragraph fm-multilist">
		<div class="slt-outlist">
			<?=$form->select(array('name'		=> 'userlist',
					       'label'		=> false,
					       'id'		=> 'it-userlist',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'paragraph'	=> false,
					       'key'		=> 'identity',
					       'altkey'		=> 'id'),
					 $user['list']);?>
		</div>

		<div class="inout-list">
			<a href="#"
			   onclick="dwho.form.move_selected('it-userlist',
							  'it-user');
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_inuser');?>">
				<?=$url->img_html('img/site/button/row-left.gif',
						  $this->bbf('bt_inuser'),
						  'class="bt-inlist" id="bt-inuser" border="0"');?></a><br />
			<a href="#"
			   onclick="dwho.form.move_selected('it-user',
							  'it-userlist');
				    return(dwho.dom.free_focus());"
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
					       'paragraph'	=> false,
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
	</fieldset>
	<fieldset id="fld-agent">
		<legend><?=$this->bbf('fld-agents');?></legend>
<?php
	if($agentgroup['list'] !== false):
?>
		<div id="agentgrouplist" class="fm-paragraph fm-multilist">
			<p>
				<label id="lb-agentgrouplist" for="it-agentgrouplist">
					<?=$this->bbf('fm_agentgroup');?>
				</label>
			</p>

			<div class="slt-outlist">
				<?=$form->select(array('name'		=> 'agentgrouplist',
						       'label'		=> false,
						       'id'		=> 'it-agentgrouplist',
						       'multiple'	=> true,
						       'size'		=> 5,
						       'paragraph'	=> false,
						       'browse'		=> 'agentgroup',
						       'key'		=> 'name',
						       'altkey'		=> 'id'),
						 $agentgroup['list']);?>
			</div>

			<div class="inout-list">
				<a href="#"
				   onclick="dwho.form.move_selected('it-agentgrouplist',
								  'it-agentgroup');
					    return(dwho.dom.free_focus());"
				   title="<?=$this->bbf('bt_inagentgroup');?>">
					<?=$url->img_html('img/site/button/row-left.gif',
							  $this->bbf('bt_inagentgroup'),
							  'class="bt-inlist" id="bt-inagentgroup" border="0"');?></a><br />
				<a href="#"
				   onclick="dwho.form.move_selected('it-agentgroup',
								  'it-agentgrouplist');
					    return(dwho.dom.free_focus());"
				   title="<?=$this->bbf('bt_outagentgroup');?>">
					<?=$url->img_html('img/site/button/row-right.gif',
							  $this->bbf('bt_outagentgroup'),
							  'class="bt-outlist" id="bt-outagentgroup" border="0"');?></a>
			</div>

			<div class="slt-inlist">
				<?=$form->select(array('name'		=> 'agentgroup[]',
						       'label'		=> false,
						       'id'		=> 'it-agentgroup',
						       'multiple'	=> true,
						       'size'		=> 5,
						       'paragraph'	=> false,
						       'browse'		=> 'agentgroup',
						       'key'		=> 'name',
						       'altkey'		=> 'id'),
						 $agentgroup['slt']);?>
			</div>
		</div>
		<div class="clearboth"></div>
<?php
		if($agent['list'] !== false):
?>
			<div id="agentlist" class="fm-paragraph fm-multilist">
				<p>
					<label id="lb-agentlist" for="it-agentlist">
						<?=$this->bbf('fm_agent');?>
					</label>
				</p>

				<div class="slt-outlist">
					<?=$form->select(array('name'		=> 'agentlist',
							       'label'		=> false,
							       'id'		=> 'it-agentlist',
							       'multiple'	=> true,
							       'size'		=> 5,
							       'paragraph'	=> false,
							       'browse'		=> 'agentfeatures',
							       'key'		=> 'identity',
							       'altkey'		=> 'id'),
							 $agent['list']);?>
				</div>

				<div class="inout-list">
					<a href="#"
					   onclick="dwho.form.move_selected('it-agentlist',
									  'it-agent');
						    return(dwho.dom.free_focus());"
					   title="<?=$this->bbf('bt_inagent');?>">
						<?=$url->img_html('img/site/button/row-left.gif',
								  $this->bbf('bt_inagent'),
								  'class="bt-inlist" id="bt-inagent" border="0"');?></a><br />
					<a href="#"
					   onclick="dwho.form.move_selected('it-agent',
									  'it-agentlist');
						    return(dwho.dom.free_focus());"
					   title="<?=$this->bbf('bt_outagent');?>">
						<?=$url->img_html('img/site/button/row-right.gif',
								  $this->bbf('bt_outagent'),
								  'class="bt-outlist" id="bt-outagent" border="0"');?></a>
				</div>

				<div class="slt-inlist">
					<?=$form->select(array('name'		=> 'agent[]',
							       'label'		=> false,
							       'id'		=> 'it-agent',
							       'multiple'	=> true,
							       'size'		=> 5,
							       'paragraph'	=> false,
							       'browse'		=> 'agentfeatures',
							       'key'		=> 'identity',
							       'altkey'		=> 'id'),
							 $agent['slt']);?>
				</div>
			</div>
			<div class="clearboth"></div>
<?php
		else:
			echo	'<div id="create-agent" class="txt-center">',
					$url->href_html($this->bbf('create_agent'),
							'service/ipbx/pbx_settings/agents',
							'act=addagent'),
				'</div>';
		endif;

	else:
		echo	'<div class="txt-center">',
				$url->href_html($this->bbf('create_agent-group'),
						'service/ipbx/pbx_settings/agents',
						'act=add'),
			'</div>';
	endif;
?>
	</fieldset>
</div>

<div id="sb-part-application" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_queuefeatures_timeout'),
				    'name'	=> 'queuefeatures[timeout]',
				    'labelid'	=> 'queuefeatures-timeout',
				    'key'	=> false,
				    'bbf'	=> 'fm_queuefeatures_timeout-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'default'	=> $element['queuefeatures']['timeout']['default'],
				    'selected'	=> $info['queuefeatures']['timeout']),
			      $element['queuefeatures']['timeout']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_queuefeatures_data-quality'),
				      'name'	=> 'queuefeatures[data_quality]',
				      'labelid'	=> 'queuefeatures-data-quality',
				      'default'	=> $element['queuefeatures']['data_quality']['default'],
				      'checked'	=> $info['queuefeatures']['data_quality'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_queuefeatures_hitting-callee'),
				      'name'	=> 'queuefeatures[hitting_callee]',
				      'labelid'	=> 'queuefeatures-hitting-callee',
				      'default'	=> $element['queuefeatures']['hitting_callee']['default'],
				      'checked'	=> $info['queuefeatures']['hitting_callee'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_queuefeatures_hitting-caller'),
				      'name'	=> 'queuefeatures[hitting_caller]',
				      'labelid'	=> 'queuefeatures-hitting-caller',
				      'default'	=> $element['queuefeatures']['hitting_caller']['default'],
				      'checked'	=> $info['queuefeatures']['hitting_caller'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_queuefeatures_retries'),
				      'name'	=> 'queuefeatures[retries]',
				      'labelid'	=> 'queuefeatures-retries',
				      'default'	=> $element['queuefeatures']['retries']['default'],
				      'checked'	=> $info['queuefeatures']['retries'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_queuefeatures_ring'),
				      'name'	=> 'queuefeatures[ring]',
				      'labelid'	=> 'queuefeatures-ring',
				      'default'	=> $element['queuefeatures']['ring']['default'],
				      'checked'	=> $info['queuefeatures']['ring'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_queuefeatures_transfer-user'),
				      'name'	=> 'queuefeatures[transfer_user]',
				      'labelid'	=> 'queuefeatures-transfer-user',
				      'default'	=> $element['queuefeatures']['transfer_user']['default'],
				      'checked'	=> $info['queuefeatures']['transfer_user'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_queuefeatures_transfer-call'),
				      'name'	=> 'queuefeatures[transfer_call]',
				      'labelid'	=> 'queuefeatures-transfer-call',
				      'default'	=> $element['queuefeatures']['transfer_call']['default'],
				      'checked'	=> $info['queuefeatures']['transfer_call'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_queuefeatures_write-caller'),
				      'name'	=> 'queuefeatures[write_caller]',
				      'labelid'	=> 'queuefeatures-write-caller',
				      'default'	=> $element['queuefeatures']['write_caller']['default'],
				      'checked'	=> $info['queuefeatures']['write_caller'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_queuefeatures_write-calling'),
				      'name'	=> 'queuefeatures[write_calling]',
				      'labelid'	=> 'queuefeatures-write-calling',
				      'default'	=> $element['queuefeatures']['write_calling']['default'],
				      'checked'	=> $info['queuefeatures']['write_calling']));
?>
</div>

<div id="sb-part-dialaction" class="b-nodisplay">
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

<div id="sb-part-last" class="b-nodisplay">
<?php

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_queue_context'),
				    'name'	=> 'queue[context]',
				    'labelid'	=> 'queue-context',
				    'empty'	=> true,
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['queue']['context']['default'],
				    'selected'	=> $info['queue']['context']),
			      $context_list);
endif;

	echo	$form->text(array('desc'	=> $this->bbf('fm_queue_servicelevel'),
				  'name'	=> 'queue[servicelevel]',
				  'labelid'	=> 'queue-servicelevel',
				  'size'	=> 15,
				  'default'	=> $element['queue']['servicelevel']['default'],
				  'value'	=> $info['queue']['servicelevel'])),

		$form->select(array('desc'	=> $this->bbf('fm_queue_timeout'),
				    'name'	=> 'queue[timeout]',
				    'labelid'	=> 'queue-timeout',
				    'key'	=> false,
				    'bbf'	=> 'fm_queue_timeout-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['queue']['timeout']['default'],
				    'selected'	=> $info['queue']['timeout']),
			      $element['queue']['timeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_queue_retry'),
				    'name'	=> 'queue[retry]',
				    'labelid'	=> 'queue-retry',
				    'key'	=> false,
				    'bbf'	=> 'fm_queue_retry-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['queue']['retry']['default'],
				    'selected'	=> $info['queue']['retry']),
			      $element['queue']['retry']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_queue_weight'),
				    'name'	=> 'queue[weight]',
				    'labelid'	=> 'queue-weight',
				    'key'	=> false,
				    'default'	=> $element['queue']['weight']['default'],
				    'selected'	=> $info['queue']['weight']),
			      $element['queue']['weight']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_queue_wrapuptime'),
				    'name'	=> 'queue[wrapuptime]',
				    'labelid'	=> 'queue-wrapuptime',
				    'key'	=> false,
				    'bbf'	=> 'fm_queue_wrapuptime-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['queue']['wrapuptime']['default'],
				    'selected'	=> $info['queue']['wrapuptime']),
			      $element['queue']['wrapuptime']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_queue_maxlen'),
				  'name'	=> 'queue[maxlen]',
				  'labelid'	=> 'queue-maxlen',
				  'size'	=> 5,
				  'default'	=> $element['queue']['maxlen']['default'],
				  'value'	=> $info['queue']['maxlen'])),

		$form->select(array('desc'	=> $this->bbf('fm_queue_monitor-type'),
				    'name'	=> 'queue[monitor-type]',
				    'labelid'	=> 'queue-monitor-type',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_queue_monitor-type-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['queue']['monitor-type']['default'],
				    'selected'	=> $info['queue']['monitor-type']),
			      $element['queue']['monitor-type']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_queue_monitor-format'),
				    'name'	=> 'queue[monitor-format]',
				    'labelid'	=> 'queue-monitor-format',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'ast_format_name_info',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['queue']['monitor-format']['default'],
				    'selected'	=> $info['queue']['monitor-format']),
			      $element['queue']['monitor-format']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_queue_joinempty'),
				    'name'	=> 'queue[joinempty]',
				    'labelid'	=> 'queue-joinempty',
				    'key'	=> false,
				    'bbf'	=> 'fm_queue_joinempty-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['queue']['joinempty']['default'],
				    'selected'	=> $info['queue']['joinempty']),
			      $element['queue']['joinempty']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_queue_leavewhenempty'),
				    'name'	=> 'queue[leavewhenempty]',
				    'labelid'	=> 'queue-leavewhenempty',
				    'key'	=> false,
				    'bbf'	=> 'fm_queue_leavewhenempty-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['queue']['leavewhenempty']['default'],
				    'selected'	=> $info['queue']['leavewhenempty']),
			      $element['queue']['leavewhenempty']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_queue_ringinuse'),
				      'name'	=> 'queue[ringinuse]',
				      'labelid'	=> 'queue-ringinuse',
				      'default'	=> $element['queue']['ringinuse']['default'],
				      'checked'	=> $info['queue']['ringinuse'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_queue_eventwhencalled'),
				      'name'	=> 'queue[eventwhencalled]',
				      'labelid'	=> 'queue-eventwhencalled',
				      'default'	=> $element['queue']['eventwhencalled']['default'],
				      'checked'	=> $info['queue']['eventwhencalled']),
				'disabled="disabled"'),

		$form->checkbox(array('desc'	=> $this->bbf('fm_queue_eventmemberstatus'),
				      'name'	=> 'queue[eventmemberstatus]',
				      'labelid'	=> 'queue-eventmemberstatus',
				      'default'	=> $element['queue']['eventmemberstatus']['default'],
				      'checked'	=> $info['queue']['eventmemberstatus']),
				'disabled="disabled"'),

		$form->checkbox(array('desc'	=> $this->bbf('fm_queue_reportholdtime'),
				      'name'	=> 'queue[reportholdtime]',
				      'labelid'	=> 'queue-reportholdtime',
				      'default'	=> $element['queue']['reportholdtime']['default'],
				      'checked'	=> $info['queue']['reportholdtime'])),

		$form->select(array('desc'	=> $this->bbf('fm_queue_memberdelay'),
				    'name'	=> 'queue[memberdelay]',
				    'labelid'	=> 'queue-memberdelay',
				    'key'	=> false,
				    'bbf'	=> 'fm_queue_memberdelay-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['queue']['memberdelay']['default'],
				    'selected'	=> $info['queue']['memberdelay']),
			      $element['queue']['memberdelay']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_queue_timeoutrestart'),
				      'name'	=> 'queue[timeoutrestart]',
				      'labelid'	=> 'queue-timeoutrestart',
				      'default'	=> $element['queue']['timeoutrestart']['default'],
				      'checked' => $info['queue']['timeoutrestart']));
?>
</div>
