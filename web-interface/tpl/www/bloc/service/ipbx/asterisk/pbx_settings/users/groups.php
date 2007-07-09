<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->vars('info');
	$element = $this->vars('element');

	$groups = $this->vars('groups');
	$gmember_slt = $this->vars('gmember_slt');
	$gmember_unslt = $this->vars('gmember_unslt');

	$queues = $this->vars('queues');
	$qmember_slt = $this->vars('qmember_slt');
	$qmember_unslt = $this->vars('qmember_unslt');

	$ringgroup = xivo_bool($info['ufeatures']['ringgroup']);
?>

<fieldset id="fd-group">
	<legend><?=$this->bbf('fd-callgroup');?></legend>
<?php
	if($groups !== false && ($nb = count($groups)) !== 0):
?>
	<div id="grouplist" class="fm-field fm-multilist">
		<div class="slt-outlist">

		<?=$form->slt(array('name' => 'grouplist','label' => false,'id' => 'it-grouplist','multiple' => true,'size' => 5,'field' => false,'key' => false),$gmember_unslt,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

		</div>
		<div class="inout-list">

		<a href="#" onclick="xivo_ingroup(); return(false);" title="<?=$this->bbf('bt-ingroup');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-ingroup'),'class="bt-inlist" id="bt-ingroup" border="0"');?></a><br />

		<a href="#" onclick="xivo_outgroup(); return(false);" title="<?=$this->bbf('bt-outgroup');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outgroup'),'class="bt-outlist" id="bt-outgroup" border="0"');?></a>

		</div>
		<div class="slt-inlist">

		<?=$form->slt(array('name' => 'group-select[]','label' => false,'id' => 'it-group','multiple' => true,'size' => 5,'field' => false,'key' => 'queue_name','key_val' => 'queue_name'),$gmember_slt,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

		</div>
	</div>

	<div class="clearboth">

		<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_ringgroup'),'name' => 'ufeatures[ringgroup]','labelid' => 'ufeatures-ringgroup','default' => $element['ufeatures']['ringgroup']['default'],'checked' => $ringgroup),(empty($gmember_slt) === true ? 'disabled="disabled" ' : '').'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" onclick="xivo_eid(\'ringgroup\').style.display = this.checked == true ? \'block\' : \'none\';"');?>

		<div id="ringgroup"<?=($ringgroup !== true ? ' class="b-nodisplay"' : '')?>>

		<?=$form->slt(array('desc' => $this->bbf('fm_usergroup'),'name' => 'usergroup','id' => 'it-usergroup','key' => 'queue_name','value' => $info['usergroup']['groupid']),$gmember_slt,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
		</div>

	</div>

	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<tr class="sb-top">
				<th class="th-left"><?=$this->bbf('col_group-name');?></th>
				<th class="th-center"><?=$this->bbf('col_group-channel');?></th>
				<th class="th-right"><?=$this->bbf('col_group-calllimit');?></th>
			</tr>
<?php
		for($i = 0;$i < $nb;$i++):
			$ref = &$groups[$i];
			$name = &$ref['queue']['name'];
			$class = ' b-nodisplay';
			$penalty = $calllimit = '';

			if(isset($ref['member']) === true && $ref['member'] !== false):
				$class = '';
				$calllimit = (int) $ref['member']['call-limit'];
			else:
				$ref['member'] = null;
			endif;
?>
			<tr id="group-<?=$name?>" class="fm-field<?=$class?>">
				<td class="td-left txt-left"><?=$name?></td>
				<td><?=$form->slt(array('field' => false,'name' => 'group['.$name.'][chantype]','id' => false,'label' => false,'key' => false,'default' => $element['qmember']['chantype']['default'],'value' => $ref['member']['channel']),$element['qmember']['chantype']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?></td>
				<td class="td-right txt-right"><?=$form->slt(array('field' => false,'name' => 'group['.$name.'][call-limit]','id' => false,'label' => false,'default' => $element['qmember']['call-limit']['default'],'value' => $calllimit),$element['qmember']['call-limit']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?></td>
			</tr>
<?php
		endfor;
?>
			<tr id="no-group"<?=($gmember_slt !== false ? ' class="b-nodisplay"' : '')?>>
				<td colspan="3" class="td-single"><?=$this->bbf('no_group');?></td>
			</tr>
		</table>
	</div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add'),'</div>';
	endif;
?>
</fieldset>
<fieldset id="fd-queue">
	<legend><?=$this->bbf('fd-queuegroup');?></legend>
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

		<?=$form->slt(array('name' => 'queue-select[]','label' => false,'id' => 'it-queue','multiple' => true,'size' => 5,'field' => false,'key' => 'queue_name','key_val' => 'queue_name'),$qmember_slt,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

		</div>
	</div>

	<div class="clearboth sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<tr class="sb-top">
				<th class="th-left"><?=$this->bbf('col_queue-name');?></th>
				<th class="th-center"><?=$this->bbf('col_queue-channel');?></th>
				<th class="th-center"><?=$this->bbf('col_queue-penalty');?></th>
				<th class="th-right"><?=$this->bbf('col_queue-calllimit');?></th>
			</tr>
<?php
		for($i = 0;$i < $nb;$i++):
			$ref = &$queues[$i];
			$name = &$ref['queue']['name'];
			$class = ' b-nodisplay';
			$penalty = $calllimit = '';

			if(isset($ref['member']) === true && $ref['member'] !== false):
				$class = '';
				$calllimit = (int) $ref['member']['call-limit'];
				$penalty = (int) $ref['member']['penalty'];
			else:
				$ref['member'] = null;
			endif;
?>
			<tr id="queue-<?=$name?>" class="fm-field<?=$class?>">
				<td class="td-left txt-left"><?=$name?></td>
				<td><?=$form->slt(array('field' => false,'name' => 'queue['.$name.'][chantype]','id' => false,'label' => false,'key' => false,'default' => $element['qmember']['chantype']['default'],'value' => $ref['member']['channel']),$element['qmember']['chantype']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?></td>
				<td><?=$form->slt(array('field' => false,'name' => 'queue['.$name.'][penalty]','id' => false,'label' => false,'default' => $element['qmember']['penalty']['default'],'value' => $penalty),$element['qmember']['penalty']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?></td>
				<td class="td-right txt-right"><?=$form->slt(array('field' => false,'name' => 'queue['.$name.'][call-limit]','id' => false,'label' => false,'default' => $element['qmember']['call-limit']['default'],'value' => $calllimit),$element['qmember']['call-limit']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?></td>
			</tr>
<?php
		endfor;
?>
			<tr id="no-queue"<?=($qmember_slt !== false ? ' class="b-nodisplay"' : '')?>>
				<td colspan="4" class="td-single"><?=$this->bbf('no_queue');?></td>
			</tr>
		</table>
	</div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_queue'),'service/ipbx/pbx_settings/queues','act=add'),'</div>';
	endif;
?>
</fieldset>
