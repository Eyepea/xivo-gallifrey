<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');

	$element = $this->get_var('element');

	if($this->get_var('fm_save') === true):
		$dhtml = &$this->get_module('dhtml');
		$dhtml->write_js('xivo_form_success(\''.$dhtml->escape($this->bbf('fm_success-save')).'\');');
	endif;
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>

<div class="sb-smenu">
	<ul>
		<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-first');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_general');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-jitterbuffer');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_jitterbuffer');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-default');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_default');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-4" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-realtime');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_realtime');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-5" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-last',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_advanced');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-codec');">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => 1));?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_bindport'),'name' => 'bindport','labelid' => 'bindport','value' => $this->get_varra('info',array('bindport','var_val')),'default' => $element['bindport']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_bindaddr'),'name' => 'bindaddr','labelid' => 'bindaddr','size' => 15,'value' => $this->get_varra('info',array('bindaddr','var_val')),'default' => $element['bindaddr']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_iaxcompat'),'name' => 'iaxcompat','labelid' => 'iaxcompat','checked' => $this->get_varra('info',array('iaxcompat','var_val')),'default' => $element['iaxcompat']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_authdebug'),'name' => 'authdebug','labelid' => 'authdebug','checked' => $this->get_varra('info',array('authdebug','var_val')),'default' => $element['authdebug']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_delayreject'),'name' => 'delayreject','labelid' => 'delayreject','checked' => $this->get_varra('info',array('delayreject','var_val')),'default' => $element['delayreject']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_trunkfreq'),'name' => 'trunkfreq','labelid' => 'trunkfreq','key' => false,'bbf' => array('paramkey','fm_trunkfreq-opt'),'value' => $this->get_varra('info',array('trunkfreq','var_val')),'default' => $element['trunkfreq']['default']),$element['trunkfreq']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trunktimestamps'),'name' => 'trunktimestamps','labelid' => 'trunktimestamps','checked' => $this->get_varra('info',array('trunktimestamps','var_val')),'default' => $element['trunktimestamps']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_regcontext'),'name' => 'regcontext','labelid' => 'regcontext','size' => 15,'value' => $this->get_varra('info',array('regcontext','var_val')),'default' => $element['regcontext']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_minregexpire'),'name' => 'minregexpire','labelid' => 'minregexpire','bbf' => array('mixkey','fm_minregexpire-opt','paramarray'),'value' => $this->get_varra('info',array('minregexpire','var_val')),'default' => $element['minregexpire']['default']),$element['minregexpire']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_maxregexpire'),'name' => 'maxregexpire','labelid' => 'maxregexpire','bbf' => array('mixkey','fm_maxregexpire-opt','paramarray'),'value' => $this->get_varra('info',array('maxregexpire','var_val')),'default' => $element['maxregexpire']['default']),$element['maxregexpire']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_bandwidth'),'name' => 'bandwidth','labelid' => 'bandwidth','key' => false,'bbf' => array('concatvalue','fm_bandwidth-opt-'),'value' => $this->get_varra('info',array('bandwidth','var_val')),'default' => $element['bandwidth']['default']),$element['bandwidth']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_tos'),'name' => 'tos','labelid' => 'tos','key' => false,'value' => $this->get_varra('info',array('tos','var_val')),'default' => $element['tos']['default']),$element['tos']['value']);?>

</div>

<div id="sb-part-jitterbuffer" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_jitterbuffer'),'name' => 'jitterbuffer','labelid' => 'jitterbuffer','checked' => $this->get_varra('info',array('jitterbuffer','var_val')),'default' => $element['jitterbuffer']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_forcejitterbuffer'),'name' => 'forcejitterbuffer','labelid' => 'forcejitterbuffer','checked' => $this->get_varra('info',array('forcejitterbuffer','var_val')),'default' => $element['forcejitterbuffer']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_dropcount'),'name' => 'dropcount','labelid' => 'dropcount','key' => false,'value' => $this->get_varra('info',array('dropcount','var_val')),'default' => $element['dropcount']['default']),$element['dropcount']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_maxjitterbuffer'),'name' => 'maxjitterbuffer','labelid' => 'maxjitterbuffer','key' => false,'bbf' => array('mixkey','fm_maxjitterbuffer-opt'),'value' => $this->get_varra('info',array('maxjitterbuffer','var_val')),'default' => $element['maxjitterbuffer']['default']),$element['maxjitterbuffer']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_maxjitterinterps'),'name' => 'maxjitterinterps','labelid' => 'maxjitterinterps','bbf' => array('mixkey','fm_maxjitterinterps-opt'),'key' => false,'value' => $this->get_varra('info',array('maxjitterinterps','var_val')),'default' => $element['maxjitterinterps']['default']),$element['maxjitterinterps']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_resyncthreshold'),'name' => 'resyncthreshold','labelid' => 'resyncthreshold','key' => false,'bbf' => array('mixkey','fm_resyncthreshold-opt'),'value' => $this->get_varra('info',array('resyncthreshold','var_val')),'default' => $element['resyncthreshold']['default']),$element['resyncthreshold']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_minexcessbuffer'),'name' => 'minexcessbuffer','labelid' => 'minexcessbuffer','bbf' => array('mixkey','fm_minexcessbuffer-opt'),'key' => false,'value' => $this->get_varra('info',array('minexcessbuffer','var_val')),'default' => $element['minexcessbuffer']['default']),$element['minexcessbuffer']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_maxexcessbuffer'),'name' => 'maxexcessbuffer','labelid' => 'maxexcessbuffer','bbf' => array('mixkey','fm_maxexcessbuffer-opt'),'key' => false,'value' => $this->get_varra('info',array('maxexcessbuffer','var_val')),'default' => $element['maxexcessbuffer']['default']),$element['maxexcessbuffer']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_jittershrinkrate'),'name' => 'jittershrinkrate','labelid' => 'jittershrinkrate','bbf' => array('mixkey','fm_jittershrinkrate-opt'),'key' => false,'value' => $this->get_varra('info',array('jittershrinkrate','var_val')),'default' => $element['jittershrinkrate']['default']),$element['jittershrinkrate']['value']);?>

</div>

<div id="sb-part-default" class="b-nodisplay">

<?=$form->text(array('desc' => $this->bbf('fm_accountcode'),'name' => 'accountcode','labelid' => 'accountcode','size' => 15,'value' => $this->get_varra('info',array('accountcode','var_val')),'default' => $element['accountcode']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_amaflags'),'name' => 'amaflags','labelid' => 'amaflags','key' => false,'bbf' => array('concatvalue','fm_amaflags-opt-'),'value' => $this->get_varra('info',array('amaflags','var_val')),'default' => $element['amaflags']['default']),$element['amaflags']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_mailboxdetail'),'name' => 'mailboxdetail','labelid' => 'mailboxdetail','checked' => $this->get_varra('info',array('mailboxdetail','var_val')),'default' => $element['mailboxdetail']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_notransfer'),'name' => 'notransfer','labelid' => 'notransfer','checked' => $this->get_varra('info',array('notransfer','var_val')),'default' => $element['notransfer']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_language'),'name' => 'language','labelid' => 'language','key' => false,'value' => $this->get_varra('info',array('language','var_val')),'default' => $element['language']['default']),$element['language']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_encryption'),'name' => 'encryption','labelid' => 'encryption','key' => false,'bbf' => array('concatvalue','fm_encryption-opt-'),'value' => $this->get_varra('info',array('encryption','var_val')),'default' => $element['encryption']['default']),$element['encryption']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_maxauthreq'),'name' => 'maxauthreq','labelid' => 'maxauthreq','bbf' => array('mixkey','fm_maxauthreq-opt'),'key' => false,'value' => $this->get_varra('info',array('maxauthreq','var_val')),'default' => $element['maxauthreq']['default']),$element['maxauthreq']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_codecpriority'),'name' => 'codecpriority','labelid' => 'codecpriority','key' => false,'bbf' => array('concatvalue','fm_codecpriority-opt-'),'value' => $this->get_varra('info',array('codecpriority','var_val')),'default' => $element['codecpriority']['default']),$element['codecpriority']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_codec-disallow'),'name' => 'disallow','labelid' => 'disallow','key' => false,'bbf' => array('concatvalue','fm_codec-disallow-opt-')),$element['disallow']['value']);?>

<div id="codeclist" class="fm-field fm-multilist"><p><label id="lb-codeclist" for="it-codeclist"><?=$this->bbf('fm_codec-allow');?></label></p>
	<div class="slt-outlist">
		<?=$form->select(array('name' => 'codeclist','label' => false,'id' => 'it-codeclist','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_codec_name_type-'),$element['allow']['value']);?>
	</div>
	<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-codeclist','it-codec'); return(false);" title="<?=$this->bbf('bt_incodec');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt_incodec'),'class="bt-inlist" id="bt-incodec" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-codec','it-codeclist'); return(false);" title="<?=$this->bbf('bt_outcodec');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt_outcodec'),'class="bt-outlist" id="bt-outcodec" border="0"');?></a>
	</div>
	<div class="slt-inlist">

		<?=$form->select(array('name' => 'allow[]','label' => false,'id' => 'it-codec','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_codec_name_type-'),$this->get_varra('info',array('allow','var_val')));?>

		<div class="bt-updown">

			<a href="#" onclick="xivo_fm_order_selected('it-codec',1); return(false);" title="<?=$this->bbf('bt_upcodec');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt_upcodec'),'class="bt-uplist" id="bt-upcodec" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-codec',-1); return(false);" title="<?=$this->bbf('bt_downcodec');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt_downcodec'),'class="bt-downlist" id="bt-downcodec" border="0"');?></a>

		</div>

	</div>
</div>
<div class="clearboth"></div>

</div>

<div id="sb-part-realtime" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_rtcachefriends'),'name' => 'rtcachefriends','labelid' => 'rtcachefriends','checked' => $this->get_varra('info',array('rtcachefriends','var_val')),'default' => $element['rtcachefriends']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_rtupdate'),'name' => 'rtupdate','labelid' => 'rtupdate','checked' => $this->get_varra('info',array('rtupdate','var_val')),'default' => $element['rtupdate']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_rtignoreregexpire'),'name' => 'rtignoreregexpire','labelid' => 'rtignoreregexpire','checked' => $this->get_varra('info',array('rtignoreregexpire','var_val')),'default' => $element['rtignoreregexpire']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_rtautoclear'),'name' => 'rtautoclear','labelid' => 'rtautoclear','bbf' => array('mixkey','fm_rtautoclear-opt','paramarray'),'value' => $this->get_varra('info',array('rtautoclear','var_val')),'default' => $element['rtautoclear']['default']),$element['rtautoclear']['value']);?>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?=$form->select(array('desc' => $this->bbf('fm_pingtime'),'name' => 'pingtime','labelid' => 'pingtime','bbf' => array('mixkey','fm_pingtime-opt'),'key' => false,'value' => $this->get_varra('info',array('pingtime','var_val')),'default' => $element['pingtime']['default']),$element['pingtime']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_lagrqtime'),'name' => 'lagrqtime','labelid' => 'lagrqtime','bbf' => array('mixkey','fm_lagrqtime-opt'),'key' => false,'value' => $this->get_varra('info',array('lagrqtime','var_val')),'default' => $element['lagrqtime']['default']),$element['lagrqtime']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_nochecksums'),'name' => 'nochecksums','labelid' => 'nochecksums','checked' => $this->get_varra('info',array('nochecksums','var_val')),'default' => $element['nochecksums']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_autokill'),'name' => 'autokill','labelid' => 'autokill','key' => false,'bbf' => array('mixkey','fm_autokill-opt'),'value' => $this->get_varra('info',array('autokill','var_val')),'default' => $element['autokill']['default']),$element['autokill']['value']);?>

</div>

		<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
