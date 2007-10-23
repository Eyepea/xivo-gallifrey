<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$element = $this->vars('element');
	$info = $this->vars('info');

	$agents = $this->vars('agents');
	$agent_unslt = $this->vars('agent_unslt');
	$agent_slt = $this->vars('agent_slt');

	$deletable = $this->varra('info',array('agroup','deletable'));

	$queues = $this->vars('queues');
	$qmember_slt = $this->vars('qmember_slt');
	$qmember_unslt = $this->vars('qmember_unslt');
?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_agentgroup_name'),'name' => 'agroup[name]','labelid' => 'agroup-name','size' => 15,'default' => $element['agroup']['name']['default'],'value' => $info['agroup']['name']));?>

<?php
	if($agents !== false):
?>

<div id="agentlist" class="fm-field fm-multilist"><p><label id="lb-agentlist" for="it-agentlist"><?=$this->bbf('fm_agents');?></label></p>
	<div class="slt-outlist">
		<?=$form->select(array('name' => 'agentlist','label' => false,'id' => 'it-agentlist','browse' => 'sort','key' => 'identity','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$agent_unslt);?>
	</div>
	<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-agentlist','it-agent'); return(false);" title="<?=$this->bbf('bt-inagent');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-inagent'),'class="bt-inlist" id="bt-inagent" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-agent','it-agentlist'); return(false);" title="<?=$this->bbf('bt-outagent');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outagent'),'class="bt-outlist" id="bt-outagent" border="0"');?></a>
	</div>
	<div class="slt-inlist">

		<?=$form->select(array('name' => 'agent-select[]','label' => false,'id' => 'it-agent','browse' => 'sort','key' => 'identity','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$agent_slt);?>

		<div class="bt-updown">

			<a href="#" onclick="xivo_fm_order_selected('it-agent',1); return(false);" title="<?=$this->bbf('bt-upagent');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt-upagent'),'class="bt-uplist" id="bt-upagent" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-agent',-1); return(false);" title="<?=$this->bbf('bt-downagent');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt-downagent'),'class="bt-downlist" id="bt-downagent" border="0"');?></a>

		</div>

	</div>
</div>
<div class="clearboth"></div>
<?php
	endif;
?>
</div>

<div id="sb-part-last" class="b-nodisplay">

<?php
	if($queues !== false && ($nb = count($queues)) !== 0):
?>
	<div id="queuelist" class="fm-field fm-multilist">
		<div class="slt-outlist">

		<?=$form->select(array('name' => 'queuelist','label' => false,'id' => 'it-queuelist','multiple' => true,'size' => 5,'field' => false,'key' => false),$qmember_unslt);?>

		</div>
		<div class="inout-list">

			<a href="#" onclick="xivo_inqueue(); return(false);" title="<?=$this->bbf('bt-inqueue');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-inqueue'),'class="bt-inlist" id="bt-inqueue" border="0"');?></a><br />

			<a href="#" onclick="xivo_outqueue(); return(false);" title="<?=$this->bbf('bt-outqueue');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outqueue'),'class="bt-outlist" id="bt-outqueue" border="0"');?></a>

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
			$name = &$ref['queue']['name'];
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
				<td class="td-right txt-right"><?=$form->select(array('field' => false,'name' => 'queue['.$name.'][penalty]','id' => false,'label' => false,'default' => $element['qmember']['penalty']['default'],'value' => $penalty),$element['qmember']['penalty']['value']);?></td>
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
