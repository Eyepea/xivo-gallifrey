<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->get_var('info');
	$element = $this->get_var('element');
	$outcalltrunk = $this->get_var('outcalltrunk');
	$rightcall = $this->get_var('rightcall');
?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_outcall_name'),'name' => 'outcall[name]','labelid' => 'outcall-name','size' => 15,'default' => $element['outcall']['name']['default'],'value' => $info['outcall']['name']));?>

<?=$form->text(array('desc' => $this->bbf('fm_outcall_context'),'name' => 'outcall[context]','labelid' => 'outcall-context','size' => 15,'default' => $element['outcall']['context']['default'],'value' => $info['outcall']['context']));?>

<?=$form->text(array('desc' => $this->bbf('fm_outcall_externprefix'),'name' => 'outcall[externprefix]','labelid' => 'outcall-externprefix','size' => 15,'default' => $element['outcall']['externprefix']['default'],'value' => $info['outcall']['externprefix']));?>

<?=$form->select(array('desc' => $this->bbf('fm_outcall_stripnum'),'name' => 'outcall[stripnum]','labelid' => 'outcall-stripnum','key' => false,'default' => $element['outcall']['stripnum']['default'],'value' => $info['outcall']['stripnum']),$element['outcall']['stripnum']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_outcall_mode'),'name' => 'outcall[mode]','labelid' => 'outcall-mode','key' => false,'bbf' => array('concatkey','fm_outcall_mode-opt-'),'default' => $element['outcall']['mode']['default'],'value' => ''),$element['outcall']['mode']['value'],'onchange="xivo_chgmode(this); (this.value == \'wizard\' ? xivo_exten_wizard(\'it-outcall-prefix\',\'it-outcall-numlen\',\'it-outcall-exten\') : xivo_wizard_exten(\'it-outcall-prefix\',\'it-outcall-numlen\',\'it-outcall-exten\'));" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');?>

<?=$form->text(array('desc' => $this->bbf('fm_outcall_prefix'),'name' => 'outcall[prefix]','labelid' => 'outcall-prefix','size' => 15,'default' => $element['outcall']['prefix']['default'],'value' => ''),'onchange="xivo_wizard_exten(this.id,\'it-outcall-numlen\',\'it-outcall-exten\');" onfocus="xivo_wizard_exten(this.id,\'it-outcall-numlen\',\'it-outcall-exten\'); xivo_fm_set_onfocus(this);" onblur="xivo_wizard_exten(this.id,\'it-outcall-numlen\',\'it-outcall-exten\'); xivo_fm_set_onblur(this);"');?>

<?=$form->select(array('desc' => $this->bbf('fm_outcall_numlen'),'name' => 'outcall[numlen]','labelid' => 'outcall-numlen','key' => false,'empty' => true,'default' => $element['outcall']['numlen']['default'],'value' => ''),$element['outcall']['numlen']['value'],'onchange="xivo_wizard_exten(\'it-outcall-prefix\',this.id,\'it-outcall-exten\');" onfocus="xivo_wizard_exten(\'it-outcall-prefix\',this.id,\'it-outcall-exten\'); xivo_fm_set_onfocus(this);" onblur="xivo_wizard_exten(\'it-outcall-prefix\',this.id,\'it-outcall-exten\'); xivo_fm_set_onblur(this);"');?>

<?=$form->text(array('desc' => $this->bbf('fm_outcall_exten'),'name' => 'outcall[exten]','labelid' => 'outcall-exten','size' => 15,'default' => $element['outcall']['exten']['default'],'value' => $info['outcall']['exten']),'onchange="xivo_exten_wizard(\'it-outcall-prefix\',\'it-outcall-numlen\',this.id);" onfocus="xivo_exten_wizard(\'it-outcall-prefix\',\'it-outcall-numlen\',this.id); xivo_fm_set_onfocus(this);" onblur="xivo_exten_wizard(\'it-outcall-prefix\',\'it-outcall-numlen\',this.id); xivo_fm_set_onblur(this);"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_outcall_setcallerid'),'name' => 'outcall[setcallerid]','labelid' => 'setcallerid','checked' => $info['outcall']['setcallerid'],'default' => $element['outcall']['setcallerid']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_outcall_callerid'),'name' => 'outcall[callerid]','labelid' => 'outcall-callerid','size' => 15,'default' => $element['outcall']['callerid']['default'],'value' => $info['outcall']['callerid']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_outcall_useenum'),'name' => 'outcall[useenum]','labelid' => 'useenum','checked' => $info['outcall']['useenum'],'default' => $element['outcall']['useenum']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_outcall_internal'),'name' => 'outcall[internal]','labelid' => 'internal','checked' => $info['outcall']['internal'],'default' => $element['outcall']['internal']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_outcall_hangupringtime'),'name' => 'outcall[hangupringtime]','labelid' => 'outcall-hangupringtime','bbf' => array('mixkey','fm_outcall_hangupringtime-opt','paramarray'),'default' => $element['outcall']['hangupringtime']['default'],'value' => $info['outcall']['hangupringtime']),$element['outcall']['hangupringtime']['value']);?>

<?php

if($outcalltrunk['list'] !== false):

?>

<div id="outcalltrunklist" class="fm-field fm-multilist"><p><label id="lb-outcalltrunklist" for="it-outcalltrunklist"><?=$this->bbf('fm_outcalltrunk');?></label></p>
	<div class="slt-outlist">
		<?=$form->select(array('name' => 'outcalltrunklist','label' => false,'id' => 'it-outcalltrunklist','multiple' => true,'size' => 5,'field' => false,'browse' => 'trunk','key' => 'identity','altkey' => 'trunkfeaturesid'),$outcalltrunk['list']);?>
	</div>
	<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-outcalltrunklist','it-outcalltrunk'); return(false);" title="<?=$this->bbf('bt_inoutcalltrunk');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt_inoutcalltrunk'),'class="bt-inlist" id="bt-inoutcalltrunk" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-outcalltrunk','it-outcalltrunklist'); return(false);" title="<?=$this->bbf('bt_outoutcalltrunk');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt_outoutcalltrunk'),'class="bt-outlist" id="bt-outoutcalltrunk" border="0"');?></a>
	</div>
	<div class="slt-inlist">

		<?=$form->select(array('name' => 'outcalltrunk[]','label' => false,'id' => 'it-outcalltrunk','multiple' => true,'size' => 5,'field' => false,'browse' => 'trunk','key' => 'identity','altkey' => 'trunkfeaturesid'),$outcalltrunk['slt']);?>

		<div class="bt-updown">

			<a href="#" onclick="xivo_fm_order_selected('it-outcalltrunk',1); return(false);" title="<?=$this->bbf('bt_upoutcalltrunk');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt_upoutcalltrunk'),'class="bt-uplist" id="bt-upoutcalltrunk" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-outcalltrunk',-1); return(false);" title="<?=$this->bbf('bt_downoutcalltrunk');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt_downoutcalltrunk'),'class="bt-downlist" id="bt-downoutcalltrunk" border="0"');?></a>

		</div>

	</div>
</div>
<div class="clearboth"></div>

<?php

else:
	echo '<div class="txt-center">',$url->href_html($this->bbf('create_trunk'),'service/ipbx/trunk_management/sip','act=add'),'</div>';
endif;

?>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?php
	if($rightcall['list'] !== false):
?>
		<div id="rightcalllist" class="fm-field fm-multilist">
			<div class="slt-outlist">

		<?=$form->select(array('name' => 'rightcalllist','label' => false,'id' => 'it-rightcalllist','browse' => 'rightcall','key' => 'name','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$rightcall['list']);?>

			</div>
			<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-rightcalllist','it-rightcall'); return(false);" title="<?=$this->bbf('bt_inrightcall');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt_inrightcall'),'class="bt-inlist" id="bt-inrightcall" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-rightcall','it-rightcalllist'); return(false);" title="<?=$this->bbf('bt_outrightcall');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt_outrightcall'),'class="bt-outlist" id="bt-outrightcall" border="0"');?></a>

			</div>
			<div class="slt-inlist">

		<?=$form->select(array('name' => 'rightcall[]','label' => false,'id' => 'it-rightcall','browse' => 'rightcall','key' => 'name','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$rightcall['slt']);?>

			</div>
		</div>
		<div class="clearboth"></div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_rightcall'),'service/ipbx/call_management/rightcall','act=add'),'</div>';
	endif;
?>
</div>
