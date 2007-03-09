<?php
	$form = &$this->get_module('form');
	$element = $this->vars('element');

	if(($host = $this->varra('info',array('trunk','host'))) === 'dynamic' || (string) $host === ''):
		$host_static = false;
	else:
		$host_static = true;
	endif;
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-codec');">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>
<?=$form->hidden(array('name' => 'act','value' => 'add'));?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_name'),'name' => 'trunk[name]','labelid' => 'trunk-name','size' => 15,'value' => $this->varra('info',array('trunk','name'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_username'),'name' => 'trunk[username]','labelid' => 'trunk-username','size' => 15,'value' => $this->varra('info',array('trunk','username'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_secret'),'name' => 'trunk[secret]','labelid' => 'trunk-secret','size' => 15,'value' => $this->varra('info',array('trunk','secret'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_callerid'),'name' => 'trunk[callerid]','labelid' => 'trunk-callerid','size' => 15,'value' => $this->varra('info',array('trunk','callerid'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_context'),'name' => 'trunk[context]','labelid' => 'trunk-context','size' => 15,'value' => $this->varra('info',array('trunk','context'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_port'),'name' => 'trunk[port]','labelid' => 'trunk-port','size' => 15,'default' => $element['trunk']['port']['default'],'value' => $this->varra('info',array('trunk','port'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_calllimit'),'name' => 'trunk[call-limit]','labelid' => 'trunk-calllimit','size' => 10,'value' => $this->varra('info',array('trunk','call-limit'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_trunk_host'),'name' => 'trunk[host-dynamic]','labelid' => 'trunk-host-dynamic','key' => false,'value' => ($host_static === true ? 'static' : 'dynamic')),$element['trunk']['host-dynamic']['value'],'onchange="xivo_chg_attrib(\'fm_trunk\',\'fd-trunk-host-static\',(this.value == \'dynamic\' ? 1 : 2))" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('name' => 'trunk[host-static]','labelid' => 'trunk-host-static','size' => 15,'value' => $this->varra('info',array('trunk','host-static'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_trunk_dtmfmode'),'name' => 'trunk[dtmfmode]','labelid' => 'trunk-dtmfmode','key' => false,'default' => $element['trunk']['dtmfmode']['default'],'value' => $this->varra('info',array('trunk','dtmfmode'))),$element['trunk']['dtmfmode']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_trunk_type'),'name' => 'trunk[type]','labelid' => 'trunk-type','key' => false,'default' => $element['trunk']['type']['default'],'value' => $this->varra('info',array('trunk','type'))),$element['trunk']['type']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trunk_nat'),'name' => 'trunk[nat]','labelid' => 'trunk-nat','default' => $element['trunk']['nat']['default'],'checked' => $this->varra('info',array('trunk','nat'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trunk_qualify'),'name' => 'trunk[qualify]','labelid' => 'trunk-qualify','default' => $element['trunk']['qualify']['default'],'checked' => $this->varra('info',array('trunk','qualify'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trunk_canreinvite'),'name' => 'trunk[canreinvite]','labelid' => 'trunk-canreinvite','default' => $element['trunk']['canreinvite']['default'],'checked' => $this->varra('info',array('trunk','canreinvite'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trunk_insecure'),'name' => 'trunk[insecure]','labelid' => 'trunk-insecure','default' => $element['trunk']['insecure']['default'],'checked' => $this->varra('info',array('trunk','insecure'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_register'),'name' => 'register-active','id' => 'it-register','checked' => false),'onclick="xivo_eid(\'register\').style.display = this.checked == true ? \'block\' : \'none\';"');?>

<div id="register" class="b-nodisplay">

	<?=$form->text(array('desc' => $this->bbf('fm_register_username'),'name' => 'register[username]','labelid' => 'register-username','size' => 15,'value' => $this->varra('info',array('register','username'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_register_password'),'name' => 'register[password]','labelid' => 'register-password','size' => 15,'value' => $this->varra('info',array('register','password'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_register_authuser'),'name' => 'register[authuser]','labelid' => 'register-authuser','size' => 15,'value' => $this->varra('info',array('register','authuser'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_register_host'),'name' => 'register[host]','labelid' => 'register-host','size' => 15,'value' => $this->varra('info',array('register','host'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_register_port'),'name' => 'register[port]','labelid' => 'register-port','size' => 15,'value' => $this->varra('info',array('register','port'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_register_contact'),'name' => 'register[contact]','labelid' => 'register-contact','size' => 15,'value' => $this->varra('info',array('register','contact'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

<?=$form->slt(array('desc' => $this->bbf('fm_trunk_codec-disallow'),'name' => 'trunk[disallow]','labelid' => 'trunk-disallow','key' => false),$element['trunk']['disallow']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

		<div id="codeclist" class="fm-field"><label id="lb-codeclist" for="it-codeclist"><?=$this->bbf('fm_trunk_codec-allow')?></label><br />
			<div>
		<?=$form->slt(array('name' => 'codeclist','label' => false,'id' => 'it-codeclist','key_val' => 'id','multiple' => true,'size' => 5,'field' => false,'key' => false),$element['trunk']['allow']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"')?>
			</div>
			<div id="inout-codec">
		<?=$form->button(array('name' => 'incodec','id' => 'it-bt-incodec','value' => $this->bbf('fm_bt-incodec')),'onclick="xivo_fm_move_selected(\'it-codeclist\',\'it-codec\');"')?>
		<?=$form->button(array('name' => 'outcodec','id' => 'it-bt-outcodec','value' => $this->bbf('fm_bt-outcodec')),'onclick="xivo_fm_move_selected(\'it-codec\',\'it-codeclist\');"');?>
			</div>
			<div id="select-codec" class="txt-left">
		<?=$form->slt(array('name' => 'trunk[allow][]','label' => false,'id' => 'it-codec','multiple' => true,'size' => 5,'field' => false,'key' => false),$this->varra('info',array('trunk','allow')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
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
