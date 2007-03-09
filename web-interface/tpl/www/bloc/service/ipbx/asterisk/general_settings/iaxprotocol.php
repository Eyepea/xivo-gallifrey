<?php
	$form = &$this->get_module('form');
	$element = $this->vars('element');
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-codec');">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>

<?=$form->text(array('desc' => $this->bbf('fm_bindport'),'name' => 'bindport','id' => 'it-bindport','default' => $element['bindport']['default'],'value' => $this->varra('info','bindport')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_bindaddr'),'name' => 'bindaddr','id' => 'it-bindaddr','size' => 15,'default' => $element['bindaddr']['default'],'value' => $this->varra('info','bindaddr')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_delayreject'),'name' => 'delayreject','id' => 'it-delayreject','default' => $element['delayreject']['default'],'checked' => $this->varra('info','delayreject')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_channel-lang'),'name' => 'language','id' => 'it-language','key' => false,'default' => $element['language']['default'],'value' => $this->varra('info','language')),$element['language']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_tos'),'name' => 'tos','id' => 'it-tos','key' => false,'default' => $element['tos']['default'],'value' => $this->varra('info','tos')),$element['tos']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_qualify'),'name' => 'qualify','id' => 'it-qualify','default' => $element['qualify']['default'],'checked' => $this->varra('info','qualify')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_rtcachefriends'),'name' => 'rtcachefriends','id' => 'it-rtcachefriends','default' => $element['rtcachefriends']['default'],'checked' => $this->varra('info','rtcachefriends')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?php
	if(($jitter = $this->varra('info','jitterbuffer')) === null)
		$jitter = $element['jitterbuffer']['default'];
	else
		$jitter = xivo_bool($jitter);
?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_jitterbuffer'),'name' => 'jitterbuffer','id' => 'it-jitterbuffer','checked' => $jitter),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" onclick="xivo_eid(\'jitter\').style.display = this.checked == true ? \'block\' : \'none\';"');?>

<div id="jitter"<?=($jitter !== true ? ' class="b-nodisplay"' : '')?>>
	<?=$form->text(array('desc' => $this->bbf('fm_dropcount'),'name' => 'dropcount','id' => 'it-dropcount','default' => $element['dropcount']['default'],'value' => $this->varra('info','dropcount')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_maxexcessbuffer'),'name' => 'maxexcessbuffer','id' => 'it-maxexcessbuffer','default' => $element['maxexcessbuffer']['default'],'value' => $this->varra('info','maxexcessbuffer')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_minexcessbuffer'),'name' => 'minexcessbuffer','id' => 'it-minexcessbuffer','default' => $element['minexcessbuffer']['default'],'value' => $this->varra('info','minexcessbuffer')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_jittershrinkrate'),'name' => 'jittershrinkrate','id' => 'it-jittershrinkrate','default' => $element['jittershrinkrate']['default'],'value' => $this->varra('info','jittershrinkrate')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

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
