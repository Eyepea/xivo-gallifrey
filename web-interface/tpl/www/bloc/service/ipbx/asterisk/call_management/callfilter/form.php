<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->get_var('info');
	$element = $this->get_var('element');
	$bosslist = $this->get_var('bosslist');
	$secretary = $this->get_var('secretary');

	if($this->get_var('act') === 'add'):
		$invalid_boss = false;
	elseif(xivo_issa('callfiltermember',$info) === false):
		$invalid_boss = true;
	elseif(xivo_issa('boss',$info['callfiltermember']) === false):
		$invalid_boss = true;
	else:
		$invalid_boss = false;
	endif;
?>

<?=$form->text(array('desc' => $this->bbf('fm_callfilter_name'),'name' => 'callfilter[name]','labelid' => 'callfilter-name','size' => 15,'default' => $element['callfilter']['name']['default'],'value' => $info['callfilter']['name']));?>

<?=$form->select(array('desc' => $this->bbf('fm_callfilter_zone'),'name' => 'callfilter[zone]','labelid' => 'callfilter-zone','bbf' => array('concatkey','fm_callfilter_zone-opt-'),'key' => false,'default' => $element['callfilter']['zone']['default'],'value' => $info['callfilter']['zone']),$element['callfilter']['zone']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_callfilter_bosssecretary'),'name' => 'callfilter[bosssecretary]','labelid' => 'callfilter-bosssecretary','bbf' => array('concatkey','fm_callfilter_bosssecretary-opt-'),'key' => false,'default' => $element['callfilter']['bosssecretary']['default'],'value' => $info['callfilter']['bosssecretary']),$element['callfilter']['bosssecretary']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_callfilter_ringseconds'),'name' => 'callfilter[ringseconds]','labelid' => 'callfilter-ringseconds','bbf' => array('mixkey','fm_callfilter_ringseconds-opt'),'key' => false,'default' => $element['callfilter']['ringseconds']['default'],'value' => $info['callfilter']['ringseconds']),$element['callfilter']['ringseconds']['value']);?>

<?=$form->text(array('desc' => $this->bbf('fm_callfilter_callerdisplay'),'name' => 'callfilter[callerdisplay]','labelid' => 'callfilter-callerdisplay','size' => 15,'default' => $element['callfilter']['callerdisplay']['default'],'value' => $info['callfilter']['callerdisplay']));?>

<fieldset id="fld-callfilter-boss">
	<legend><?=$this->bbf('fld-callfilter-boss');?></legend>
<?php
	if(empty($bosslist) === false):
		echo $form->select(array('desc' => $this->bbf('fm_callfiltermember-boss'),'name' => 'callfiltermember[boss][id]','labelid' => 'callfiltermember-boss','key' => 'identity','altkey' => 'id','invalid' => $invalid_boss,'value' => $info['callfiltermember']['boss']['typeval']),$bosslist);

		echo $form->select(array('desc' => $this->bbf('fm_callfiltermember_ringseconds-boss'),'name' => 'callfiltermember[boss][ringseconds]','labelid' => 'callfiltermember-ringseconds-boss','bbf' => array('mixkey','fm_callfiltermember_ringseconds-boss-opt'),'key' => false,'default' => $element['callfiltermember']['ringseconds']['default'],'value' => $info['callfiltermember']['boss']['ringseconds']),$element['callfiltermember']['ringseconds']['value']);
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_user-boss'),'service/ipbx/pbx_settings/users','act=add'),'</div>';
	endif;
?>
</fieldset>

<fieldset id="fld-callfilter-secretary">
	<legend><?=$this->bbf('fld-callfilter-secretary');?></legend>
<?php
	if($secretary['list'] !== false):
?>

<div id="callfiltermember-secretarylist" class="fm-field fm-multilist">
	<div class="slt-outlist">
		<?=$form->select(array('name' => 'callfiltermember-secretarylist','label' => false,'id' => 'it-callfiltermember-secretarylist','multiple' => true,'size' => 5,'field' => false,'key' => 'identity','altkey' => 'id'),$secretary['list']);?>
	</div>
	<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-callfiltermember-secretarylist','it-callfiltermember-secretary'); return(false);" title="<?=$this->bbf('bt-insecretary');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-insecretary'),'class="bt-inlist" id="bt-insecretary" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-callfiltermember-secretary','it-callfiltermember-secretarylist'); return(false);" title="<?=$this->bbf('bt-outsecretary');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outsecretary'),'class="bt-outlist" id="bt-outsecretary" border="0"');?></a>
	</div>
	<div class="slt-inlist">

		<?=$form->select(array('name' => 'callfiltermember[secretary][]','label' => false,'id' => 'it-callfiltermember-secretary','multiple' => true,'size' => 5,'field' => false,'key' => 'identity','altkey' => 'id'),$secretary['slt']);?>

		<div class="bt-updown">

			<a href="#" onclick="xivo_fm_order_selected('it-callfiltermember-secretary',1); return(false);" title="<?=$this->bbf('bt-upsecretary');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt-upsecretary'),'class="bt-uplist" id="bt-upsecretary" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-callfiltermember-secretary',-1); return(false);" title="<?=$this->bbf('bt-downsecretary');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt-downsecretary'),'class="bt-downlist" id="bt-downsecretary" border="0"');?></a>

		</div>

	</div>
</div>
<div class="clearboth"></div>

<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_user-secretary'),'service/ipbx/pbx_settings/users','act=add'),'</div>';
	endif;
?>
</fieldset>

<div class="fm-field fm-description"><p><label id="lb-callfilter-description" for="it-callfilter-description"><?=$this->bbf('fm_callfilter_description');?></label></p>
<?=$form->textarea(array('field' => false,'label' => false,'name' => 'callfilter[description]','id' => 'it-callfilter-description','cols' => 60,'rows' => 5,'default' => $element['callfilter']['description']['default']),$info['callfilter']['description']);?>
</div>

	<fieldset id="fld-dialstatus-noanswer">
		<legend><?=$this->bbf('fld-dialstatus-noanswer');?></legend>
		<?=$this->file_include('bloc/service/ipbx/asterisk/call_management/callfilter/dialstatus',array('status' => 'noanswer'));?>
	</fieldset>
