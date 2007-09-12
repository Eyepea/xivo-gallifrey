<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->vars('info');
	$element = $this->vars('element');
	$list = $this->vars('list');
	$trunks_list = $this->vars('trunks_list');
?>

<?=$form->text(array('desc' => $this->bbf('fm_outcall_name'),'name' => 'outcall[name]','labelid' => 'outcall-name','size' => 15,'default' => $element['outcall']['name']['default'],'value' => $info['outcall']['name']));?>

<?=$form->select(array('desc' => $this->bbf('fm_outcall_trunk'),'name' => 'outcall[trunkfeaturesid]','labelid' => 'outcall-trunk','browse' => 'trunk','key' => 'name','altkey' => 'trunkfeaturesid','optgroup' => array('key' => true,'bbf' => array('concat','fm_outcall-trunk-opt-')),'value' => $info['outcall']['trunkfeaturesid'],'default' => $element['outcall']['trunkfeaturesid']['default']),$trunks_list);?>

<?=$form->text(array('desc' => $this->bbf('fm_outcall_context'),'name' => 'outcall[context]','labelid' => 'outcall-context','size' => 15,'default' => $element['outcall']['context']['default'],'value' => $info['outcall']['context']));?>

<?=$form->text(array('desc' => $this->bbf('fm_outcall_externprefix'),'name' => 'outcall[externprefix]','labelid' => 'outcall-externprefix','size' => 15,'default' => $element['outcall']['externprefix']['default'],'value' => $info['outcall']['externprefix']));?>

<?=$form->select(array('desc' => $this->bbf('fm_outcall_stripnum'),'name' => 'outcall[stripnum]','labelid' => 'outcall-stripnum','key' => false,'default' => $element['outcall']['stripnum']['default'],'value' => $info['outcall']['stripnum']),$element['outcall']['stripnum']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_outcall_mode'),'name' => 'outcall[mode]','labelid' => 'outcall-mode','key' => false,'bbf' => array('concatkey','fm_outcall_mode-opt-'),'default' => $element['outcall']['mode']['default'],'value' => $info['outcall']['mode']),$element['outcall']['mode']['value'],'onchange="xivo_chgmode(this); (this.value == \'wizard\' ? xivo_wizard_exten(\'it-outcall-prefix\',\'it-outcall-numlen\',\'it-extenumbers-exten\') : false);" onfocus="this.className = xivo_fm_onfocus_class;" onblur="this.className = xivo_fm_onblur_class;"');?>

<?=$form->text(array('desc' => $this->bbf('fm_outcall_prefix'),'name' => 'outcall[prefix]','labelid' => 'outcall-prefix','size' => 15,'default' => $element['outcall']['prefix']['default'],'value' => $info['outcall']['prefix']),'onchange="xivo_wizard_exten(this.id,\'it-outcall-numlen\',\'it-extenumbers-exten\');" onfocus="xivo_wizard_exten(this.id,\'it-outcall-numlen\',\'it-extenumbers-exten\'); this.className = xivo_fm_onfocus_class;" onblur="xivo_wizard_exten(this.id,\'it-outcall-numlen\',\'it-extenumbers-exten\'); this.className = xivo_fm_onblur_class;"');?>

<?=$form->select(array('desc' => $this->bbf('fm_outcall_numlen'),'name' => 'outcall[numlen]','labelid' => 'outcall-numlen','key' => false,'empty' => true,'default' => $element['outcall']['numlen']['default'],'value' => $info['outcall']['numlen']),$element['outcall']['numlen']['value'],'onchange="xivo_wizard_exten(\'it-outcall-prefix\',this.id,\'it-extenumbers-exten\');" onfocus="xivo_wizard_exten(\'it-outcall-prefix\',this.id,\'it-extenumbers-exten\'); this.className = xivo_fm_onfocus_class;" onblur="xivo_wizard_exten(\'it-outcall-prefix\',this.id,\'it-extenumbers-exten\'); this.className = xivo_fm_onblur_class;"');?>

<?=$form->text(array('desc' => $this->bbf('fm_extenumbers_exten'),'name' => 'extenumbers[exten]','labelid' => 'extenumbers-exten','size' => 15,'default' => $element['extenumbers']['exten']['default'],'value' => $info['extenumbers']['exten']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_outcall_setcallerid'),'name' => 'outcall[setcallerid]','labelid' => 'setcallerid','checked' => $info['outcall']['setcallerid'],'default' => $element['outcall']['setcallerid']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_outcall_callerid'),'name' => 'outcall[callerid]','labelid' => 'outcall-callerid','size' => 15,'default' => $element['outcall']['callerid']['default'],'value' => $info['outcall']['callerid']));?>

<?=$form->select(array('desc' => $this->bbf('fm_outcall_weight'),'name' => 'outcall[weight]','labelid' => 'outcall-weight','key' => false,'default' => $element['outcall']['weight']['default'],'value' => $info['outcall']['weight']),$element['outcall']['weight']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_outcall_useenum'),'name' => 'outcall[useenum]','labelid' => 'useenum','checked' => $info['outcall']['useenum'],'default' => $element['outcall']['useenum']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_outcall_hangupringtime'),'name' => 'outcall[hangupringtime]','labelid' => 'outcall-hangupringtime','bbf' => array('mixkey','fm_outcall_hangupringtime-opt','paramarray'),'default' => $element['outcall']['hangupringtime']['default'],'value' => $info['outcall']['hangupringtime']),$element['outcall']['hangupringtime']['value']);?>

