<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$element = $this->vars('element');
	$info = $this->vars('info');

	$moh_list = $this->vars('moh_list');
	$beep_list = $this->vars('beep_list');
	$list_grps = $this->vars('list_grps');

	$queues = $this->vars('queues');
	$qmember_slt = $this->vars('qmember_slt');
	$qmember_unslt = $this->vars('qmember_unslt');
?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_agentfeatures_firstname'),'name' => 'afeatures[firstname]','labelid' => 'afeatures-firstname','value' => $info['afeatures']['firstname'],'default' => $element['afeatures']['firstname']['default'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_agentfeatures_lastname'),'name' => 'afeatures[lastname]','labelid' => 'afeatures-lastname','value' => $info['afeatures']['lastname'],'default' => $element['afeatures']['lastname']['default'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_agentfeatures_number'),'name' => 'afeatures[number]','labelid' => 'afeatures-number','value' => $info['afeatures']['number'],'default' => $element['afeatures']['number']['default'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_agentfeatures_passwd'),'name' => 'afeatures[passwd]','labelid' => 'afeatures-passwd','value' => $info['afeatures']['passwd'],'default' => $element['afeatures']['passwd']['default'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_agentfeatures_silent'),'name' => 'afeatures[silent]','labelid' => 'afeatures-silent','default' => $element['afeatures']['silent']['default'],'checked' => $info['afeatures']['silent']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_agentfeatures_numgroup'),'name' => 'afeatures[numgroup]','labelid' => 'afeatures-numgroup','browse' => 'agroup','key' => 'name','overkey' => 'id','default' => $this->vars('group'),'value' => $info['afeatures']['numgroup']),$list_grps,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

<div id="sb-part-queue" class="b-nodisplay">

<?php
	if($queues !== false && ($nb = count($queues)) !== 0):
?>
	<div id="queuelist" class="fm-field fm-multilist">
		<div class="slt-outlist">

		<?=$form->slt(array('name' => 'queuelist','label' => false,'id' => 'it-queuelist','multiple' => true,'size' => 5,'field' => false,'key' => false),$qmember_unslt,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

		</div>
		<div class="inout-list">

			<a href="#" onclick="xivo_inqueue(); return(false);" title="<?=$this->bbf('bt-inqueue');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-inqueue'),'class="bt-inlist" id="bt-inqueue" border="0"');?></a><br />

			<a href="#" onclick="xivo_outqueue(); return(false);" title="<?=$this->bbf('bt-outqueue');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outqueue'),'class="bt-outlist" id="bt-outqueue" border="0"');?></a>

		</div>
		<div class="slt-inlist">

		<?=$form->slt(array('name' => 'queue-select[]','label' => false,'id' => 'it-queue','multiple' => true,'size' => 5,'field' => false,'key' => 'queue_name','overkey' => 'queue_name'),$qmember_slt,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

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
				<td class="td-left txt-left"><?=$name?></td>
				<td class="td-right txt-right"><?=$form->slt(array('field' => false,'name' => 'queue['.$name.'][penalty]','id' => false,'label' => false,'default' => $element['qmember']['penalty']['default'],'value' => $penalty),$element['qmember']['penalty']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?></td>
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

<?=$form->slt(array('desc' => $this->bbf('fm_agent_ackcall'),'name' => 'agent[ackcall]','labelid' => 'agent-ackcall','key' => false,'bbf' => array('concatkey','fm_agent_ackcall-opt-'),'default' => $element['agent']['ackcall']['default'],'value' => $info['agent']['ackcall']),$element['agent']['ackcall']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_agent_autologoff'),'name' => 'agent[autologoff]','labelid' => 'agent-autologoff','key' => false,'default' => $element['agent']['autologoff']['default'],'value' => xivo_cast_except($info['agent']['autologoff'],null,'uint'),'bbf' => array('mixkey','fm_agent_autologoff-opt')),$element['agent']['autologoff']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_agent_wrapuptime'),'name' => 'agent[wrapuptime]','labelid' => 'agent-wrapuptime','default' => $element['agent']['wrapuptime']['default'],'value' => xivo_cast_except($info['agent']['wrapuptime'],null,'uint'),'bbf' => array('mixvalue','fm_agent_wrapuptime-opt')),$element['agent']['wrapuptime']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_agent_maxlogintries'),'name' => 'agent[maxlogintries]','labelid' => 'agent-maxlogintries','key' => false,'default' => $element['agent']['maxlogintries']['default'],'value' => xivo_cast_except($info['agent']['maxlogintries'],null,'uint'),'bbf' => array('mixkey','fm_agent_maxlogintries-opt')),$element['agent']['maxlogintries']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_agent_goodbye'),'name' => 'agent[goodbye]','labelid' => 'agent-goodbye','empty' => $this->bbf('fm_agent_goodbye-opt-default'),'default' => $element['agent']['goodbye']['default'],'value' => $info['agent']['goodbye']),$beep_list,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?php
	if($moh_list !== false):
		echo $form->slt(array('desc' => $this->bbf('fm_agent_musiconhold'),'name' => 'agent[musiconhold]','labelid' => 'agent-musiconhold','key' => 'category','empty' => true,'default' => $element['agent']['musiconhold']['default'],'value' => $info['agent']['musiconhold']),$moh_list,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');
	endif;
?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_agent_updatecdr'),'name' => 'agent[updatecdr]','labelid' => 'agent-updatecdr','default' => $element['agent']['updatecdr']['default'],'checked' => $info['agent']['updatecdr']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_agent_recordagentcalls'),'name' => 'agent[recordagentcalls]','labelid' => 'agent-recordagentcalls','default' => $element['agent']['recordagentcalls']['default'],'checked' => $info['agent']['recordagentcalls']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_agent_createlink'),'name' => 'agent[createlink]','labelid' => 'agent-createlink','default' => $element['agent']['createlink']['default'],'checked' => $info['agent']['createlink']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_agent_recordformat'),'name' => 'agent[recordformat]','labelid' => 'agent-recordformat','key' => false,'default' => $element['agent']['recordformat']['default'],'value' => $info['agent']['recordformat']),$element['agent']['recordformat']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_agent_urlprefix'),'name' => 'agent[urlprefix]','labelid' => 'agent-urlprefix','value' => $info['agent']['urlprefix'],'default' => $element['agent']['urlprefix']['default'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_agent_custom_beep'),'name' => 'agent[custom_beep]','labelid' => 'agent-custom-beep','empty' => $this->bbf('fm_agent_custom-beep-opt-default'),'default' => $element['agent']['custom_beep']['default'],'value' => $info['agent']['custom_beep']),$beep_list,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>
