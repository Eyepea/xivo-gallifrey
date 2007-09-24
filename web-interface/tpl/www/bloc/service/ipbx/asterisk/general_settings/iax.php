<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$element = $this->vars('element');

	if($this->vars('fm_save') === true):
		$dhtml = &$this->get_module('dhtml');
		$dhtml->write_js('xivo_form_success(\''.xivo_stript($this->bbf('fm_success-save')).'\');');
	endif;

	if(($ntos = xivo_uint($this->varra('info','tos'))) !== 0):
		$tos = $ntos;
	else:
		$tos = $this->varra('info','tos');
	endif;

	if(($nrtautoclear = xivo_uint($this->varra('info','rtautoclear'))) !== 0):
		$rtautoclear = $nrtautoclear;
	else:
		$rtautoclear = $this->varra('info','rtautoclear');
	endif;

	if(($nautokill = xivo_uint($this->varra('info','autokill'))) !== 0):
		$autokill = $nautokill;
	else:
		$autokill = $this->varra('info','autokill');
	endif;
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>

<div class="sb-smenu">
	<ul>
		<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-first');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-first'); return(false);"><?=$this->bbf('smenu_general');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-jitterbuffer');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-jitterbuffer'); return(false);"><?=$this->bbf('smenu_jitterbuffer');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-default');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-default'); return(false);"><?=$this->bbf('smenu_default');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-4" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-realtime');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-realtime'); return(false);"><?=$this->bbf('smenu_realtime');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-5" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-last',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-last'); return(false);"><?=$this->bbf('smenu_advanced');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-codec');">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => 1));?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_bindport'),'name' => 'bindport','labelid' => 'bindport','value' => $this->varra('info','bindport'),'default' => $element['bindport']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_bindaddr'),'name' => 'bindaddr','labelid' => 'bindaddr','size' => 15,'value' => $this->varra('info','bindaddr'),'default' => $element['bindaddr']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_iaxcompat'),'name' => 'iaxcompat','labelid' => 'iaxcompat','checked' => $this->varra('info','iaxcompat'),'default' => $element['iaxcompat']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_authdebug'),'name' => 'authdebug','labelid' => 'authdebug','checked' => $this->varra('info','authdebug'),'default' => $element['authdebug']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_delayreject'),'name' => 'delayreject','labelid' => 'delayreject','checked' => $this->varra('info','delayreject'),'default' => $element['delayreject']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_trunkfreq'),'name' => 'trunkfreq','labelid' => 'trunkfreq','key' => false,'bbf' => array('paramkey','fm_trunkfreq-opt'),'value' => $this->varra('info','trunkfreq'),'default' => $element['trunkfreq']['default']),$element['trunkfreq']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trunktimestamps'),'name' => 'trunktimestamps','labelid' => 'trunktimestamps','checked' => $this->varra('info','trunktimestamps'),'default' => $element['trunktimestamps']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_regcontext'),'name' => 'regcontext','labelid' => 'regcontext','size' => 15,'value' => $this->varra('info','regcontext'),'default' => $element['regcontext']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_minregexpire'),'name' => 'minregexpire','labelid' => 'minregexpire','bbf' => array('mixkey','fm_minregexpire-opt','paramarray'),'value' => $this->varra('info','minregexpire'),'default' => $element['minregexpire']['default']),$element['minregexpire']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_maxregexpire'),'name' => 'maxregexpire','labelid' => 'maxregexpire','bbf' => array('mixkey','fm_maxregexpire-opt','paramarray'),'value' => $this->varra('info','maxregexpire'),'default' => $element['maxregexpire']['default']),$element['maxregexpire']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_bandwidth'),'name' => 'bandwidth','labelid' => 'bandwidth','key' => false,'bbf' => array('concatvalue','fm_bandwidth-opt-'),'value' => $this->varra('info','bandwidth'),'default' => $element['bandwidth']['default']),$element['bandwidth']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_tos'),'name' => 'tos','labelid' => 'tos','key' => false,'value' => $tos,'default' => $element['tos']['default']),$element['tos']['value']);?>

</div>

<div id="sb-part-jitterbuffer" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_jitterbuffer'),'name' => 'jitterbuffer','labelid' => 'jitterbuffer','checked' => $this->varra('info','jitterbuffer'),'default' => $element['jitterbuffer']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_forcejitterbuffer'),'name' => 'forcejitterbuffer','labelid' => 'forcejitterbuffer','checked' => $this->varra('info','forcejitterbuffer'),'default' => $element['forcejitterbuffer']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_dropcount'),'name' => 'dropcount','labelid' => 'dropcount','key' => false,'value' => $this->varra('info','dropcount'),'default' => $element['dropcount']['default']),$element['dropcount']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_maxjitterbuffer'),'name' => 'maxjitterbuffer','labelid' => 'maxjitterbuffer','key' => false,'bbf' => array('mixkey','fm_maxjitterbuffer-opt'),'value' => $this->varra('info','maxjitterbuffer'),'default' => $element['maxjitterbuffer']['default']),$element['maxjitterbuffer']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_maxjitterinterps'),'name' => 'maxjitterinterps','labelid' => 'maxjitterinterps','bbf' => array('mixkey','fm_maxjitterinterps-opt'),'key' => false,'value' => $this->varra('info','maxjitterinterps'),'default' => $element['maxjitterinterps']['default']),$element['maxjitterinterps']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_resyncthreshold'),'name' => 'resyncthreshold','labelid' => 'resyncthreshold','key' => false,'bbf' => array('mixkey','fm_resyncthreshold-opt'),'value' => $this->varra('info','resyncthreshold'),'default' => $element['resyncthreshold']['default']),$element['resyncthreshold']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_minexcessbuffer'),'name' => 'minexcessbuffer','labelid' => 'minexcessbuffer','bbf' => array('mixkey','fm_minexcessbuffer-opt'),'key' => false,'value' => $this->varra('info','minexcessbuffer'),'default' => $element['minexcessbuffer']['default']),$element['minexcessbuffer']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_maxexcessbuffer'),'name' => 'maxexcessbuffer','labelid' => 'maxexcessbuffer','bbf' => array('mixkey','fm_maxexcessbuffer-opt'),'key' => false,'value' => $this->varra('info','maxexcessbuffer'),'default' => $element['maxexcessbuffer']['default']),$element['maxexcessbuffer']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_jittershrinkrate'),'name' => 'jittershrinkrate','labelid' => 'jittershrinkrate','bbf' => array('mixkey','fm_jittershrinkrate-opt'),'key' => false,'value' => $this->varra('info','jittershrinkrate'),'default' => $element['jittershrinkrate']['default']),$element['jittershrinkrate']['value']);?>

</div>

<div id="sb-part-default" class="b-nodisplay">

<?=$form->text(array('desc' => $this->bbf('fm_accountcode'),'name' => 'accountcode','labelid' => 'accountcode','size' => 15,'value' => $this->varra('info','accountcode'),'default' => $element['accountcode']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_amaflags'),'name' => 'amaflags','labelid' => 'amaflags','key' => false,'bbf' => array('concatvalue','fm_amaflags-opt-'),'value' => $this->varra('info','amaflags'),'default' => $element['amaflags']['default']),$element['amaflags']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_mailboxdetail'),'name' => 'mailboxdetail','labelid' => 'mailboxdetail','checked' => $this->varra('info','mailboxdetail'),'default' => $element['mailboxdetail']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_notransfer'),'name' => 'notransfer','labelid' => 'notransfer','checked' => $this->varra('info','notransfer'),'default' => $element['notransfer']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_language'),'name' => 'language','labelid' => 'language','key' => false,'value' => $this->varra('info','language'),'default' => $element['language']['default']),$element['language']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_encryption'),'name' => 'encryption','labelid' => 'encryption','key' => false,'bbf' => array('concatvalue','fm_encryption-opt-'),'value' => $this->varra('info','encryption'),'default' => $element['encryption']['default']),$element['encryption']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_maxauthreq'),'name' => 'maxauthreq','labelid' => 'maxauthreq','bbf' => array('mixkey','fm_maxauthreq-opt'),'key' => false,'value' => $this->varra('info','maxauthreq'),'default' => $element['maxauthreq']['default']),$element['maxauthreq']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_codecpriority'),'name' => 'codecpriority','labelid' => 'codecpriority','key' => false,'bbf' => array('concatvalue','fm_codecpriority-opt-'),'value' => $this->varra('info','codecpriority'),'default' => $element['codecpriority']['default']),$element['codecpriority']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_codec-disallow'),'name' => 'disallow','labelid' => 'disallow','key' => false,'bbf' => array('concatvalue','fm_codec-disallow-opt-')),$element['disallow']['value']);?>

<div id="codeclist" class="fm-field fm-multilist"><p><label id="lb-codeclist" for="it-codeclist"><?=$this->bbf('fm_codec-allow');?></label></p>
	<div class="slt-outlist">
		<?=$form->select(array('name' => 'codeclist','label' => false,'id' => 'it-codeclist','altkey' => 'id','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_codec_name_type-'),$element['allow']['value']);?>
	</div>
	<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-codeclist','it-codec'); return(false);" title="<?=$this->bbf('bt-incodec');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-incodec'),'class="bt-inlist" id="bt-incodec" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-codec','it-codeclist'); return(false);" title="<?=$this->bbf('bt-outcodec');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outcodec'),'class="bt-outlist" id="bt-outcodec" border="0"');?></a>
	</div>
	<div class="slt-inlist">

		<?=$form->select(array('name' => 'allow[]','label' => false,'id' => 'it-codec','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_codec_name_type-'),$this->varra('info','allow'));?>

		<div class="bt-updown">

			<a href="#" onclick="xivo_fm_order_selected('it-codec',1); return(false);" title="<?=$this->bbf('bt-upcodec');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt-upcodec'),'class="bt-uplist" id="bt-upcodec" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-codec',-1); return(false);" title="<?=$this->bbf('bt-downcodec');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt-downcodec'),'class="bt-downlist" id="bt-downcodec" border="0"');?></a>

		</div>

	</div>
</div>
<div class="clearboth"></div>

</div>

<div id="sb-part-realtime" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_rtcachefriends'),'name' => 'rtcachefriends','labelid' => 'rtcachefriends','checked' => $this->varra('info','rtcachefriends'),'default' => $element['rtcachefriends']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_rtupdate'),'name' => 'rtupdate','labelid' => 'rtupdate','checked' => $this->varra('info','rtupdate'),'default' => $element['rtupdate']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_rtignoreregexpire'),'name' => 'rtignoreregexpire','labelid' => 'rtignoreregexpire','checked' => $this->varra('info','rtignoreregexpire'),'default' => $element['rtignoreregexpire']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_rtautoclear'),'name' => 'rtautoclear','labelid' => 'rtautoclear','bbf' => array('mixkey','fm_rtautoclear-opt','paramarray'),'value' => $rtautoclear,'default' => $element['rtautoclear']['default']),$element['rtautoclear']['value']);?>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?=$form->select(array('desc' => $this->bbf('fm_pingtime'),'name' => 'pingtime','labelid' => 'pingtime','bbf' => array('mixkey','fm_pingtime-opt'),'key' => false,'value' => $this->varra('info','pingtime'),'default' => $element['pingtime']['default']),$element['pingtime']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_lagrqtime'),'name' => 'lagrqtime','labelid' => 'lagrqtime','bbf' => array('mixkey','fm_lagrqtime-opt'),'key' => false,'value' => $this->varra('info','lagrqtime'),'default' => $element['lagrqtime']['default']),$element['lagrqtime']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_nochecksums'),'name' => 'nochecksums','labelid' => 'nochecksums','checked' => $this->varra('info','nochecksums'),'default' => $element['nochecksums']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_autokill'),'name' => 'autokill','labelid' => 'autokill','key' => false,'bbf' => array('mixkey','fm_autokill-opt'),'value' => $autokill,'default' => $element['autokill']['default']),$element['autokill']['value']);?>

</div>

		<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
