<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');
	$element = $this->vars('element');
	$info = $this->vars('info');

	$user_slt = $this->vars('user_slt');
	$user_list = $this->vars('user_list');
	$agent_slt = $this->vars('agent_slt');
	$agent_list = $this->vars('agent_list');
	$agroup_slt = $this->vars('agroup_slt');
	$agroup_list = $this->vars('agroup_list');
	$moh_list = $this->vars('moh_list');
	$announce_list = $this->vars('announce_list');
?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_qfeatures_name'),'name' => 'qfeatures[name]','labelid' => 'qfeatures-name','size' => 15,'default' => $element['qfeatures']['name']['default'],'value' => $info['qfeatures']['name']));?>

<?=$form->text(array('desc' => $this->bbf('fm_qfeatures_number'),'name' => 'qfeatures[number]','labelid' => 'qfeatures-number','size' => 15,'default' => $element['qfeatures']['number']['default'],'value' => $info['qfeatures']['number']));?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_strategy'),'name' => 'queue[strategy]','labelid' => 'queue-strategy','key' => false,'default' => $element['queue']['strategy']['default'],'value' => $info['queue']['strategy']),$element['queue']['strategy']['value']);?>

<?php

if($moh_list !== false):

	echo $form->select(array('desc' => $this->bbf('fm_queue_musiconhold'),'name' => 'queue[musiconhold]','labelid' => 'queue-musiconhold','key' => 'category','empty' => true,'default' => $element['queue']['musiconhold']['default'],'value' => $info['queue']['musiconhold']),$moh_list);
	
endif;

if($announce_list !== false):
	echo $form->select(array('desc' => $this->bbf('fm_queue_announce'),'name' => 'queue[announce]','labelid' => 'queue-announce','key' => false,'empty' => true,'default' => $element['queue']['announce']['default'],'value' => $info['queue']['announce']),$announce_list);
else:
	echo '<div class="txt-center">',$url->href_html($this->bbf('add_announce'),'service/ipbx/general_settings/sounds',array('act' => 'list','dir' => 'acd')),'</div>';
endif;

?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_periodic-announce'),'name' => 'queue[periodic-announce]','labelid' => 'queue-periodic-announce','empty' => $this->bbf('fm_queue_periodic-announce-opt-default'),'default' => $element['queue']['periodic-announce']['default'],'value' => $info['queue']['periodic-announce']),$announce_list);?>

</div>

<div id="sb-part-announce" class="b-nodisplay">

<?=$form->select(array('desc' => $this->bbf('fm_queue_announce-frequency'),'name' => 'queue[announce-frequency]','labelid' => 'queue-announce-frequency','bbf' => array('mixkey','fm_queue_announce-frequency-opt','paramarray'),'default' => $element['queue']['announce-frequency']['default'],'value' => $info['queue']['announce-frequency']),$element['queue']['announce-frequency']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_periodic-announce-frequency'),'name' => 'queue[periodic-announce-frequency]','labelid' => 'queue-periodic-announce-frequency','bbf' => array('mixkey','fm_queue_periodic-announce-frequency-opt','paramarray'),'default' => $element['queue']['periodic-announce-frequency']['default'],'value' => $info['queue']['periodic-announce-frequency']),$element['queue']['periodic-announce-frequency']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_announce-holdtime'),'name' => 'queue[announce-holdtime]','labelid' => 'queue-announce-holdtime','bbf' => 'fm_queue_announce-holdtime-opt-','key' => false,'default' => $element['queue']['announce-holdtime']['default'],'value' => $info['queue']['announce-holdtime']),$element['queue']['announce-holdtime']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_announce-round-seconds'),'name' => 'queue[announce-round-seconds]','labelid' => 'queue-announce-round-seconds','bbf' => array('mixkey','fm_queue_announce-round-seconds-opt'),'key' => false,'default' => $element['queue']['announce-round-seconds']['default'],'value' => $info['queue']['announce-round-seconds']),$element['queue']['announce-round-seconds']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_queue-youarenext'),'name' => 'queue[queue-youarenext]','labelid' => 'queue-queue-youarenext','empty' => $this->bbf('fm_queue_queue-youarenext-opt-default'),'default' => $element['queue']['queue-youarenext']['default'],'value' => $info['queue']['queue-youarenext']),$announce_list);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_queue-thereare'),'name' => 'queue[queue-thereare]','labelid' => 'queue-queue-thereare','empty' => $this->bbf('fm_queue_queue-thereare-opt-default'),'default' => $element['queue']['queue-thereare']['default'],'value' => $info['queue']['queue-thereare']),$announce_list);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_queue-callswaiting'),'name' => 'queue[queue-callswaiting]','labelid' => 'queue-queue-callswaiting','empty' => $this->bbf('fm_queue_queue-callswaiting-opt-default'),'default' => $element['queue']['queue-callswaiting']['default'],'value' => $info['queue']['queue-callswaiting']),$announce_list);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_queue-holdtime'),'name' => 'queue[queue-holdtime]','labelid' => 'queue-queue-holdtime','empty' => $this->bbf('fm_queue_queue-holdtime-opt-default'),'default' => $element['queue']['queue-holdtime']['default'],'value' => $info['queue']['queue-holdtime']),$announce_list);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_queue-minutes'),'name' => 'queue[queue-minutes]','labelid' => 'queue-queue-minutes','empty' => $this->bbf('fm_queue_queue-minutes-opt-default'),'default' => $element['queue']['queue-minutes']['default'],'value' => $info['queue']['queue-minutes']),$announce_list);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_queue-seconds'),'name' => 'queue[queue-seconds]','labelid' => 'queue-queue-seconds','empty' => $this->bbf('fm_queue_queue-seconds-opt-default'),'default' => $element['queue']['queue-seconds']['default'],'value' => $info['queue']['queue-seconds']),$announce_list);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_queue-thankyou'),'name' => 'queue[queue-thankyou]','labelid' => 'queue-queue-thankyou','empty' => $this->bbf('fm_queue_queue-thankyou-opt-default'),'default' => $element['queue']['queue-thankyou']['default'],'value' => $info['queue']['queue-thankyou']),$announce_list);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_queue-lessthan'),'name' => 'queue[queue-lessthan]','labelid' => 'queue-queue-lessthan','empty' => $this->bbf('fm_queue_queue-lessthan-opt-default'),'default' => $element['queue']['queue-lessthan']['default'],'value' => $info['queue']['queue-lessthan']),$announce_list);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_queue-reporthold'),'name' => 'queue[queue-reporthold]','labelid' => 'queue-queue-reporthold','empty' => $this->bbf('fm_queue_queue-reporthold-opt-default'),'default' => $element['queue']['queue-reporthold']['default'],'value' => $info['queue']['queue-reporthold']),$announce_list);?>

</div>

<div id="sb-part-member" class="b-nodisplay">
	<fieldset id="fld-user">
		<legend><?=$this->bbf('fld-users');?></legend>
<?php
	if($user_list !== false):
?>
		<div id="userlist" class="fm-field fm-multilist">
			<div class="slt-outlist">

		<?=$form->select(array('name' => 'userlist','label' => false,'id' => 'it-userlist','multiple' => true,'size' => 5,'field' => false,'key' => 'fullname'),$user_list);?>

			</div>
			<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-userlist','it-user'); return(false);" title="<?=$this->bbf('bt-inuser');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-inuser'),'class="bt-inlist" id="bt-inuser" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-user','it-userlist'); return(false);" title="<?=$this->bbf('bt-outuser');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outuser'),'class="bt-outlist" id="bt-outuser" border="0"');?></a>

			</div>
			<div class="slt-inlist">

		<?=$form->select(array('name' => 'user[]','label' => false,'id' => 'it-user','multiple' => true,'size' => 5,'field' => false,'key' => 'fullname'),$user_slt);?>

			</div>
		</div>
		<div class="clearboth"></div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_user'),'service/ipbx/pbx_settings/users','act=add'),'</div>';
	endif;
?>
	</fieldset>
	<fieldset id="fld-agent">
		<legend><?=$this->bbf('fld-agents');?></legend>
<?php
	if($agroup_list !== false):
?>
		<div id="agrouplist" class="fm-field fm-multilist"><p><label id="lb-agrouplist" for="it-agrouplist"><?=$this->bbf('fm_agentgroup');?></label></p>
			<div class="slt-outlist">

		<?=$form->select(array('name' => 'agrouplist','label' => false,'id' => 'it-agrouplist','multiple' => true,'size' => 5,'field' => false,'key' => 'fullname'),$agroup_list);?>

			</div>
			<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-agrouplist','it-agroup'); return(false);" title="<?=$this->bbf('bt-inagroup');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-inagroup'),'class="bt-inlist" id="bt-inagroup" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-agroup','it-agrouplist'); return(false);" title="<?=$this->bbf('bt-outagroup');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outagroup'),'class="bt-outlist" id="bt-outagroup" border="0"');?></a>

			</div>
			<div class="slt-inlist">

		<?=$form->select(array('name' => 'agroup[]','label' => false,'id' => 'it-agroup','multiple' => true,'size' => 5,'field' => false,'key' => 'fullname'),$agroup_slt);?>

			</div>
		</div>
		<div class="clearboth"></div>
<?php
		if($agent_list !== false):
?>
		<div id="agentlist" class="fm-field fm-multilist"><p><label id="lb-agentlist" for="it-agentlist"><?=$this->bbf('fm_agent');?></label></p>
			<div class="slt-outlist">

		<?=$form->select(array('name' => 'agentlist','label' => false,'id' => 'it-agentlist','multiple' => true,'size' => 5,'field' => false,'key' => 'fullname'),$agent_list);?>

			</div>
			<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-agentlist','it-agent'); return(false);" title="<?=$this->bbf('bt-inagent');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-inagent'),'class="bt-inlist" id="bt-inagent" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-agent','it-agentlist'); return(false);" title="<?=$this->bbf('bt-outagent');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outagent'),'class="bt-outlist" id="bt-outagent" border="0"');?></a>

			</div>
			<div class="slt-inlist">

		<?=$form->select(array('name' => 'agent[]','label' => false,'id' => 'it-agent','multiple' => true,'size' => 5,'field' => false,'key' => 'fullname'),$agent_slt);?>

			</div>
		</div>
		<div class="clearboth"></div>
<?php
		else:
			echo '<div class="txt-center">',$url->href_html($this->bbf('create_agent'),'service/ipbx/pbx_settings/agents','act=addagent'),'</div>';
		endif;

	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_agent-group'),'service/ipbx/pbx_settings/agents','act=add'),'</div>';
	endif;
?>
	</fieldset>
</div>

<div id="sb-part-application" class="b-nodisplay">

<?=$form->select(array('desc' => $this->bbf('fm_qfeatures_timeout'),'name' => 'qfeatures[timeout]','labelid' => 'qfeatures-timeout','bbf' => array('mixkey','fm_qfeatures_timeout-opt'),'key' => false,'default' => $element['qfeatures']['timeout']['default'],'value' => $info['qfeatures']['timeout']),$element['qfeatures']['timeout']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_qfeatures_data-quality'),'name' => 'qfeatures[data_quality]','labelid' => 'qfeatures-data-quality','default' => $element['qfeatures']['data_quality']['default'],'checked' => $info['qfeatures']['data_quality']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_qfeatures_hitting-callee'),'name' => 'qfeatures[hitting_callee]','labelid' => 'qfeatures-hitting-callee','default' => $element['qfeatures']['hitting_callee']['default'],'checked' => $info['qfeatures']['hitting_callee']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_qfeatures_hitting-caller'),'name' => 'qfeatures[hitting_caller]','labelid' => 'qfeatures-hitting-caller','default' => $element['qfeatures']['hitting_caller']['default'],'checked' => $info['qfeatures']['hitting_caller']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_qfeatures_retries'),'name' => 'qfeatures[retries]','labelid' => 'qfeatures-retries','default' => $element['qfeatures']['retries']['default'],'checked' => $info['qfeatures']['retries']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_qfeatures_ring'),'name' => 'qfeatures[ring]','labelid' => 'qfeatures-ring','default' => $element['qfeatures']['ring']['default'],'checked' => $info['qfeatures']['ring']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_qfeatures_transfer-user'),'name' => 'qfeatures[transfer_user]','labelid' => 'qfeatures-transfer-user','default' => $element['qfeatures']['transfer_user']['default'],'checked' => $info['qfeatures']['transfer_user']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_qfeatures_transfer-call'),'name' => 'qfeatures[transfer_call]','labelid' => 'qfeatures-transfer-call','default' => $element['qfeatures']['transfer_call']['default'],'checked' => $info['qfeatures']['transfer_call']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_qfeatures_write-caller'),'name' => 'qfeatures[write_caller]','labelid' => 'qfeatures-write-caller','default' => $element['qfeatures']['write_caller']['default'],'checked' => $info['qfeatures']['write_caller']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_qfeatures_write-calling'),'name' => 'qfeatures[write_calling]','labelid' => 'qfeatures-write-calling','default' => $element['qfeatures']['write_calling']['default'],'checked' => $info['qfeatures']['write_calling']));?>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?=$form->text(array('desc' => $this->bbf('fm_qfeatures_context'),'name' => 'qfeatures[context]','labelid' => 'qfeatures-context','size' => 15,'default' => $element['qfeatures']['context']['default'],'value' => $info['qfeatures']['context']));?>

<?=$form->text(array('desc' => $this->bbf('fm_queue_context'),'name' => 'queue[context]','labelid' => 'queue-context','size' => 15,'default' => $element['queue']['context']['default'],'value' => $info['queue']['context']));?>

<?=$form->text(array('desc' => $this->bbf('fm_queue_servicelevel'),'name' => 'queue[servicelevel]','labelid' => 'queue-servicelevel','size' => 15,'default' => $element['queue']['servicelevel']['default'],'value' => $info['queue']['servicelevel']));?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_timeout'),'name' => 'queue[timeout]','labelid' => 'queue-timeout','bbf' => array('mixkey','fm_queue_timeout-opt'),'key' => false,'default' => $element['queue']['timeout']['default'],'value' => $info['queue']['timeout']),$element['queue']['timeout']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_retry'),'name' => 'queue[retry]','labelid' => 'queue-retry','bbf' => array('mixkey','fm_queue_retry-opt'),'key' => false,'default' => $element['queue']['retry']['default'],'value' => $info['queue']['retry']),$element['queue']['retry']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_weight'),'name' => 'queue[weight]','labelid' => 'queue-weight','key' => false,'default' => $element['queue']['weight']['default'],'value' => $info['queue']['weight']),$element['queue']['weight']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_wrapuptime'),'name' => 'queue[wrapuptime]','labelid' => 'queue-wrapuptime','default' => $element['queue']['wrapuptime']['default'],'value' => $info['queue']['wrapuptime'],'bbf' => array('mixvalue','fm_queue_wrapuptime-opt')),$element['queue']['wrapuptime']['value']);?>

<?=$form->text(array('desc' => $this->bbf('fm_queue_maxlen'),'name' => 'queue[maxlen]','labelid' => 'queue-maxlen','size' => 15,'default' => $element['queue']['maxlen']['default'],'value' => $info['queue']['maxlen']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_queue_monitor-join'),'name' => 'queue[monitor-join]','labelid' => 'queue-monitor-join','default' => $element['queue']['monitor-join']['default'],'checked' => $info['queue']['monitor-join']));?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_monitor-format'),'name' => 'queue[monitor-format]','labelid' => 'queue-monitor-format','empty' => true,'key' => false,'default' => $element['queue']['monitor-format']['default'],'value' => $info['queue']['monitor-format']),$element['queue']['monitor-format']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_joinempty'),'name' => 'queue[joinempty]','labelid' => 'queue-joinempty','bbf' => 'fm_queue_joinempty-opt-','key' => false,'default' => $element['queue']['joinempty']['default'],'value' => $info['queue']['joinempty']),$element['queue']['joinempty']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_leavewhenempty'),'name' => 'queue[leavewhenempty]','labelid' => 'queue-leavewhenempty','bbf' => 'fm_queue_leavewhenempty-opt-','key' => false,'default' => $element['queue']['leavewhenempty']['default'],'value' => $info['queue']['leavewhenempty']),$element['queue']['leavewhenempty']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_queue_eventwhencalled'),'name' => 'queue[eventwhencalled]','labelid' => 'queue-eventwhencalled','default' => $element['queue']['eventwhencalled']['default'],'checked' => $info['queue']['eventwhencalled']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_queue_eventmemberstatus'),'name' => 'queue[eventmemberstatus]','labelid' => 'queue-eventmemberstatus','default' => $element['queue']['eventmemberstatus']['default'],'checked' => $info['queue']['eventmemberstatus']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_queue_reportholdtime'),'name' => 'queue[reportholdtime]','labelid' => 'queue-reportholdtime','default' => $element['queue']['reportholdtime']['default'],'checked' => $info['queue']['reportholdtime']));?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_memberdelay'),'name' => 'queue[memberdelay]','labelid' => 'queue-memberdelay','bbf' => array('mixkey','fm_queue_memberdelay-opt'),'key' => false,'default' => $element['queue']['memberdelay']['default'],'value' => $info['queue']['memberdelay']),$element['queue']['memberdelay']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_queue_timeoutrestart'),'name' => 'queue[timeoutrestart]','labelid' => 'queue-timeoutrestart','default' => $element['queue']['timeoutrestart']['default'],'checked' => $info['queue']['timeoutrestart']));?>

</div>
