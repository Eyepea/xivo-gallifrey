<?php
	$form = &$this->get_module('form');
	$dhtml = &$this->get_module('dhtml');

	$element = $this->vars('element');
	$error = $this->vars('error');
	$sound_list = $this->vars('sound_list');

	if($this->vars('fm_save') === true):
		$dhtml->write_js('xivo_form_success(\''.xivo_stript($this->bbf('fm_success-save')).'\');');
	endif;

	$error_js = array();
	$error_nb = count($error['extenfeatures']);

	for($i = 0;$i < $error_nb;$i++):
		$error_js[] = 'xivo_fm_error[\'it-extenfeatures-'.$error['extenfeatures'][$i].'\'] = true;';
		$element['extenfeatures'][$error['extenfeatures'][$i]]['default'] = '';
	endfor;

	$error_nb = count($error['generalfeatures']);

	for($i = 0;$i < $error_nb;$i++):
		$error_js[] = 'xivo_fm_error[\'it-generalfeatures-'.$error['generalfeatures'][$i].'\'] = true;';
		$element['generalfeatures'][$error['generalfeatures'][$i]]['default'] = '';
	endfor;

	$error_nb = count($error['featuremap']);

	for($i = 0;$i < $error_nb;$i++):
		$error_js[] = 'xivo_fm_error[\'it-featuremap-'.$error['featuremap'][$i].'\'] = true;';
		$element['featuremap'][$error['featuremap'][$i]]['default'] = '';
	endfor;

	if($error_nb > 0)
		$dhtml->write_js($error_js);
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>


<div class="sb-smenu">
	<ul>
		<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-first');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_general');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-forwards');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_forwards');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-last',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_parking');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => 1));?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_featuremap_blindxfer'),'name' => 'featuremap[blindxfer]','labelid' => 'featuremap-blindxfer','size' => 15,'value' => $this->varra('featuremap',array('blindxfer','var_val')),'default' => $element['featuremap']['blindxfer']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_featuremap_atxfer'),'name' => 'featuremap[atxfer]','labelid' => 'featuremap-atxfer','size' => 15,'value' => $this->varra('featuremap',array('atxfer','var_val')),'default' => $element['featuremap']['atxfer']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_featuremap_automon'),'name' => 'featuremap[automon]','labelid' => 'featuremap-automon','size' => 15,'value' => $this->varra('featuremap',array('automon','var_val')),'default' => $element['featuremap']['automon']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_featuremap_disconnect'),'name' => 'featuremap[disconnect]','labelid' => 'featuremap-disconnect','size' => 15,'value' => $this->varra('featuremap',array('disconnect','var_val')),'default' => $element['featuremap']['disconnect']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-enablevm'),'name' => 'extenfeatures[enablevm][enable]','labelid' => 'extenfeatures-enable-enablevm','checked' => ((bool) $this->varra('extenfeatures',array('enablevm','commented')) === false ? true : false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_enablevm'),'name' => 'extenfeatures[enablevm][exten]','labelid' => 'extenfeatures-enablevm','size' => 15,'value' => $this->varra('extenfeatures',array('enablevm','exten')),'default' => $element['extenfeatures']['enablevm']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-voicemsg'),'name' => 'extenfeatures[voicemsg][enable]','labelid' => 'extenfeatures-enable-voicemsg','checked' => ((bool) $this->varra('extenfeatures',array('voicemsg','commented')) === false ? true : false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_voicemsg'),'name' => 'extenfeatures[voicemsg][exten]','labelid' => 'extenfeatures-voicemsg','size' => 15,'value' => $this->varra('extenfeatures',array('voicemsg','exten')),'default' => $element['extenfeatures']['voicemsg']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-recsnd'),'name' => 'extenfeatures[recsnd][enable]','labelid' => 'extenfeatures-enable-recsnd','checked' => ((bool) $this->varra('extenfeatures',array('recsnd','commented')) === false ? true : false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_recsnd'),'name' => 'extenfeatures[recsnd][exten]','labelid' => 'extenfeatures-recsnd','size' => 15,'value' => $this->varra('extenfeatures',array('recsnd','exten')),'default' => $element['extenfeatures']['recsnd']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-phonestatus'),'name' => 'extenfeatures[phonestatus][enable]','labelid' => 'extenfeatures-enable-phonestatus','checked' => ((bool) $this->varra('extenfeatures',array('phonestatus','commented')) === false ? true : false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_phonestatus'),'name' => 'extenfeatures[phonestatus][exten]','labelid' => 'extenfeatures-phonestatus','size' => 15,'value' => $this->varra('extenfeatures',array('phonestatus','exten')),'default' => $element['extenfeatures']['phonestatus']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-enablednd'),'name' => 'extenfeatures[enablednd][enable]','labelid' => 'extenfeatures-enable-enablednd','checked' => ((bool) $this->varra('extenfeatures',array('enablednd','commented')) === false ? true : false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_enablednd'),'name' => 'extenfeatures[enablednd][exten]','labelid' => 'extenfeatures-enablednd','size' => 15,'value' => $this->varra('extenfeatures',array('enablednd','exten')),'default' => $element['extenfeatures']['enablednd']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-incallrec'),'name' => 'extenfeatures[incallrec][enable]','labelid' => 'extenfeatures-enable-incallrec','checked' => ((bool) $this->varra('extenfeatures',array('incallrec','commented')) === false ? true : false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_incallrec'),'name' => 'extenfeatures[incallrec][exten]','labelid' => 'extenfeatures-incallrec','size' => 15,'value' => $this->varra('extenfeatures',array('incallrec','exten')),'default' => $element['extenfeatures']['incallrec']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-incallfilter'),'name' => 'extenfeatures[incallfilter][enable]','labelid' => 'extenfeatures-enable-incallfilter','checked' => ((bool) $this->varra('extenfeatures',array('incallfilter','commented')) === false ? true : false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_incallfilter'),'name' => 'extenfeatures[incallfilter][exten]','labelid' => 'extenfeatures-incallfilter','size' => 15,'value' => $this->varra('extenfeatures',array('incallfilter','exten')),'default' => $element['extenfeatures']['incallfilter']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_generalfeatures_pickupexten'),'name' => 'generalfeatures[pickupexten]','labelid' => 'generalfeatures-pickupexten','size' => 15,'value' => $this->varra('generalfeatures',array('pickupexten','var_val')),'default' => $element['generalfeatures']['pickupexten']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-pickup'),'name' => 'extenfeatures[pickup][enable]','labelid' => 'extenfeatures-enable-pickup','checked' => ((bool) $this->varra('extenfeatures',array('pickup','commented')) === false ? true : false)));?>

<div class="fm-field">
<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_pickup'),'name' => 'extenfeatures[pickup][exten]','field' => false,'labelid' => 'extenfeatures-pickup','size' => 15,'value' => $this->varra('extenfeatures',array('pickup','exten')),'default' => $element['extenfeatures']['pickup']['default']));?>
<?=$form->select(array('field' => false,'name' => 'extenfeatures[list-pickup]','labelid' => 'extenfeatures-list-pickup','key' => false,'empty' => true),array('*',range(3,11)),'onchange="xivo_exten_pattern(\'it-extenfeatures-pickup\',this.value);"');?>
</div>

</div>

<div id="sb-part-forwards" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-fwdundoall'),'name' => 'extenfeatures[fwdundoall][enable]','labelid' => 'extenfeatures-enable-fwdundoall','checked' => ((bool) $this->varra('extenfeatures',array('fwdundoall','commented')) === false ? true : false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_fwdundoall'),'name' => 'extenfeatures[fwdundoall][exten]','labelid' => 'extenfeatures-fwdundoall','size' => 15,'value' => $this->varra('extenfeatures',array('fwdundoall','exten')),'default' => $element['extenfeatures']['fwdundoall']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-fwdundorna'),'name' => 'extenfeatures[fwdundorna][enable]','labelid' => 'extenfeatures-enable-fwdundorna','checked' => ((bool) $this->varra('extenfeatures',array('fwdundorna','commented')) === false ? true : false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_fwdundorna'),'name' => 'extenfeatures[fwdundorna][exten]','labelid' => 'extenfeatures-fwdundorna','size' => 15,'value' => $this->varra('extenfeatures',array('fwdundorna','exten')),'default' => $element['extenfeatures']['fwdundorna']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-fwdundobusy'),'name' => 'extenfeatures[fwdundobusy][enable]','labelid' => 'extenfeatures-enable-fwdundobusy','checked' => ((bool) $this->varra('extenfeatures',array('fwdundobusy','commented')) === false ? true : false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_fwdundobusy'),'name' => 'extenfeatures[fwdundobusy][exten]','labelid' => 'extenfeatures-fwdundobusy','size' => 15,'value' => $this->varra('extenfeatures',array('fwdundobusy','exten')),'default' => $element['extenfeatures']['fwdundobusy']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-fwdundounc'),'name' => 'extenfeatures[fwdundounc][enable]','labelid' => 'extenfeatures-enable-fwdundounc','checked' => ((bool) $this->varra('extenfeatures',array('fwdundounc','commented')) === false ? true : false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_fwdundounc'),'name' => 'extenfeatures[fwdundounc][exten]','labelid' => 'extenfeatures-fwdundounc','size' => 15,'value' => $this->varra('extenfeatures',array('fwdundounc','exten')),'default' => $element['extenfeatures']['fwdundounc']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-fwdrna'),'name' => 'extenfeatures[fwdrna][enable]','labelid' => 'extenfeatures-enable-fwdrna','checked' => ((bool) $this->varra('extenfeatures',array('fwdrna','commented')) === false ? true : false)));?>

<div class="fm-field">
<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_fwdrna'),'name' => 'extenfeatures[fwdrna][exten]','field' => false,'labelid' => 'extenfeatures-fwdrna','size' => 15,'value' => $this->varra('extenfeatures',array('fwdrna','exten')),'default' => $element['extenfeatures']['fwdrna']['default']));?>
<?=$form->select(array('field' => false,'name' => 'extenfeatures[list-fwdrna]','labelid' => 'extenfeatures-list-fwdrna','key' => false,'empty' => true),array('*',range(3,11)),'onchange="xivo_exten_pattern(\'it-extenfeatures-fwdrna\',this.value);"');?>
</div>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-fwdbusy'),'name' => 'extenfeatures[fwdbusy][enable]','labelid' => 'extenfeatures-enable-fwdbusy','checked' => ((bool) $this->varra('extenfeatures',array('fwdbusy','commented')) === false ? true : false)));?>

<div class="fm-field">
<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_fwdbusy'),'name' => 'extenfeatures[fwdbusy][exten]','field' => false,'labelid' => 'extenfeatures-fwdbusy','size' => 15,'value' => $this->varra('extenfeatures',array('fwdbusy','exten')),'default' => $element['extenfeatures']['fwdbusy']['default']));?>
<?=$form->select(array('field' => false,'name' => 'extenfeatures[list-fwdbusy]','labelid' => 'extenfeatures-list-fwdbusy','key' => false,'empty' => true),array('*',range(3,11)),'onchange="xivo_exten_pattern(\'it-extenfeatures-fwdbusy\',this.value);"');?>
</div>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-fwdunc'),'name' => 'extenfeatures[fwdunc][enable]','labelid' => 'extenfeatures-enable-fwdunc','checked' => ((bool) $this->varra('extenfeatures',array('fwdunc','commented')) === false ? true : false)));?>

<div class="fm-field">
<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_fwdunc'),'name' => 'extenfeatures[fwdunc][exten]','field' => false,'labelid' => 'extenfeatures-fwdunc','size' => 15,'value' => $this->varra('extenfeatures',array('fwdunc','exten')),'default' => $element['extenfeatures']['fwdunc']['default']));?>
<?=$form->select(array('field' => false,'name' => 'extenfeatures[list-fwdunc]','labelid' => 'extenfeatures-list-fwdunc','key' => false,'empty' => true),array('*',range(3,11)),'onchange="xivo_exten_pattern(\'it-extenfeatures-fwdunc\',this.value);"');?>
</div>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?=$form->text(array('desc' => $this->bbf('fm_generalfeatures_parkext'),'name' => 'generalfeatures[parkext]','labelid' => 'generalfeatures-parkext','size' => 15,'value' => $this->varra('generalfeatures',array('parkext','var_val')),'default' => $element['generalfeatures']['parkext']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_generalfeatures_context'),'name' => 'generalfeatures[context]','labelid' => 'generalfeatures-context','size' => 15,'value' => $this->varra('generalfeatures',array('context','var_val')),'default' => $element['generalfeatures']['context']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_generalfeatures_parkingtime'),'name' => 'generalfeatures[parkingtime]','labelid' => 'generalfeatures-parkingtime','bbf' => array('mixkey','fm_generalfeatures_parkingtime-opt','paramarray'),'value' => $this->varra('generalfeatures',array('parkingtime','var_val')),'default' => $element['generalfeatures']['parkingtime']['default']),$element['generalfeatures']['parkingtime']['value']);?>

<?=$form->text(array('desc' => $this->bbf('fm_generalfeatures_parkpos'),'name' => 'generalfeatures[parkpos]','labelid' => 'generalfeatures-parkpos','size' => 15,'value' => $this->varra('generalfeatures',array('parkpos','var_val')),'default' => $element['generalfeatures']['parkpos']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_generalfeatures_parkfindnext'),'name' => 'generalfeatures[parkfindnext]','labelid' => 'generalfeatures-parkfindnext','checked' => $this->varra('generalfeatures',array('parkfindnext','var_val')),'default' => $element['generalfeatures']['parkfindnext']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_generalfeatures_adsipark'),'name' => 'generalfeatures[adsipark]','labelid' => 'generalfeatures-adsipark','checked' => $this->varra('generalfeatures',array('adsipark','var_val')),'default' => $element['generalfeatures']['adsipark']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_generalfeatures_transferdigittimeout'),'name' => 'generalfeatures[transferdigittimeout]','labelid' => 'generalfeatures-transferdigittimeout','key' => false,'bbf' => array('mixkey','fm_generalfeatures_transferdigittimeout-opt'),'value' => $this->varra('generalfeatures',array('transferdigittimeout','var_val')),'default' => $element['generalfeatures']['transferdigittimeout']['default']),$element['generalfeatures']['transferdigittimeout']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_generalfeatures_featuredigittimeout'),'name' => 'generalfeatures[featuredigittimeout]','labelid' => 'generalfeatures-featuredigittimeout','key' => false,'bbf' => array('mixkey','fm_generalfeatures_featuredigittimeout-opt'),'value' => $this->varra('generalfeatures',array('featuredigittimeout','var_val')),'default' => $element['generalfeatures']['featuredigittimeout']['default']),$element['generalfeatures']['featuredigittimeout']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_generalfeatures_courtesytone'),'name' => 'generalfeatures[courtesytone]','labelid' => 'generalfeatures-courtesytone','empty' => $this->bbf('fm_generalfeatures_courtesytone-opt-default'),'default' => $element['generalfeatures']['courtesytone']['default'],'value' => $this->varra('generalfeatures',array('courtesytone','var_val'))),$sound_list);?>

<?=$form->select(array('desc' => $this->bbf('fm_generalfeatures_xfersound'),'name' => 'generalfeatures[xfersound]','labelid' => 'generalfeatures-xfersound','empty' => $this->bbf('fm_generalfeatures_xfersound-opt-default'),'default' => $element['generalfeatures']['xfersound']['default'],'value' => $this->varra('generalfeatures',array('xfersound','var_val'))),$sound_list);?>

<?=$form->select(array('desc' => $this->bbf('fm_generalfeatures_xferfailsound'),'name' => 'generalfeatures[xferfailsound]','labelid' => 'generalfeatures-xferfailsound','empty' => $this->bbf('fm_generalfeatures_xferfailsound-opt-default'),'default' => $element['generalfeatures']['xferfailsound']['default'],'value' => $this->varra('generalfeatures',array('xferfailsound','var_val'))),$sound_list);?>

</div>

	<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
