<?php
	$form = &$this->get_module('form');
	$url = $this->get_module('url');

	$element = $this->vars('element');

	if((string) ($host = $this->varra('info',array('protocol','host-static'))) !== ''):
		$host_static = true;
	else:
		$host_static = false;
	endif;
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>

	<?=$this->file_include('bloc/service/ipbx/asterisk/trunk_management/iax/submenu');?>
	
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-codec');">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>
<?=$form->hidden(array('name' => 'act','value' => 'add'));?>

<div id="sb-part-general">

<?=$form->text(array('desc' => $this->bbf('fm_trunk_name'),'name' => 'trunk[name]','labelid' => 'trunk-name','size' => 15,'value' => $this->varra('info',array('trunk','name'))),'onchange="xivo_eid(\'it-trunk-username\').value = xivo_trunk == \'friend\' ? this.value : xivo_eid(\'it-trunk-username\').value;" onfocus="xivo_eid(\'it-trunk-username\').value = xivo_trunk == \'friend\' ? this.value : xivo_eid(\'it-trunk-username\').value; this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_username'),'name' => 'trunk[username]','labelid' => 'trunk-username','size' => 15,'value' => $this->varra('info',array('trunk','username'))),'onfocus="(this.readOnly != true ? this.className = \'it-mfocus\' : false);" onblur="(this.readOnly != true ? this.className = \'it-mblur\' : false);"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_secret'),'name' => 'trunk[secret]','labelid' => 'trunk-secret','size' => 15,'value' => $this->varra('info',array('trunk','secret'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_callerid'),'name' => 'trunk[callerid]','labelid' => 'trunk-callerid','size' => 15,'value' => $this->varra('info',array('trunk','callerid'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_calllimit'),'name' => 'trunk[call-limit]','labelid' => 'trunk-calllimit','size' => 10,'value' => $this->varra('info',array('trunk','call-limit'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_trunk_host'),'name' => 'trunk[host-dynamic]','labelid' => 'trunk-host-dynamic','key' => false,'value' => ($host_static === true ? 'static' : 'dynamic')),$element['trunk']['host-dynamic']['value'],'onchange="xivo_chg_attrib(\'fm_host\',\'fd-trunk-host-static\',(this.value == \'dynamic\' ? 1 : 2))" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => '&nbsp;','name' => 'trunk[host-static]','labelid' => 'trunk-host-static','size' => 15,'value' => $host),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_trunk_type'),'name' => 'trunk[type]','labelid' => 'trunk-type','key' => false,'default' => $element['trunk']['type']['default'],'value' => $this->varra('info',array('trunk','type'))),$element['trunk']['type']['value'],'onchange="xivo_chgtrunk(this);" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

<div id="sb-part-register" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_register'),'name' => 'register-active','labelid' => 'register-active','checked' => false),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" onclick="xivo_chg_attrib(\'fm_register\',\'it-register-username\',(this.checked == true ? 1 : 2))"');?>

<?=$form->text(array('desc' => $this->bbf('fm_register_username'),'name' => 'register[username]','labelid' => 'register-username','size' => 15,'value' => $this->varra('info',array('register','username'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_register_password'),'name' => 'register[password]','labelid' => 'register-password','size' => 15,'value' => $this->varra('info',array('register','password'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_register_authuser'),'name' => 'register[authuser]','labelid' => 'register-authuser','size' => 15,'value' => $this->varra('info',array('register','authuser'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_register_host'),'name' => 'register[host]','labelid' => 'register-host','size' => 15,'value' => $this->varra('info',array('register','host'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_register_port'),'name' => 'register[port]','labelid' => 'register-port','size' => 15,'value' => $this->varra('info',array('register','port'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_register_contact'),'name' => 'register[contact]','labelid' => 'register-contact','size' => 15,'value' => $this->varra('info',array('register','contact'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

<div id="sb-part-codec" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_codec-custom'),'name' => 'codec-active','labelid' => 'codec-active','checked' => false),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" onclick="xivo_chg_attrib(\'fm_codec\',\'it-trunk-disallow\',(this.checked == true ? 1 : 2))"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_trunk_codec-disallow'),'name' => 'trunk[disallow]','labelid' => 'trunk-disallow','key' => false),$element['trunk']['disallow']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<div id="codeclist" class="fm-field"><p><label id="lb-codeclist" for="it-codeclist"><?=$this->bbf('fm_trunk_codec-allow')?></label></p>
	<div>

	<?=$form->slt(array('name' => 'codeclist','label' => false,'id' => 'it-codeclist','key_val' => 'id','multiple' => true,'size' => 5,'field' => false,'key' => false),$element['trunk']['allow']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"')?>

	</div>
	<div id="inout-codec">

		<a href="#" onclick="xivo_fm_move_selected('it-codeclist','it-codec'); return(false);" title="<?=$this->bbf('bt-incodec')?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-incodec'),'id="bt-incodec" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-codec','it-codeclist'); return(false);" title="<?=$this->bbf('bt-outcodec')?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outcodec'),'id="bt-outcodec" border="0"');?></a>

	</div>
	<div id="select-codec" class="txt-left">

		<?=$form->slt(array('name' => 'trunk[allow][]','label' => false,'id' => 'it-codec','multiple' => true,'size' => 5,'field' => false,'key' => false),$this->varra('info',array('trunk','allow')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

		<div id="updown-codec" class="txt-left">

			<a href="#" onclick="xivo_fm_order_selected('it-codec',1); return(false);" title="<?=$this->bbf('bt-upcodec')?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt-upcodec'),'id="bt-upcodec" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-codec',-1); return(false);" title="<?=$this->bbf('bt-downcodec')?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt-downcodec'),'id="bt-downcodec" border="0"');?></a>

		</div>
	</div>
</div>
<div class="clearboth"></div>

</div>

<div id="sb-part-advanced" class="b-nodisplay">

<?=$form->text(array('desc' => $this->bbf('fm_trunk_context'),'name' => 'trunk[context]','labelid' => 'trunk-context','size' => 15,'value' => $this->varra('info',array('trunk','context'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_port'),'name' => 'trunk[port]','labelid' => 'trunk-port','size' => 15,'default' => $element['trunk']['port']['default'],'value' => $this->varra('info',array('trunk','port'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trunk_notransfer'),'name' => 'trunk[notransfer]','labelid' => 'trunk-notransfer','default' => $element['trunk']['notransfer']['default'],'checked' => $this->varra('info',array('trunk','notransfer'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trunk_trunk'),'name' => 'trunk[trunk]','labelid' => 'trunk-trunk','default' => $element['trunk']['trunk']['default'],'checked' => $this->varra('info',array('trunk','trunk'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trunk_qualify'),'name' => 'trunk[qualify]','labelid' => 'trunk-qualify','default' => $element['trunk']['qualify']['default'],'checked' => $this->varra('info',array('trunk','qualify'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

	<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
