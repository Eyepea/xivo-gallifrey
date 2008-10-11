<?php
	$form = &$this->get_module('form');
	$dhtml = &$this->get_module('dhtml');

	$element = $this->get_var('element');
	$error = $this->get_var('error');
	$sound_list = $this->get_var('sound_list');

	if($this->get_var('fm_save') === true):
		$dhtml->write_js('xivo_form_success(\''.$dhtml->escape($this->bbf('fm_success-save')).'\');');
	endif;

	$invalid = array();
	$invalid['extenfeatures'] = array();
	$invalid['generalfeatures'] = array();
	$invalid['featuremap'] = array();

	$error_js = array();
	$error_nb = count($error['extenfeatures']);

	for($i = 0;$i < $error_nb;$i++):
		$error_js[] = 'xivo_fm_error[\'it-extenfeatures-'.$error['extenfeatures'][$i].'\'] = true;';
		$invalid['extenfeatures'][$error['extenfeatures'][$i]] = true;
	endfor;

	$error_nb = count($error['generalfeatures']);

	for($i = 0;$i < $error_nb;$i++):
		$error_js[] = 'xivo_fm_error[\'it-generalfeatures-'.$error['generalfeatures'][$i].'\'] = true;';
		$invalid['generalfeatures'][$error['generalfeatures'][$i]] = true;
	endfor;

	$error_nb = count($error['featuremap']);

	for($i = 0;$i < $error_nb;$i++):
		$error_js[] = 'xivo_fm_error[\'it-featuremap-'.$error['featuremap'][$i].'\'] = true;';
		$invalid['featuremap'][$error['featuremap'][$i]] = true;
	endfor;

	if(isset($error_js[0]) === true)
		$dhtml->write_js($error_js);
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>

<div class="sb-smenu">
	<ul>
		<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-first');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_general');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-voicemail');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_voicemail');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-forward');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_forwards');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-4" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-agent');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_agents');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-5" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-last',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_parking');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => 1));?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_featuremap_blindxfer'),'name' => 'featuremap[blindxfer]','labelid' => 'featuremap-blindxfer','size' => 15,'value' => $this->get_varra('featuremap',array('blindxfer','var_val')),'default' => $element['featuremap']['blindxfer']['default'],'invalid' => isset($invalid['featuremap']['blindxfer'])));?>

<?=$form->text(array('desc' => $this->bbf('fm_featuremap_atxfer'),'name' => 'featuremap[atxfer]','labelid' => 'featuremap-atxfer','size' => 15,'value' => $this->get_varra('featuremap',array('atxfer','var_val')),'default' => $element['featuremap']['atxfer']['default'],'invalid' => isset($invalid['featuremap']['atxfer'])));?>

<?=$form->text(array('desc' => $this->bbf('fm_featuremap_automon'),'name' => 'featuremap[automon]','labelid' => 'featuremap-automon','size' => 15,'value' => $this->get_varra('featuremap',array('automon','var_val')),'default' => $element['featuremap']['automon']['default'],'invalid' => isset($invalid['featuremap']['automon'])));?>

<?=$form->text(array('desc' => $this->bbf('fm_featuremap_disconnect'),'name' => 'featuremap[disconnect]','labelid' => 'featuremap-disconnect','size' => 15,'value' => $this->get_varra('featuremap',array('disconnect','var_val')),'default' => $element['featuremap']['disconnect']['default'],'invalid' => isset($invalid['featuremap']['disconnect'])));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-recsnd'),'name' => 'extenfeatures[recsnd][enable]','labelid' => 'extenfeatures-enable-recsnd','checked' => ((bool) $this->get_varra('extenfeatures',array('recsnd','commented')) === false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_recsnd'),'name' => 'extenfeatures[recsnd][exten]','labelid' => 'extenfeatures-recsnd','size' => 15,'value' => $this->get_varra('extenfeatures',array('recsnd','exten')),'default' => $element['extenfeatures']['recsnd']['default'],'invalid' => isset($invalid['extenfeatures']['recsnd'])));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-phonestatus'),'name' => 'extenfeatures[phonestatus][enable]','labelid' => 'extenfeatures-enable-phonestatus','checked' => ((bool) $this->get_varra('extenfeatures',array('phonestatus','commented')) === false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_phonestatus'),'name' => 'extenfeatures[phonestatus][exten]','labelid' => 'extenfeatures-phonestatus','size' => 15,'value' => $this->get_varra('extenfeatures',array('phonestatus','exten')),'default' => $element['extenfeatures']['phonestatus']['default'],'invalid' => isset($invalid['extenfeatures']['phonestatus'])));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-enablednd'),'name' => 'extenfeatures[enablednd][enable]','labelid' => 'extenfeatures-enable-enablednd','checked' => ((bool) $this->get_varra('extenfeatures',array('enablednd','commented')) === false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_enablednd'),'name' => 'extenfeatures[enablednd][exten]','labelid' => 'extenfeatures-enablednd','size' => 15,'value' => $this->get_varra('extenfeatures',array('enablednd','exten')),'default' => $element['extenfeatures']['enablednd']['default'],'invalid' => isset($invalid['extenfeatures']['enablednd'])));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-incallrec'),'name' => 'extenfeatures[incallrec][enable]','labelid' => 'extenfeatures-enable-incallrec','checked' => ((bool) $this->get_varra('extenfeatures',array('incallrec','commented')) === false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_incallrec'),'name' => 'extenfeatures[incallrec][exten]','labelid' => 'extenfeatures-incallrec','size' => 15,'value' => $this->get_varra('extenfeatures',array('incallrec','exten')),'default' => $element['extenfeatures']['incallrec']['default'],'invalid' => isset($invalid['extenfeatures']['incallrec'])));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-incallfilter'),'name' => 'extenfeatures[incallfilter][enable]','labelid' => 'extenfeatures-enable-incallfilter','checked' => ((bool) $this->get_varra('extenfeatures',array('incallfilter','commented')) === false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_incallfilter'),'name' => 'extenfeatures[incallfilter][exten]','labelid' => 'extenfeatures-incallfilter','size' => 15,'value' => $this->get_varra('extenfeatures',array('incallfilter','exten')),'default' => $element['extenfeatures']['incallfilter']['default'],'invalid' => isset($invalid['extenfeatures']['incallfilter'])));?>

<?=$form->text(array('desc' => $this->bbf('fm_generalfeatures_pickupexten'),'name' => 'generalfeatures[pickupexten]','labelid' => 'generalfeatures-pickupexten','size' => 15,'value' => $this->get_varra('generalfeatures',array('pickupexten','var_val')),'default' => $element['generalfeatures']['pickupexten']['default'],'invalid' => isset($invalid['generalfeatures']['pickupexten'])));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-pickup'),'name' => 'extenfeatures[pickup][enable]','labelid' => 'extenfeatures-enable-pickup','checked' => ((bool) $this->get_varra('extenfeatures',array('pickup','commented')) === false)));?>

<div class="fm-field">
<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_pickup'),'name' => 'extenfeatures[pickup][exten]','field' => false,'labelid' => 'extenfeatures-pickup','size' => 15,'value' => $this->get_varra('extenfeatures',array('pickup','exten')),'default' => $element['extenfeatures']['pickup']['default']));?>
<?=$form->select(array('field' => false,'name' => 'extenfeatures[list-pickup]','labelid' => 'extenfeatures-list-pickup','key' => false,'empty' => true),array('*',range(3,11)),'onchange="xivo_exten_pattern(\'it-extenfeatures-pickup\',this.value);"');?>
</div>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-calllistening'),'name' => 'extenfeatures[calllistening][enable]','labelid' => 'extenfeatures-enable-calllistening','checked' => ((bool) $this->get_varra('extenfeatures',array('calllistening','commented')) === false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_calllistening'),'name' => 'extenfeatures[calllistening][exten]','labelid' => 'extenfeatures-calllistening','size' => 15,'value' => $this->get_varra('extenfeatures',array('calllistening','exten')),'default' => $element['extenfeatures']['calllistening']['default'],'invalid' => isset($invalid['extenfeatures']['calllistening'])));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-directoryaccess'),'name' => 'extenfeatures[directoryaccess][enable]','labelid' => 'extenfeatures-enable-directoryaccess','checked' => ((bool) $this->get_varra('extenfeatures',array('directoryaccess','commented')) === false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_directoryaccess'),'name' => 'extenfeatures[directoryaccess][exten]','labelid' => 'extenfeatures-directoryaccess','size' => 15,'value' => $this->get_varra('extenfeatures',array('directoryaccess','exten')),'default' => $element['extenfeatures']['directoryaccess']['default'],'invalid' => isset($invalid['extenfeatures']['directoryaccess'])));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-bsfilter'),'name' => 'extenfeatures[bsfilter][enable]','labelid' => 'extenfeatures-enable-bsfilter','checked' => ((bool) $this->get_varra('extenfeatures',array('bsfilter','commented')) === false)));?>

<div class="fm-field">
<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_bsfilter'),'name' => 'extenfeatures[bsfilter][exten]','field' => false,'labelid' => 'extenfeatures-bsfilter','size' => 15,'value' => $this->get_varra('extenfeatures',array('bsfilter','exten')),'default' => $element['extenfeatures']['bsfilter']['default']));?>
<?=$form->select(array('field' => false,'name' => 'extenfeatures[list-bsfilter]','labelid' => 'extenfeatures-list-bsfilter','key' => false,'empty' => true),array('*',range(3,11)),'onchange="xivo_exten_pattern(\'it-extenfeatures-bsfilter\',this.value);"');?>
</div>

</div>

<div id="sb-part-voicemail" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-enablevm'),'name' => 'extenfeatures[enablevm][enable]','labelid' => 'extenfeatures-enable-enablevm','checked' => ((bool) $this->get_varra('extenfeatures',array('enablevm','commented')) === false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_enablevm'),'name' => 'extenfeatures[enablevm][exten]','labelid' => 'extenfeatures-enablevm','size' => 15,'value' => $this->get_varra('extenfeatures',array('enablevm','exten')),'default' => $element['extenfeatures']['enablevm']['default'],'invalid' => isset($invalid['extenfeatures']['enablevm'])));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-voicemsg'),'name' => 'extenfeatures[voicemsg][enable]','labelid' => 'extenfeatures-enable-voicemsg','checked' => ((bool) $this->get_varra('extenfeatures',array('voicemsg','commented')) === false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_voicemsg'),'name' => 'extenfeatures[voicemsg][exten]','labelid' => 'extenfeatures-voicemsg','size' => 15,'value' => $this->get_varra('extenfeatures',array('voicemsg','exten')),'default' => $element['extenfeatures']['voicemsg']['default'],'invalid' => isset($invalid['extenfeatures']['voicemsg'])));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-vmdelete'),'name' => 'extenfeatures[vmdelete][enable]','labelid' => 'extenfeatures-enable-vmdelete','checked' => ((bool) $this->get_varra('extenfeatures',array('vmdelete','commented')) === false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_vmdelete'),'name' => 'extenfeatures[vmdelete][exten]','labelid' => 'extenfeatures-vmdelete','size' => 15,'value' => $this->get_varra('extenfeatures',array('vmdelete','exten')),'default' => $element['extenfeatures']['vmdelete']['default'],'invalid' => isset($invalid['extenfeatures']['vmdelete'])));?>

</div>

<div id="sb-part-forward" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-fwdundoall'),'name' => 'extenfeatures[fwdundoall][enable]','labelid' => 'extenfeatures-enable-fwdundoall','checked' => ((bool) $this->get_varra('extenfeatures',array('fwdundoall','commented')) === false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_fwdundoall'),'name' => 'extenfeatures[fwdundoall][exten]','labelid' => 'extenfeatures-fwdundoall','size' => 15,'value' => $this->get_varra('extenfeatures',array('fwdundoall','exten')),'default' => $element['extenfeatures']['fwdundoall']['default'],'invalid' => isset($invalid['extenfeatures']['fwdundoall'])));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-fwdundorna'),'name' => 'extenfeatures[fwdundorna][enable]','labelid' => 'extenfeatures-enable-fwdundorna','checked' => ((bool) $this->get_varra('extenfeatures',array('fwdundorna','commented')) === false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_fwdundorna'),'name' => 'extenfeatures[fwdundorna][exten]','labelid' => 'extenfeatures-fwdundorna','size' => 15,'value' => $this->get_varra('extenfeatures',array('fwdundorna','exten')),'default' => $element['extenfeatures']['fwdundorna']['default'],'invalid' => isset($invalid['extenfeatures']['fwdundorna'])));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-fwdundobusy'),'name' => 'extenfeatures[fwdundobusy][enable]','labelid' => 'extenfeatures-enable-fwdundobusy','checked' => ((bool) $this->get_varra('extenfeatures',array('fwdundobusy','commented')) === false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_fwdundobusy'),'name' => 'extenfeatures[fwdundobusy][exten]','labelid' => 'extenfeatures-fwdundobusy','size' => 15,'value' => $this->get_varra('extenfeatures',array('fwdundobusy','exten')),'default' => $element['extenfeatures']['fwdundobusy']['default'],'invalid' => isset($invalid['extenfeatures']['fwdundobusy'])));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-fwdundounc'),'name' => 'extenfeatures[fwdundounc][enable]','labelid' => 'extenfeatures-enable-fwdundounc','checked' => ((bool) $this->get_varra('extenfeatures',array('fwdundounc','commented')) === false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_fwdundounc'),'name' => 'extenfeatures[fwdundounc][exten]','labelid' => 'extenfeatures-fwdundounc','size' => 15,'value' => $this->get_varra('extenfeatures',array('fwdundounc','exten')),'default' => $element['extenfeatures']['fwdundounc']['default'],'invalid' => isset($invalid['extenfeatures']['fwdundounc'])));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-fwdrna'),'name' => 'extenfeatures[fwdrna][enable]','labelid' => 'extenfeatures-enable-fwdrna','checked' => ((bool) $this->get_varra('extenfeatures',array('fwdrna','commented')) === false)));?>

<div class="fm-field">
<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_fwdrna'),'name' => 'extenfeatures[fwdrna][exten]','field' => false,'labelid' => 'extenfeatures-fwdrna','size' => 15,'value' => $this->get_varra('extenfeatures',array('fwdrna','exten')),'default' => $element['extenfeatures']['fwdrna']['default'],'invalid' => isset($invalid['extenfeatures']['fwdrna'])));?>
<?=$form->select(array('field' => false,'name' => 'extenfeatures[list-fwdrna]','labelid' => 'extenfeatures-list-fwdrna','key' => false,'empty' => true),array('*',range(3,11)),'onchange="xivo_exten_pattern(\'it-extenfeatures-fwdrna\',this.value);"');?>
</div>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-fwdbusy'),'name' => 'extenfeatures[fwdbusy][enable]','labelid' => 'extenfeatures-enable-fwdbusy','checked' => ((bool) $this->get_varra('extenfeatures',array('fwdbusy','commented')) === false)));?>

<div class="fm-field">
<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_fwdbusy'),'name' => 'extenfeatures[fwdbusy][exten]','field' => false,'labelid' => 'extenfeatures-fwdbusy','size' => 15,'value' => $this->get_varra('extenfeatures',array('fwdbusy','exten')),'default' => $element['extenfeatures']['fwdbusy']['default'],'invalid' => isset($invalid['extenfeatures']['fwdbusy'])));?>
<?=$form->select(array('field' => false,'name' => 'extenfeatures[list-fwdbusy]','labelid' => 'extenfeatures-list-fwdbusy','key' => false,'empty' => true),array('*',range(3,11)),'onchange="xivo_exten_pattern(\'it-extenfeatures-fwdbusy\',this.value);"');?>
</div>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-fwdunc'),'name' => 'extenfeatures[fwdunc][enable]','labelid' => 'extenfeatures-enable-fwdunc','checked' => ((bool) $this->get_varra('extenfeatures',array('fwdunc','commented')) === false)));?>

<div class="fm-field">
<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_fwdunc'),'name' => 'extenfeatures[fwdunc][exten]','field' => false,'labelid' => 'extenfeatures-fwdunc','size' => 15,'value' => $this->get_varra('extenfeatures',array('fwdunc','exten')),'default' => $element['extenfeatures']['fwdunc']['default'],'invalid' => isset($invalid['extenfeatures']['fwdunc'])));?>
<?=$form->select(array('field' => false,'name' => 'extenfeatures[list-fwdunc]','labelid' => 'extenfeatures-list-fwdunc','key' => false,'empty' => true),array('*',range(3,11)),'onchange="xivo_exten_pattern(\'it-extenfeatures-fwdunc\',this.value);"');?>
</div>

</div>

<div id="sb-part-agent" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-agentstaticlogin'),'name' => 'extenfeatures[agentstaticlogin][enable]','labelid' => 'extenfeatures-enable-agentstaticlogin','checked' => ((bool) $this->get_varra('extenfeatures',array('agentstaticlogin','commented')) === false)));?>

<div class="fm-field">
<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_agentstaticlogin'),'name' => 'extenfeatures[agentstaticlogin][exten]','field' => false,'labelid' => 'extenfeatures-agentstaticlogin','size' => 15,'value' => $this->get_varra('extenfeatures',array('agentstaticlogin','exten')),'default' => $element['extenfeatures']['agentstaticlogin']['default']));?>
<?=$form->select(array('field' => false,'name' => 'extenfeatures[list-agentstaticlogin]','labelid' => 'extenfeatures-list-agentstaticlogin','key' => false,'empty' => true),array('*',range(3,11)),'onchange="xivo_exten_pattern(\'it-extenfeatures-agentstaticlogin\',this.value);"');?>
</div>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-agentstaticlogoff'),'name' => 'extenfeatures[agentstaticlogoff][enable]','labelid' => 'extenfeatures-enable-agentstaticlogoff','checked' => ((bool) $this->get_varra('extenfeatures',array('agentstaticlogoff','commented')) === false)));?>

<div class="fm-field">
<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_agentstaticlogoff'),'name' => 'extenfeatures[agentstaticlogoff][exten]','field' => false,'labelid' => 'extenfeatures-agentstaticlogoff','size' => 15,'value' => $this->get_varra('extenfeatures',array('agentstaticlogoff','exten')),'default' => $element['extenfeatures']['agentstaticlogoff']['default']));?>
<?=$form->select(array('field' => false,'name' => 'extenfeatures[list-agentstaticlogoff]','labelid' => 'extenfeatures-list-agentstaticlogoff','key' => false,'empty' => true),array('*',range(3,11)),'onchange="xivo_exten_pattern(\'it-extenfeatures-agentstaticlogoff\',this.value);"');?>
</div>

<?=$form->checkbox(array('desc' => $this->bbf('fm_extenfeatures_enable-agentdynamiclogin'),'name' => 'extenfeatures[agentdynamiclogin][enable]','labelid' => 'extenfeatures-enable-agentdynamiclogin','checked' => ((bool) $this->get_varra('extenfeatures',array('agentdynamiclogin','commented')) === false)));?>

<?=$form->text(array('desc' => $this->bbf('fm_extenfeatures_agentdynamiclogin'),'name' => 'extenfeatures[agentdynamiclogin][exten]','labelid' => 'extenfeatures-agentdynamiclogin','size' => 15,'value' => $this->get_varra('extenfeatures',array('agentdynamiclogin','exten')),'default' => $element['extenfeatures']['agentdynamiclogin']['default'],'invalid' => isset($invalid['extenfeatures']['agentdynamiclogin'])));?>

</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_generalfeatures_parkext'),
				  'name'	=> 'generalfeatures[parkext]',
				  'labelid'	=> 'generalfeatures-parkext',
				  'size'	=> 15,
				  'value'	=> $this->get_varra('generalfeatures',array('parkext','var_val')),
				  'default'	=> $element['generalfeatures']['parkext']['default'],
				  'invalid'	=> isset($invalid['generalfeatures']['parkext']))),

		$form->text(array('desc'	=> $this->bbf('fm_generalfeatures_context'),
				  'name'	=> 'generalfeatures[context]',
				  'labelid'	=> 'generalfeatures-context',
				  'size'	=> 15,
				  'value'	=> $this->get_varra('generalfeatures',array('context','var_val')),
				  'default'	=> $element['generalfeatures']['context']['default'],
				  'invalid'	=> isset($invalid['generalfeatures']['context'])),
			    'class="it-readonly" readonly="readonly"'),

		$form->select(array('desc'	=> $this->bbf('fm_generalfeatures_parkingtime'),
				    'name'	=> 'generalfeatures[parkingtime]',
				    'labelid'	=> 'generalfeatures-parkingtime',
				    'key'	=> false,
				    'bbf'	=> 'fm_generalfeatures_parkingtime-opt',
				    'bbf_opt'	=> array('argmode'	=> 'paramkey',
				    			 'time'		=> array(
							 		'from'		=> 'second',
							 		'format'	=> '%M%s')),
				    'value'	=> $this->get_varra('generalfeatures',array('parkingtime','var_val')),
				    'default'	=> $element['generalfeatures']['parkingtime']['default']),
			      $element['generalfeatures']['parkingtime']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_generalfeatures_parkpos'),
				  'name'	=> 'generalfeatures[parkpos]',
				  'labelid'	=> 'generalfeatures-parkpos',
				  'size'	=> 15,
				  'value'	=> $this->get_varra('generalfeatures',array('parkpos','var_val')),
				  'default'	=> $element['generalfeatures']['parkpos']['default'],
				  'invalid'	=> isset($invalid['generalfeatures']['parkpos']))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_generalfeatures_parkfindnext'),
				      'name'	=> 'generalfeatures[parkfindnext]',
				      'labelid'	=> 'generalfeatures-parkfindnext',
				      'checked'	=> $this->get_varra('generalfeatures',array('parkfindnext','var_val')),
				      'default'	=> $element['generalfeatures']['parkfindnext']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_generalfeatures_adsipark'),
				      'name'	=> 'generalfeatures[adsipark]',
				      'labelid'	=> 'generalfeatures-adsipark',
				      'checked'	=> $this->get_varra('generalfeatures',array('adsipark','var_val')),
				      'default'	=> $element['generalfeatures']['adsipark']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_generalfeatures_transferdigittimeout'),
				    'name'	=> 'generalfeatures[transferdigittimeout]',
				    'labelid'	=> 'generalfeatures-transferdigittimeout',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_generalfeatures_transferdigittimeout-opt'),
				    'value'	=> $this->get_varra('generalfeatures',array('transferdigittimeout','var_val')),
				    'default'	=> $element['generalfeatures']['transferdigittimeout']['default']),
			      $element['generalfeatures']['transferdigittimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_generalfeatures_featuredigittimeout'),
				    'name'	=> 'generalfeatures[featuredigittimeout]',
				    'labelid'	=> 'generalfeatures-featuredigittimeout',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_generalfeatures_featuredigittimeout-opt'),
				    'value'	=> $this->get_varra('generalfeatures',array('featuredigittimeout','var_val')),
				    'default'	=> $element['generalfeatures']['featuredigittimeout']['default']),
				    $element['generalfeatures']['featuredigittimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_generalfeatures_courtesytone'),
				    'name'	=> 'generalfeatures[courtesytone]',
				    'labelid'	=> 'generalfeatures-courtesytone',
				    'empty'	=> $this->bbf('fm_generalfeatures_courtesytone-opt-default'),
				    'default'	=> $element['generalfeatures']['courtesytone']['default'],
				    'value'	=> $this->get_varra('generalfeatures',array('courtesytone','var_val'))),
			      $sound_list),

		$form->select(array('desc'	=> $this->bbf('fm_generalfeatures_xfersound'),
				    'name'	=> 'generalfeatures[xfersound]',
				    'labelid'	=> 'generalfeatures-xfersound',
				    'empty'	=> $this->bbf('fm_generalfeatures_xfersound-opt-default'),
				    'default'	=> $element['generalfeatures']['xfersound']['default'],
				    'value'	=> $this->get_varra('generalfeatures',array('xfersound','var_val'))),
			      $sound_list),

		$form->select(array('desc'	=> $this->bbf('fm_generalfeatures_xferfailsound'),
				    'name'	=> 'generalfeatures[xferfailsound]',
				    'labelid'	=> 'generalfeatures-xferfailsound',
				    'empty'	=> $this->bbf('fm_generalfeatures_xferfailsound-opt-default'),
				    'default'	=> $element['generalfeatures']['xferfailsound']['default'],
				    'value'	=> $this->get_varra('generalfeatures',array('xferfailsound','var_val'))),
			      $sound_list);
?>
</div>
	<?=$form->submit(array('name'	=> 'submit',
			       'id'	=> 'it-submit',
			       'value'	=> $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>
