<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->get_var('info');
	$element = $this->get_var('element');

	$groups = $this->get_var('groups');
	$gmember = $this->get_var('gmember');

	$queues = $this->get_var('queues');
	$qmember = $this->get_var('qmember');
?>

<fieldset id="fld-group">
	<legend><?=$this->bbf('fld-callgroup');?></legend>
<?php
	if(is_array($groups) === true && empty($groups) === false):
?>
	<div id="grouplist" class="fm-field fm-multilist">
		<div class="slt-outlist">

		<?=$form->select(array('name' => 'grouplist','label' => false,'id' => 'it-grouplist','multiple' => true,'size' => 5,'field' => false,'browse' => 'gfeatures','key' => 'name','altkey' => 'name'),$gmember['list']);?>

		</div>
		<div class="inout-list">

		<a href="#" onclick="xivo_ingroup(); return(false);" title="<?=$this->bbf('bt-ingroup');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-ingroup'),'class="bt-inlist" id="bt-ingroup" border="0"');?></a><br />

		<a href="#" onclick="xivo_outgroup(); return(false);" title="<?=$this->bbf('bt-outgroup');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outgroup'),'class="bt-outlist" id="bt-outgroup" border="0"');?></a>

		</div>
		<div class="slt-inlist">

		<?=$form->select(array('name' => 'group-select[]','label' => false,'id' => 'it-group','multiple' => true,'size' => 5,'field' => false,'browse' => 'gfeatures','key' => 'name','altkey' => 'name'),$gmember['slt']);?>

		</div>
	</div>
	<div class="clearboth"></div>

	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<tr class="sb-top">
				<th class="th-left"><?=$this->bbf('col_group-name');?></th>
				<th class="th-center"><?=$this->bbf('col_group-channel');?></th>
				<th class="th-right"><?=$this->bbf('col_group-calllimit');?></th>
			</tr>
<?php
		foreach($groups as $value):
			$name = &$value['gfeatures']['name'];

			if(xivo_issa($value['gfeatures']['id'],$gmember['info']) === true):
				$class = '';
				$value['member'] = $gmember['info'][$value['gfeatures']['id']];
				$calllimit = (int) $value['member']['call-limit'];
			else:
				$class = ' b-nodisplay';
				$value['member'] = null;
				$calllimit = '';
			endif;
?>
			<tr id="group-<?=$name?>" class="fm-field<?=$class?>">
				<td class="td-left"><?=$name?></td>
				<td><?=$form->select(array('field' => false,'name' => 'group['.$name.'][chantype]','id' => false,'label' => false,'key' => false,'default' => $element['qmember']['chantype']['default'],'value' => $value['member']['channel']),$element['qmember']['chantype']['value']);?></td>
				<td class="td-right"><?=$form->select(array('field' => false,'name' => 'group['.$name.'][call-limit]','id' => false,'label' => false,'default' => $element['qmember']['call-limit']['default'],'value' => $calllimit),$element['qmember']['call-limit']['value']);?></td>
			</tr>
<?php
		endforeach;
?>
			<tr id="no-group"<?=(empty($gmember['slt']) === false ? ' class="b-nodisplay"' : '')?>>
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
<fieldset id="fld-queue">
	<legend><?=$this->bbf('fld-queuegroup');?></legend>
<?php
	if(is_array($queues) === true && empty($queues) === false):
?>
	<div id="queuelist" class="fm-field fm-multilist">
		<div class="slt-outlist">

		<?=$form->select(array('name' => 'queuelist','label' => false,'id' => 'it-queuelist','multiple' => true,'size' => 5,'field' => false,'browse' => 'qfeatures','key' => 'name','altkey' => 'name'),$qmember['list']);?>

		</div>
		<div class="inout-list">

			<a href="#" onclick="xivo_inqueue(); return(false);" title="<?=$this->bbf('bt-inqueue');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-inqueue'),'class="bt-inlist" id="bt-inqueue" border="0"');?></a><br />

			<a href="#" onclick="xivo_outqueue(); return(false);" title="<?=$this->bbf('bt-outqueue');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outqueue'),'class="bt-outlist" id="bt-outqueue" border="0"');?></a>

		</div>
		<div class="slt-inlist">

		<?=$form->select(array('name' => 'queue-select[]','label' => false,'id' => 'it-queue','multiple' => true,'size' => 5,'field' => false,'browse' => 'qfeatures','key' => 'name','altkey' => 'name'),$qmember['slt']);?>

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
		foreach($queues as $value):
			$name = &$value['qfeatures']['name'];

			if(xivo_issa($value['qfeatures']['id'],$qmember['info']) === true):
				$class = '';
				$value['member'] = $qmember['info'][$value['qfeatures']['id']];
				$calllimit = (int) $value['member']['call-limit'];
				$penalty = (int) $value['member']['penalty'];
			else:
				$class = ' b-nodisplay';
				$value['member'] = null;
				$penalty = $calllimit = '';
			endif;
?>
			<tr id="queue-<?=$name?>" class="fm-field<?=$class?>">
				<td class="td-left"><?=$name?></td>
				<td><?=$form->select(array('field' => false,'name' => 'queue['.$name.'][chantype]','id' => false,'label' => false,'key' => false,'default' => $element['qmember']['chantype']['default'],'value' => $value['member']['channel']),$element['qmember']['chantype']['value']);?></td>
				<td><?=$form->select(array('field' => false,'name' => 'queue['.$name.'][penalty]','id' => false,'label' => false,'default' => $element['qmember']['penalty']['default'],'value' => $penalty),$element['qmember']['penalty']['value']);?></td>
				<td class="td-right"><?=$form->select(array('field' => false,'name' => 'queue['.$name.'][call-limit]','id' => false,'label' => false,'default' => $element['qmember']['call-limit']['default'],'value' => $calllimit),$element['qmember']['call-limit']['value']);?></td>
			</tr>
<?php
		endforeach;
?>
			<tr id="no-queue"<?=(empty($qmember['slt']) === false ? ' class="b-nodisplay"' : '')?>>
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
