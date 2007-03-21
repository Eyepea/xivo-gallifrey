<?php
	$form = &$this->get_module('form');

	$element = $this->vars('element');
	$moh_list = $this->vars('moh_list');
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-codec');">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>

<?=$form->text(array('desc' => $this->bbf('fm_bindport'),'name' => 'bindport','id' => 'it-bindport','value' => $this->varra('info','bindport'),'default' => $element['bindport']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_bindaddr'),'name' => 'bindaddr','id' => 'it-bindaddr','size' => 15,'value' => $this->varra('info','bindaddr'),'default' => $element['bindaddr']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_srvlookup'),'name' => 'srvlookup','id' => 'it-srvlookup','checked' => $this->varra('info','srvlookup'),'default' => $element['srvlookup']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_channel-lang'),'name' => 'language','id' => 'it-language','key' => false,'value' => $this->varra('info','language'),'default' => $element['language']['default']),$element['language']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_realm'),'name' => 'realm','id' => 'it-realm','size' => 15,'value' => $this->varra('info','realm'),'default' => $element['realm']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_maxexpiry'),'name' => 'maxexpiry','id' => 'it-maxexpiry','value' => $this->varra('info','maxexpiry'),'default' => $element['maxexpiry']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_defaultexpiry'),'name' => 'defaultexpiry','id' => 'it-defaultexpiry','value' => $this->varra('info','defaultexpiry'),'default' => $element['defaultexpiry']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_useragent'),'name' => 'useragent','id' => 'it-useragent','size' => 15,'value' => $this->varra('info','useragent'),'default' => $element['useragent']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_nat'),'name' => 'nat','id' => 'it-nat','checked' => $this->varra('info','nat'),'default' => $element['nat']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_qualify'),'name' => 'qualify','id' => 'it-qualify','checked' => $this->varra('info','qualify'),'default' => $element['qualify']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_rtcachefriends'),'name' => 'rtcachefriends','id' => 'it-rtcachefriends','checked' => $this->varra('info','rtcachefriends'),'default' => $element['rtcachefriends']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_allowguest'),'name' => 'allowguest','id' => 'it-allowguest','checked' => $this->varra('info','allowguest'),'default' => $element['allowguest']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_tos'),'name' => 'tos','id' => 'it-tos','key' => false,'value' => $this->varra('info','tos'),'default' => $element['tos']['default']),$element['tos']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_dtmfmode'),'name' => 'dtmfmode','id' => 'it-dtmfmode','key' => false,'value' => $this->varra('info','dtmfmode'),'default' => $element['dtmfmode']['default']),$element['dtmfmode']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_relaxdtmf'),'name' => 'relaxdtmf','id' => 'it-relaxdtmf','checked' => $this->varra('info','relaxdtmf'),'default' => $element['relaxdtmf']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_externip'),'name' => 'externip','id' => 'it-externip','size' => 15,'value' => $this->varra('info','externip'),'default' => $element['externip']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_context'),'name' => 'context','id' => 'it-context','size' => 15,'value' => $this->varra('info','context'),'default' => $element['context']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_musicclass'),'name' => 'musicclass','id' => 'musicclass','key' => 'category','empty' => true,'value' => $this->varra('info','musicclass'),'default' => $element['musicclass']['default']),$moh_list,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_checkmwi'),'name' => 'checkmwi','id' => 'it-checkmwi','value' => $this->varra('info','checkmwi'),'default' => $element['checkmwi']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_vmexten'),'name' => 'vmexten','id' => 'it-vmexten','value' => $this->varra('info','vmexten')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_videosupport'),'name' => 'videosupport','id' => 'it-videosupport','checked' => $this->varra('info','videosupport'),'default' => $element['videosupport']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_codec-disallow'),'name' => 'disallow','id' => 'it-disallow','key' => false),array('all'),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" disabled="disabled"');?>

		<div id="codeclist" class="fm-field"><label id="lb-codeclist" for="it-codeclist"><?=$this->bbf('fm_codec-allow')?></label><br />
			<div>
		<?=$form->slt(array('name' => 'codeclist','label' => false,'id' => 'it-codeclist','key_val' => 'id','multiple' => true,'size' => 5,'field' => false,'key' => false),$element['allow']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"')?>
			</div>
			<div id="inout-codec">
		<?=$form->button(array('name' => 'incodec','id' => 'it-bt-incodec','value' => $this->bbf('fm_bt-incodec')),'onclick="xivo_fm_move_selected(\'it-codeclist\',\'it-codec\');"')?>
		<?=$form->button(array('name' => 'outcodec','id' => 'it-bt-outcodec','value' => $this->bbf('fm_bt-outcodec')),'onclick="xivo_fm_move_selected(\'it-codec\',\'it-codeclist\');"');?>
			</div>
			<div id="select-codec" class="txt-left">
		<?=$form->slt(array('name' => 'allow[]','label' => false,'id' => 'it-codec','multiple' => true,'size' => 5,'field' => false,'key' => false),$this->varra('info','allow'),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
				<div id="updown-codec" class="txt-left">
			<?=$form->button(array('name' => 'upcodec','id' => 'it-bt-upcodec','value' => '&uarr;','schars' => true),'onclick="xivo_fm_order_selected(\'it-codec\',1);"')?>
			<?=$form->button(array('name' => 'downcodec','id' => 'it-bt-downcodec','value' => '&darr;','schars' => true),'onclick="xivo_fm_order_selected(\'it-codec\',-1);"');?>
				</div>
			</div>
		</div>
		<div class="clearboth"></div>
		<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
