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

	if(($host = (string) $info['trunk']['host']) === ''
	|| in_array($host,$element['trunk']['host']['value'],true) === true):
		$host_static = false;
	else:
		$host_static = true;
	endif;
?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_trunk_name'),'name' => 'trunk[name]','labelid' => 'trunk-name','size' => 15,'value' => $info['trunk']['name']));?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_username'),'name' => 'trunk[username]','labelid' => 'trunk-username','size' => 15,'value' => $info['trunk']['username']));?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_secret'),'name' => 'trunk[secret]','labelid' => 'trunk-secret','size' => 15,'value' => $info['trunk']['secret']));?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_callerid'),'name' => 'trunk[callerid]','labelid' => 'trunk-callerid','size' => 15,'value' => $info['trunk']['callerid']));?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_calllimit'),'name' => 'trunk[call-limit]','labelid' => 'trunk-calllimit','size' => 10,'value' => $info['trunk']['call-limit']));?>

<?=$form->select(array('desc' => $this->bbf('fm_trunk_host'),'name' => 'trunk[host-dynamic]','labelid' => 'trunk-host-dynamic','bbf' => 'fm_trunk_host-','key' => false,'value' => ($host_static === true ? 'static' : $host)),$element['trunk']['host-dynamic']['value'],'onchange="xivo_chg_attrib(\'fm_host\',\'fd-trunk-host-static\',(this.value != \'static\' ? 0 : 1))"');?>

<?=$form->text(array('desc' => '&nbsp;','name' => 'trunk[host-static]','labelid' => 'trunk-host-static','size' => 15,'value' => ($host_static === true ? $host : '')));?>

<?=$form->select(array('desc' => $this->bbf('fm_trunk_type'),'name' => 'trunk[type]','labelid' => 'trunk-type','key' => false,'default' => $element['trunk']['type']['default'],'value' => $info['trunk']['type']),$element['trunk']['type']['value'],'onchange="xivo_chgtrunk(this);"');?>

</div>

<div id="sb-part-register" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_register'),'name' => 'register-active','labelid' => 'register-active','checked' => $reg_active),($trunk_active === true ? ' disabled="disabled"' : '').' onclick="xivo_chg_attrib(\'fm_register\',\'it-register-username\',(this.checked == true ? 0 : 1))"');?>

<?=$form->text(array('desc' => $this->bbf('fm_register_username'),'name' => 'register[username]','labelid' => 'register-username','size' => 15,'value' => $this->varra('info',array('register','username'))));?>

<?=$form->text(array('desc' => $this->bbf('fm_register_password'),'name' => 'register[password]','labelid' => 'register-password','size' => 15,'value' => $this->varra('info',array('register','password'))));?>

<?=$form->text(array('desc' => $this->bbf('fm_register_authuser'),'name' => 'register[authuser]','labelid' => 'register-authuser','size' => 15,'value' => $this->varra('info',array('register','authuser'))));?>

<?=$form->text(array('desc' => $this->bbf('fm_register_host'),'name' => 'register[host]','labelid' => 'register-host','size' => 15,'value' => $this->varra('info',array('register','host'))));?>

<?=$form->text(array('desc' => $this->bbf('fm_register_port'),'name' => 'register[port]','labelid' => 'register-port','size' => 15,'value' => $this->varra('info',array('register','port'))));?>

<?=$form->text(array('desc' => $this->bbf('fm_register_contact'),'name' => 'register[contact]','labelid' => 'register-contact','size' => 15,'value' => $this->varra('info',array('register','contact'))));?>

</div>

<div id="sb-part-codec" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_codec-custom'),'name' => 'codec-active','labelid' => 'codec-active','checked' => $codec_active),'onclick="xivo_chg_attrib(\'fm_codec\',\'it-trunk-disallow\',(this.checked == true ? 0 : 1))"');?>

<?=$form->select(array('desc' => $this->bbf('fm_trunk_codec-disallow'),'name' => 'trunk[disallow]','labelid' => 'trunk-disallow','key' => false),$element['trunk']['disallow']['value']);?>


<div id="codeclist" class="fm-field fm-multilist"><p><label id="lb-codeclist" for="it-codeclist"><?=$this->bbf('fm_trunk_codec-allow');?></label></p>
	<div class="slt-outlist">
		<?=$form->select(array('name' => 'codeclist','label' => false,'id' => 'it-codeclist','altkey' => 'id','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_codec_name_type-'),$element['trunk']['allow']['value']);?>
	</div>
	<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-codeclist','it-codec'); return(false);" title="<?=$this->bbf('bt-incodec');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-incodec'),'class="bt-inlist" id="bt-incodec" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-codec','it-codeclist'); return(false);" title="<?=$this->bbf('bt-outcodec');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outcodec'),'class="bt-outlist" id="bt-outcodec" border="0"');?></a>
	</div>
	<div class="slt-inlist">

		<?=$form->select(array('name' => 'trunk[allow][]','label' => false,'id' => 'it-codec','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_codec_name_type-'),$allow);?>

		<div class="bt-updown">

			<a href="#" onclick="xivo_fm_order_selected('it-codec',1); return(false);" title="<?=$this->bbf('bt-upcodec');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt-upcodec'),'class="bt-uplist" id="bt-upcodec" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-codec',-1); return(false);" title="<?=$this->bbf('bt-downcodec');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt-downcodec'),'class="bt-downlist" id="bt-downcodec" border="0"');?></a>

		</div>

	</div>
</div>
<div class="clearboth"></div>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?=$form->text(array('desc' => $this->bbf('fm_trunk_context'),'name' => 'trunk[context]','labelid' => 'trunk-context','size' => 15,'value' => $info['trunk']['context']));?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_fromuser'),'name' => 'trunk[fromuser]','labelid' => 'trunk-fromuser','size' => 15,'value' => $info['trunk']['fromuser']));?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_fromdomain'),'name' => 'trunk[fromdomain]','labelid' => 'trunk-fromdomain','size' => 15,'value' => $info['trunk']['fromdomain']));?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_port'),'name' => 'trunk[port]','labelid' => 'trunk-port','default' => $element['trunk']['port']['default'],'size' => 15,'value' => $info['trunk']['port']));?>

<?=$form->select(array('desc' => $this->bbf('fm_trunk_dtmfmode'),'name' => 'trunk[dtmfmode]','labelid' => 'trunk-dtmfmode','key' => false,'default' => $element['trunk']['dtmfmode']['default'],'value' => $info['trunk']['dtmfmode']),$element['trunk']['dtmfmode']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trunk_nat'),'name' => 'trunk[nat]','labelid' => 'trunk-nat','default' => $element['trunk']['nat']['default'],'checked' => $info['trunk']['nat']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trunk_qualify'),'name' => 'trunk[qualify]','labelid' => 'trunk-qualify','default' => $element['trunk']['qualify']['default'],'checked' => $info['trunk']['qualify']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trunk_canreinvite'),'name' => 'trunk[canreinvite]','labelid' => 'trunk-canreinvite','default' => $element['trunk']['canreinvite']['default'],'checked' => $info['trunk']['canreinvite']));?>

<?=$form->select(array('desc' => $this->bbf('fm_trunk_insecure'),'name' => 'trunk[insecure]','labelid' => 'trunk-insecure','empty' => true,'bbf' => array('concatvalue','fm_trunk_insecure-opt-'),'default' => $element['trunk']['insecure']['default'],'value' => $info['trunk']['insecure']),$element['trunk']['insecure']['value']);?>

</div>
