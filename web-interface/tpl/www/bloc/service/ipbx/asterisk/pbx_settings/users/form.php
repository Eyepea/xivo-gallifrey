<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->vars('info');
	$element = $this->vars('element');

	$moh_list = $this->vars('moh_list');
	$autoprov_list = $this->vars('autoprov_list');

	$act = $this->vars('act');

	$ringgroup = xivo_bool($info['ufeatures']['ringgroup']);

	$vm_active = $info['voicemail']['commented'];

	if($vm_active !== null):
		$vm_active = xivo_bool($vm_active) === true ? false : true;
	endif;

	$allow = $info['protocol']['allow'];

	if(empty($allow) === true):
		$codec_active = false;
	else:
		$codec_active = true;
	endif;

	if(($host = (string) $this->varra('info',array('protocol','host-static'))) !== ''):
		$host_static = true;
	elseif(($host = (string) $this->varra('info',array('protocol','host'))) === 'dynamic' || $host === ''):
		$host_static = false;
	else:
		$host_static = true;
	endif;

	if(empty($info['autoprov']) === true):
		$vendormodel = '';
	else:
		$vendormodel = $info['autoprov']['vendor'].'.'.$info['autoprov']['model'];
	endif;
?>

<div id="sb-part-general">

<?=$form->text(array('desc' => $this->bbf('fm_userfeatures_firstname'),'name' => 'ufeatures[firstname]','labelid' => 'ufeatures-firstname','value' => $info['ufeatures']['firstname'],'size' => 15),'onchange="xivo_chgname();" onfocus="xivo_cpyname(); this.className=\'it-mfocus\';" onblur="xivo_chgname(); this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_userfeatures_lastname'),'name' => 'ufeatures[lastname]','labelid' => 'ufeatures-lastname','value' => $info['ufeatures']['lastname'],'size' => 15),'onchange="xivo_chgname();" onfocus="xivo_cpyname(); this.className=\'it-mfocus\';" onblur="xivo_chgname(); this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_name'),'name' => 'protocol[name]','labelid' => 'protocol-name','value' => $info['protocol']['name'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_ufeatures_number'),'name' => 'ufeatures[number]','labelid' => 'ufeatures-number','value' => $info['ufeatures']['number'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_secret'),'name' => 'protocol[secret]','labelid' => 'protocol-secret','value' => $info['protocol']['secret'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_popupwidget'),'name' => 'ufeatures[popupwidget]','labelid' => 'ufeatures-popupwidget','default' => $element['ufeatures']['popupwidget']['default'],'checked' => $info['ufeatures']['popupwidget']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_userfeatures_ringseconds'),'name' => 'ufeatures[ringseconds]','labelid' => 'ufeatures-ringseconds','bbf' => array('mixkey','fm_userfeatures_ringseconds-opt'),'key' => false,'default' => $element['ufeatures']['ringseconds']['default'],'value' => (isset($info['ufeatures']['ringseconds']) === true ? (int) $info['ufeatures']['ringseconds'] : null)),$element['ufeatures']['ringseconds']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_userfeatures_simultcalls'),'name' => 'ufeatures[simultcalls]','labelid' => 'ufeatures-simultcalls','key' => false,'default' => $element['ufeatures']['simultcalls']['default'],'value' => (isset($info['ufeatures']['simultcalls']) === true ? (int) $info['ufeatures']['simultcalls'] : null)),$element['ufeatures']['simultcalls']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_protocol'),'name' => 'protocol[protocol]','labelid' => 'protocol-protocol','key' => true,'value' => $info['ufeatures']['protocol']),$this->vars('protocol'),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" onchange="xivo_chgprotocol(this);"');?>

</div>

<div id="sb-part-group" class="b-nodisplay">
	<?=$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/groups');?>
</div>

<div id="sb-part-codec" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_codec-custom'),'name' => 'codec-active','labelid' => 'codec-active','checked' => $codec_active),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" onclick="xivo_chg_attrib(\'fm_codec\',\'it-\'+xivo_protocol+\'-protocol-disallow\',(this.checked == true ? 0 : 1))"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_codec-disallow'),'name' => 'protocol[disallow]','labelid' => 'sip-protocol-disallow','key' => false),$element['protocol']['sip']['disallow']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_codec-disallow'),'name' => 'protocol[disallow]','labelid' => 'iax-protocol-disallow','key' => false),$element['protocol']['iax']['disallow']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<div id="codeclist" class="fm-field"><p><label id="lb-codeclist" for="it-codeclist" onclick="xivo_eid('it-'+xivo_protocol+'-codeclist').focus();"><?=$this->bbf('fm_protocol_codec-allow');?></label></p>
	<div>
		<?=$form->slt(array('name' => 'codeclist','label' => false,'id' => 'it-sip-codeclist','key_val' => 'id','multiple' => true,'size' => 5,'field' => false,'key' => false),$element['protocol']['sip']['allow']['value'],'class="codeclisted" onfocus="this.className=\'it-mfocus codeclisted\';" onblur="this.className=\'it-mblur codeclisted\';"');?>
		<?=$form->slt(array('name' => 'codeclist','label' => false,'id' => 'it-iax-codeclist','key_val' => 'id','multiple' => true,'size' => 5,'field' => false,'key' => false),$element['protocol']['iax']['allow']['value'],'class="codeclisted" onfocus="this.className=\'it-mfocus codeclisted\';" onblur="this.className=\'it-mblur codeclisted\';"');?>

	</div>
	<div id="inout-codec">

		<a href="#" onclick="xivo_fm_move_selected('it-'+xivo_protocol+'-codeclist','it-'+xivo_protocol+'-codec'); return(false);" title="<?=$this->bbf('bt-incodec');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-incodec'),'id="bt-incodec" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-'+xivo_protocol+'-codec','it-'+xivo_protocol+'-codeclist'); return(false);" title="<?=$this->bbf('bt-outcodec');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outcodec'),'id="bt-outcodec" border="0"');?></a>

	</div>
	<div id="select-codec" class="txt-left">

		<?=$form->slt(array('name' => 'protocol[allow][]','label' => false,'id' => 'it-sip-codec','multiple' => true,'size' => 5,'field' => false,'key' => false),$allow,'class="codecselected" onfocus="this.className=\'it-mfocus codecselected\';" onblur="this.className=\'it-mblur codecselected\';"');?>

		<?=$form->slt(array('name' => 'protocol[allow][]','label' => false,'id' => 'it-iax-codec','multiple' => true,'size' => 5,'field' => false,'key' => false),$allow,'class="codecselected" onfocus="this.className=\'it-mfocus codecselected\';" onblur="this.className=\'it-mblur codecselected\';"');?>

		<div id="updown-codec" class="txt-left">

			<a href="#" onclick="xivo_fm_order_selected('it-'+xivo_protocol+'-codec',1); return(false);" title="<?=$this->bbf('bt-upcodec');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt-upcodec'),'id="bt-upcodec" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-'+xivo_protocol+'-codec',-1); return(false);" title="<?=$this->bbf('bt-downcodec');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt-downcodec'),'id="bt-downcodec" border="0"');?></a>

		</div>
	</div>
</div>

</div>
<div class="clearboth"></div>

<div id="sb-part-voicemail" class="b-nodisplay">

	<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-active'),'name' => 'voicemail-active','labelid' => 'voicemail-active','checked' => $vm_active),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" onclick="xivo_chg_attrib(\'fm_voicemail\',\'it-voicemail-fullname\',(this.checked == true ? 0 : 1))"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_voicemail_fullname'),'name' => 'voicemail[fullname]','labelid' => 'voicemail-fullname','value' => $info['voicemail']['fullname'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_voicemail_password'),'name' => 'voicemail[password]','labelid' => 'voicemail-password','value' => $info['voicemail']['password'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_voicemail_email'),'name' => 'voicemail[email]','labelid' => 'voicemail-email','value' => $info['voicemail']['email'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail_attach'),'name' => 'voicemail[attach]','labelid' => 'voicemail-attach','checked' => $this->varra('info',array('voicemail','attach'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail_delete'),'name' => 'voicemail[delete]','labelid' => 'voicemail-delete','checked' => $this->varra('info',array('voicemail','delete'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

<div id="sb-part-autoprov" class="b-nodisplay">

<?php
	if($act === 'edit'):

	echo $form->slt(array('desc' => $this->bbf('fm_autoprov_modact'),'name' => 'autoprov[modact]','labelid' => 'autoprov-modact','bbf' => 'fm_autoprov_modact-','key' => false,'empty' => true),$element['autoprov']['modact']['value'],'onchange="xivo_chg_attrib(\'fm_autoprov-\'+xivo_protocol,\'it-autoprov-modact\',(this.value != \'\' ? 0 : 1));" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');

	endif;

	if(is_array($info['autoprov']) === false):
?>

	<?=$form->slt(array('desc' => $this->bbf('fm_autoprov_vendormodel'),'name' => 'autoprov[vendormodel]','labelid' => 'autoprov-vendormodel','optgroup' => array('key' => 'name'),'empty' => true,'key' => 'label','key_val' => 'path','value' => $vendormodel),$autoprov_list,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_autoprov_macaddr'),'name' => 'autoprov[macaddr]','labelid' => 'autoprov-macaddr','value' => $info['autoprov']['macaddr'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?php
	elseif(isset($autoprov_list[$info['autoprov']['vendor']]) === true):
?>

<p id="fd-autoprov-vendormodel" class="fm-field">
	<label id="lb-autoprov-vendormodel"><span class="fm-desc"><?=$this->bbf('fm_autoprov_vendormodel');?></span>&nbsp;<?=$autoprov_list[$info['autoprov']['vendor']]['name']?> <?=$autoprov_list[$info['autoprov']['vendor']]['model'][$info['autoprov']['model']]['label']?></label>
</p>

<p id="fd-autoprov-macaddr" class="fm-field">
	<label id="lb-autoprov-macaddr"><span class="fm-desc"><?=$this->bbf('fm_autoprov_macaddr');?></span>&nbsp;<?=$info['autoprov']['macaddr']?></label>
</p>
<?php
	endif;
?>
</div>

<div id="sb-part-advanced" class="b-nodisplay">

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_callerid'),'name' => 'protocol[callerid]','labelid' => 'protocol-callerid','value' => $info['protocol']['callerid'],'size' => 15,'notag' => false),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?php
	if($moh_list !== false):
		echo $form->slt(array('desc' => $this->bbf('fm_userfeatures_musiconhold'),'name' => 'ufeatures[musiconhold]','labelid' => 'ufeatures-musiconhold','key' => 'category','empty' => true,'default' => $element['ufeatures']['musiconhold']['default'],'value' => $info['ufeatures']['musiconhold']),$moh_list,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');
	endif;
?>

	<?=$form->slt(array('desc' => $this->bbf('fm_protocol_host'),'name' => 'protocol[host-dynamic]','labelid' => 'sip-protocol-host-dynamic','bbf' => 'fm_protocol_host-','key' => false,'value' => ($host_static === true ? 'static' : 'dynamic')),$element['protocol']['sip']['host-dynamic']['value'],'onchange="xivo_chg_attrib(\'fm_host\',\'fd-sip-protocol-host-static\',(this.value == \'dynamic\' ? 0 : 1))" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => '&nbsp;','name' => 'protocol[host-static]','labelid' => 'sip-protocol-host-static','size' => 15,'value' => ($host_static === true ? $host : '')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->slt(array('desc' => $this->bbf('fm_protocol_host'),'name' => 'protocol[host-dynamic]','labelid' => 'iax-protocol-host-dynamic','bbf' => 'fm_protocol_host-','key' => false,'value' => ($host_static === true ? 'static' : 'dynamic')),$element['protocol']['iax']['host-dynamic']['value'],'onchange="xivo_chg_attrib(\'fm_host\',\'fd-iax-protocol-host-static\',(this.value == \'dynamic\' ? 0 : 1))" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => '&nbsp;','name' => 'protocol[host-static]','labelid' => 'iax-protocol-host-static','size' => 15,'value' => ($host_static === true ? $host : '')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->slt(array('desc' => $this->bbf('fm_protocol_dtmfmode'),'name' => 'protocol[dtmfmode]','labelid' => 'protocol-dtmfmode','key' => false,'value' => $this->varra('info',array('protocol','dtmfmode'))),$element['protocol']['sip']['dtmfmode']['value'],'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_canreinvite'),'name' => 'protocol[canreinvite]','labelid' => 'protocol-canreinvite','default' => $element['protocol']['sip']['canreinvite']['default'],'checked' => $this->varra('info',array('protocol','canreinvite'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_context'),'name' => 'protocol[context]','labelid' => 'sip-protocol-context','default' => $element['protocol']['sip']['context']['default'],'value' => $info['protocol']['context'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_context'),'name' => 'protocol[context]','labelid' => 'iax-protocol-context','default' => $element['protocol']['iax']['context']['default'],'value' => $info['protocol']['context'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->slt(array('desc' => $this->bbf('fm_protocol_amaflags'),'name' => 'protocol[amaflags]','labelid' => 'sip-protocol-amaflags','key' => false,'default' => $element['protocol']['sip']['amaflags']['default'],'value' => $info['protocol']['amaflags']),$element['protocol']['sip']['amaflags']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->slt(array('desc' => $this->bbf('fm_protocol_amaflags'),'name' => 'protocol[amaflags]','labelid' => 'iax-protocol-amaflags','key' => false,'default' => $element['protocol']['sip']['amaflags']['default'],'value' => $info['protocol']['amaflags']),$element['protocol']['iax']['amaflags']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_accountcode'),'name' => 'protocol[accountcode]','labelid' => 'protocol-accountcode','value' => $info['protocol']['accountcode'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_nat'),'name' => 'protocol[nat]','labelid' => 'protocol-nat','default' => $element['protocol']['sip']['nat']['default'],'checked' => $this->varra('info',array('protocol','nat'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_qualify'),'name' => 'protocol[qualify]','labelid' => 'sip-protocol-qualify','default' => $element['protocol']['sip']['qualify']['default'],'checked' => $info['protocol']['qualify']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_qualify'),'name' => 'protocol[qualify]','labelid' => 'iax-protocol-qualify','default' => $element['protocol']['iax']['qualify']['default'],'checked' => $info['protocol']['qualify']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<div id="comment" class="fm-field"><p><label id="lb-ufeatures-comment" for="it-ufeatures-comment"><?=$this->bbf('fm_userfeatures_comment');?></label></p>
<?=$form->textarea(array('field' => false,'label' => false,'name' => 'ufeatures[comment]','id' => 'it-ufeatures-comment','cols' => 60,'rows' => 5),$info['ufeatures']['comment'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

</div>
