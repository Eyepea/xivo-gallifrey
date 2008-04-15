<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$element = $this->get_var('element');
	$info = $this->get_var('info');

	$qmember_slt = $this->get_var('qmember_slt');
	$qmember_unslt = $this->get_var('qmember_unslt');
?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_agentfeatures_firstname'),'name' => 'afeatures[firstname]','labelid' => 'afeatures-firstname','value' => $info['afeatures']['firstname'],'default' => $element['afeatures']['firstname']['default'],'size' => 15));?>

<?=$form->text(array('desc' => $this->bbf('fm_agentfeatures_lastname'),'name' => 'afeatures[lastname]','labelid' => 'afeatures-lastname','value' => $info['afeatures']['lastname'],'default' => $element['afeatures']['lastname']['default'],'size' => 15));?>

<?=$form->text(array('desc' => $this->bbf('fm_agentfeatures_number'),'name' => 'afeatures[number]','labelid' => 'afeatures-number','value' => $info['afeatures']['number'],'default' => $element['afeatures']['number']['default'],'size' => 15));?>

<?=$form->text(array('desc' => $this->bbf('fm_agentfeatures_passwd'),'name' => 'afeatures[passwd]','labelid' => 'afeatures-passwd','value' => $info['afeatures']['passwd'],'default' => $element['afeatures']['passwd']['default'],'size' => 15));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_agentfeatures_silent'),'name' => 'afeatures[silent]','labelid' => 'afeatures-silent','default' => $element['afeatures']['silent']['default'],'checked' => $info['afeatures']['silent']));?>

<?=$form->select(array('desc' => $this->bbf('fm_agentfeatures_numgroup'),'name' => 'afeatures[numgroup]','labelid' => 'afeatures-numgroup','browse' => 'agroup','key' => 'name','altkey' => 'id','default' => $this->get_var('group'),'value' => $info['afeatures']['numgroup']),$this->get_var('list_grps'));?>

</div>

<div id="sb-part-queue" class="b-nodisplay">

<?php
	if(($queues = $this->get_var('queues')) !== false && ($nb = count($queues)) !== 0):
?>
	<div id="queuelist" class="fm-field fm-multilist">
		<div class="slt-outlist">

		<?=$form->select(array('name' => 'queuelist','label' => false,'id' => 'it-queuelist','multiple' => true,'size' => 5,'field' => false,'key' => false),$qmember_unslt);?>

		<div class="inout-list">

			<a href="#" onclick="xivo_inqueue(); return(false);" title="<?=$this->bbf('bt_inqueue');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt_inqueue'),'class="bt-inlist" id="bt-inqueue" border="0"');?></a><br />

			<a href="#" onclick="xivo_outqueue(); return(false);" title="<?=$this->bbf('bt_outqueue');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt_outqueue'),'class="bt-outlist" id="bt-outqueue" border="0"');?></a>

		</div>
		<div class="slt-inlist">

		<?=$form->select(array('name' => 'queue-select[]','label' => false,'id' => 'it-queue','multiple' => true,'size' => 5,'field' => false,'key' => 'queue_name','altkey' => 'queue_name'),$qmember_slt);?>

		</div>
	</div>

	<div class="clearboth sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<tr class="sb-top">
				<th class="th-left"><?=$this->bbf('col_queue-name');?></th>
				<th class="th-right"><?=$this->bbf('col_queue-penalty');?></th>
			</tr>
<?php
		for($i = 0;$i < $nb;$i++):
			$ref = &$queues[$i];
			$name = &$ref['qfeatures']['name'];
			$class = ' b-nodisplay';
			$penalty = '';

			if(isset($ref['member']) === true && $ref['member'] !== false):
				$class = '';
				$penalty = (int) $ref['member']['penalty'];
			else:
				$ref['member'] = null;
			endif;
?>
			<tr id="queue-<?=$name?>" class="fm-field<?=$class?>">
				<td class="td-left txt-center"><?=$name?></td>
				<td class="td-right txt-center"><?=$form->select(array('field' => false,'name' => 'queue['.$name.'][penalty]','id' => false,'label' => false,'default' => $element['qmember']['penalty']['default'],'value' => $penalty),$element['qmember']['penalty']['value']);?></td>
			</tr>
<?php
		endfor;
?>
			<tr id="no-queue"<?=($qmember_slt !== false ? ' class="b-nodisplay"' : '')?>>
				<td colspan="2" class="td-single"><?=$this->bbf('no_queue');?></td>
			</tr>
		</table>
	</div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_queue'),'service/ipbx/pbx_settings/queues','act=add'),'</div>';
	endif;
?>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?=$form->select(array('desc' => $this->bbf('fm_agent_ackcall'),'name' => 'agent[ackcall]','labelid' => 'agent-ackcall','key' => false,'bbf' => array('concatkey','fm_agent_ackcall-opt-'),'default' => $element['agent']['ackcall']['default'],'value' => $info['agent']['ackcall']),$element['agent']['ackcall']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_agent_autologoff'),'name' => 'agent[autologoff]','labelid' => 'agent-autologoff','key' => false,'default' => $element['agent']['autologoff']['default'],'value' => $info['agent']['autologoff'],'bbf' => array('mixkey','fm_agent_autologoff-opt')),$element['agent']['autologoff']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_agent_wrapuptime'),'name' => 'agent[wrapuptime]','labelid' => 'agent-wrapuptime','default' => $element['agent']['wrapuptime']['default'],'value' => $info['agent']['wrapuptime'],'bbf' => array('mixvalue','fm_agent_wrapuptime-opt')),$element['agent']['wrapuptime']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_agent_maxlogintries'),'name' => 'agent[maxlogintries]','labelid' => 'agent-maxlogintries','key' => false,'default' => $element['agent']['maxlogintries']['default'],'value' => $info['agent']['maxlogintries'],'bbf' => array('mixkey','fm_agent_maxlogintries-opt')),$element['agent']['maxlogintries']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_agent_goodbye'),'name' => 'agent[goodbye]','labelid' => 'agent-goodbye','empty' => $this->bbf('fm_agent_goodbye-opt-default'),'default' => $element['agent']['goodbye']['default'],'value' => $info['agent']['goodbye']),$this->get_var('goodbye_list'));?>

<?php
	if(($moh_list = $this->get_var('moh_list')) !== false):
		echo $form->select(array('desc' => $this->bbf('fm_agent_musiconhold'),'name' => 'agent[musiconhold]','labelid' => 'agent-musiconhold','key' => 'category','empty' => true,'default' => $element['agent']['musiconhold']['default'],'value' => $info['agent']['musiconhold']),$moh_list);
	endif;
?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_agent_updatecdr'),'name' => 'agent[updatecdr]','labelid' => 'agent-updatecdr','default' => $element['agent']['updatecdr']['default'],'checked' => $info['agent']['updatecdr']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_agent_recordagentcalls'),'name' => 'agent[recordagentcalls]','labelid' => 'agent-recordagentcalls','default' => $element['agent']['recordagentcalls']['default'],'checked' => $info['agent']['recordagentcalls']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_agent_createlink'),'name' => 'agent[createlink]','labelid' => 'agent-createlink','default' => $element['agent']['createlink']['default'],'checked' => $info['agent']['createlink']));?>

<?=$form->select(array('desc' => $this->bbf('fm_agent_recordformat'),'name' => 'agent[recordformat]','labelid' => 'agent-recordformat','key' => false,'default' => $element['agent']['recordformat']['default'],'value' => $info['agent']['recordformat']),$element['agent']['recordformat']['value']);?>

<?=$form->text(array('desc' => $this->bbf('fm_agent_urlprefix'),'name' => 'agent[urlprefix]','labelid' => 'agent-urlprefix','value' => $info['agent']['urlprefix'],'default' => $element['agent']['urlprefix']['default'],'size' => 15));?>

<?=$form->select(array('desc' => $this->bbf('fm_agent_custom_beep'),'name' => 'agent[custom_beep]','labelid' => 'agent-custom-beep','empty' => $this->bbf('fm_agent_custom-beep-opt-default'),'default' => $element['agent']['custom_beep']['default'],'value' => $info['agent']['custom_beep']),$this->get_var('beep_list'));?>

</div>
