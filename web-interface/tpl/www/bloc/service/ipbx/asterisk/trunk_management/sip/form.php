<?php
	$form = &$this->get_module('form');
	$url = $this->get_module('url');

	$info = $this->vars('info');
	$element = $this->vars('element');

	$allow = $info['trunk']['allow'];

	if(empty($allow) === true):
		$codec_active = false;
	else:
		$codec_active = true;
	endif;

	$trunk_active = (bool) $this->varra('info',array('trunk','commented'));

	$reg_active = $this->varra('info',array('register','commented'));

	if($reg_active !== null):
		$reg_active = xivo_bool($reg_active) === true ? false : true;
	endif;

	if(($host = (string) $this->varra('info',array('trunk','host-static'))) !== ''):
		$host_static = true;
	elseif(($host = (string) $this->varra('info',array('trunk','host'))) === 'dynamic' || $host === ''):
		$host_static = false;
	else:
		$host_static = true;
	endif;

	if(isset($info['trunk']['call-limit']) === false || ($calllimit = xivo_uint($info['trunk']['call-limit'])) === 0):
		$calllimit = '';
	else:
		$calllimit = xivo_uint($info['trunk']['call-limit']);
	endif;
?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_trunk_name'),'name' => 'trunk[name]','labelid' => 'trunk-name','size' => 15,'value' => $info['trunk']['name']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_username'),'name' => 'trunk[username]','labelid' => 'trunk-username','size' => 15,'value' => $info['trunk']['username']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_secret'),'name' => 'trunk[secret]','labelid' => 'trunk-secret','size' => 15,'value' => $info['trunk']['secret']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_callerid'),'name' => 'trunk[callerid]','labelid' => 'trunk-callerid','size' => 15,'value' => $info['trunk']['callerid']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_calllimit'),'name' => 'trunk[call-limit]','labelid' => 'trunk-calllimit','size' => 10,'value' => $calllimit),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->select(array('desc' => $this->bbf('fm_trunk_host'),'name' => 'trunk[host-dynamic]','labelid' => 'trunk-host-dynamic','key' => false,'value' => ($host_static === true ? 'static' : 'dynamic')),$element['trunk']['host-dynamic']['value'],'onchange="xivo_chg_attrib(\'fm_host\',\'fd-trunk-host-static\',(this.value == \'dynamic\' ? 0 : 1))" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => '&nbsp;','name' => 'trunk[host-static]','labelid' => 'trunk-host-static','size' => 15,'value' => ($host_static === true ? $host : '')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->select(array('desc' => $this->bbf('fm_trunk_type'),'name' => 'trunk[type]','labelid' => 'trunk-type','key' => false,'default' => $element['trunk']['type']['default'],'value' => $info['trunk']['type']),$element['trunk']['type']['value'],'onchange="xivo_chgtrunk(this);" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

<div id="sb-part-register" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_register'),'name' => 'register-active','labelid' => 'register-active','checked' => $reg_active),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"'.($trunk_active === true ? ' disabled="disabled"' : '').' onclick="xivo_chg_attrib(\'fm_register\',\'it-register-username\',(this.checked == true ? 0 : 1))"');?>

<?=$form->text(array('desc' => $this->bbf('fm_register_username'),'name' => 'register[username]','labelid' => 'register-username','size' => 15,'value' => $this->varra('info',array('register','username'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_register_password'),'name' => 'register[password]','labelid' => 'register-password','size' => 15,'value' => $this->varra('info',array('register','password'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_register_authuser'),'name' => 'register[authuser]','labelid' => 'register-authuser','size' => 15,'value' => $this->varra('info',array('register','authuser'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_register_host'),'name' => 'register[host]','labelid' => 'register-host','size' => 15,'value' => $this->varra('info',array('register','host'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_register_port'),'name' => 'register[port]','labelid' => 'register-port','size' => 15,'value' => $this->varra('info',array('register','port'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_register_contact'),'name' => 'register[contact]','labelid' => 'register-contact','size' => 15,'value' => $this->varra('info',array('register','contact'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

<div id="sb-part-codec" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_codec-custom'),'name' => 'codec-active','labelid' => 'codec-active','checked' => $codec_active),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" onclick="xivo_chg_attrib(\'fm_codec\',\'it-trunk-disallow\',(this.checked == true ? 0 : 1))"');?>

<?=$form->select(array('desc' => $this->bbf('fm_trunk_codec-disallow'),'name' => 'trunk[disallow]','labelid' => 'trunk-disallow','key' => false),$element['trunk']['disallow']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>


<div id="codeclist" class="fm-field fm-multilist"><p><label id="lb-codeclist" for="it-codeclist"><?=$this->bbf('fm_trunk_codec-allow');?></label></p>
	<div class="slt-outlist">
		<?=$form->select(array('name' => 'codeclist','label' => false,'id' => 'it-codeclist','altkey' => 'id','multiple' => true,'size' => 5,'field' => false,'key' => false),$element['trunk']['allow']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
	</div>
	<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-codeclist','it-codec'); return(false);" title="<?=$this->bbf('bt-incodec');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-incodec'),'class="bt-inlist" id="bt-incodec" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-codec','it-codeclist'); return(false);" title="<?=$this->bbf('bt-outcodec');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outcodec'),'class="bt-outlist" id="bt-outcodec" border="0"');?></a>
	</div>
	<div class="slt-inlist">

		<?=$form->select(array('name' => 'trunk[allow][]','label' => false,'id' => 'it-codec','multiple' => true,'size' => 5,'field' => false,'key' => false),$allow,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

		<div class="bt-updown">

			<a href="#" onclick="xivo_fm_order_selected('it-codec',1); return(false);" title="<?=$this->bbf('bt-upcodec');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt-upcodec'),'class="bt-uplist" id="bt-upcodec" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-codec',-1); return(false);" title="<?=$this->bbf('bt-downcodec');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt-downcodec'),'class="bt-downlist" id="bt-downcodec" border="0"');?></a>

		</div>

	</div>
</div>
<div class="clearboth"></div>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?=$form->text(array('desc' => $this->bbf('fm_trunk_context'),'name' => 'trunk[context]','labelid' => 'trunk-context','size' => 15,'value' => $info['trunk']['context']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_fromuser'),'name' => 'trunk[fromuser]','labelid' => 'trunk-fromuser','size' => 15,'value' => $info['trunk']['fromuser']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_fromdomain'),'name' => 'trunk[fromdomain]','labelid' => 'trunk-fromdomain','size' => 15,'value' => $info['trunk']['fromdomain']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_port'),'name' => 'trunk[port]','labelid' => 'trunk-port','default' => $element['trunk']['port']['default'],'size' => 15,'value' => $info['trunk']['port']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->select(array('desc' => $this->bbf('fm_trunk_dtmfmode'),'name' => 'trunk[dtmfmode]','labelid' => 'trunk-dtmfmode','key' => false,'default' => $element['trunk']['dtmfmode']['default'],'value' => $info['trunk']['dtmfmode']),$element['trunk']['dtmfmode']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trunk_nat'),'name' => 'trunk[nat]','labelid' => 'trunk-nat','default' => $element['trunk']['nat']['default'],'checked' => $info['trunk']['nat']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trunk_qualify'),'name' => 'trunk[qualify]','labelid' => 'trunk-qualify','default' => $element['trunk']['qualify']['default'],'checked' => $info['trunk']['qualify']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trunk_canreinvite'),'name' => 'trunk[canreinvite]','labelid' => 'trunk-canreinvite','default' => $element['trunk']['canreinvite']['default'],'checked' => $info['trunk']['canreinvite']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->select(array('desc' => $this->bbf('fm_trunk_insecure'),'name' => 'trunk[insecure]','labelid' => 'trunk-insecure','empty' => true,'bbf' => array('concatvalue','fm_trunk_insecure-opt-'),'default' => $element['trunk']['insecure']['default'],'value' => $info['trunk']['insecure']),$element['trunk']['insecure']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>
