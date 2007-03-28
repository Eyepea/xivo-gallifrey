<?php
	$form = &$this->get_module('form');
	$url = $this->get_module('url');

	$protocol_elt = $this->vars('protocol_elt');
	$ufeatures_elt = $this->vars('ufeatures_elt');
	$voicemail_elt = $this->vars('voicemail_elt');
	$group_list = $this->vars('group_list');
	$info = $this->vars('info');
	$moh_list = $this->vars('moh_list');

	if((string) ($host = $this->varra('info',array('protocol','host-static'))) !== ''):
		$host_static = true;
	else:
		$host_static = false;
	endif;
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>

	<?=$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/submenu');?>

	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-group'); xivo_fm_select('it-'+xivo_protocol+'-codec');">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'add'));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>

<div id="sb-part-general">

<?=$form->text(array('desc' => $this->bbf('fm_userfeatures_firstname'),'name' => 'ufeatures[firstname]','labelid' => 'ufeatures-firstname','size' => 15),'onchange="xivo_chgname();" onfocus="xivo_cpyname(); this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_userfeatures_lastname'),'name' => 'ufeatures[lastname]','labelid' => 'ufeatures-lastname','size' => 15),'onchange="xivo_chgname();" onfocus="xivo_cpyname(); this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_name'),'name' => 'protocol[name]','labelid' => 'protocol-name','size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_ufeatures_number'),'name' => 'ufeatures[number]','labelid' => 'ufeatures-number','size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_secret'),'name' => 'protocol[secret]','labelid' => 'protocol-secret','size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_popupwidget'),'name' => 'ufeatures[popupwidget]','labelid' => 'ufeatures-popupwidget','default' => $ufeatures_elt['popupwidget']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_userfeatures_ringseconds'),'name' => 'ufeatures[ringseconds]','labelid' => 'ufeatures-ringseconds','bbf' => array('paramkey','fm_userfeatures_ringseconds-opt'),'key' => false,'default' => $ufeatures_elt['ringseconds']['default']),$ufeatures_elt['ringseconds']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_userfeatures_simultcalls'),'name' => 'ufeatures[simultcalls]','labelid' => 'ufeatures-simultcalls','key' => false,'default' => $ufeatures_elt['simultcalls']['default']),$ufeatures_elt['simultcalls']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_protocol'),'name' => 'protocol[protocol]','labelid' => 'protocol-protocol','key' => true),$this->vars('protocol'),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" onchange="xivo_chgprotocol(this);"');?>

</div>

<div id="sb-part-group" class="b-nodisplay">
<?php
	if($this->vars('group') === true):
?>
		<div id="grouplist" class="fm-field">
			<div>
		<?=$form->slt(array('name' => 'grouplist','label' => false,'id' => 'it-grouplist','key_val' => 'id','key' => 'name','multiple' => true,'size' => 5,'field' => false),$group_list,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"')?>
			</div>
			<div id="inout-group">

		<a href="#" onclick="xivo_ingroup(); return(false);" title="<?=$this->bbf('bt-ingroup')?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-ingroup'),'id="bt-ingroup" border="0"');?></a><br />

		<a href="#" onclick="xivo_outgroup(); return(false);" title="<?=$this->bbf('bt-outgroup')?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outgroup'),'id="bt-outgroup" border="0"');?></a>

			</div>
			<div class="txt-left">

		<?=$form->slt(array('name' => 'group[]','label' => false,'id' => 'it-group','multiple' => true,'size' => 5,'field' => false,'key' => 'name','key_val' => 'id'),null,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

			</div>
		</div>
		<div class="clearboth">

		<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_ringgroup'),'name' => 'ufeatures[ringgroup]','labelid' => 'ufeatures-ringgroup','default' => $ufeatures_elt['ringgroup']['default']),'disabled="disabled" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" onclick="xivo_eid(\'ringgroup\').style.display = this.checked == true ? \'block\' : \'none\';"');?>

			<div id="ringgroup" class="b-nodisplay">

		<?=$form->slt(array('desc' => $this->bbf('fm_usergroup'),'name' => 'usergroup','id' => 'it-usergroup','key' => 'name','key_val' => 'id'),null,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

			</div>
		</div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add'),'</div>';
	endif;
?>
</div>

<div id="sb-part-codec" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_codec-custom'),'name' => 'codec-active','labelid' => 'codec-active','checked' => false),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" onclick="xivo_chg_attrib(\'fm_codec\',\'it-\'+xivo_protocol+\'-protocol-disallow\',(this.checked == true ? 1 : 2))"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_codec-disallow'),'name' => 'protocol[disallow]','labelid' => 'sip-protocol-disallow','key' => false),$protocol_elt['sip']['disallow']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_codec-disallow'),'name' => 'protocol[disallow]','labelid' => 'iax-protocol-disallow','key' => false),$protocol_elt['iax']['disallow']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<div id="codeclist" class="fm-field"><p><label id="lb-codeclist" for="it-codeclist" onclick="xivo_eid('it-'+xivo_protocol+'-codeclist').focus();"><?=$this->bbf('fm_protocol_codec-allow')?></label></p>
	<div>

		<?=$form->slt(array('name' => 'codeclist','label' => false,'id' => 'it-sip-codeclist','key_val' => 'id','multiple' => true,'size' => 5,'field' => false,'key' => false),$protocol_elt['sip']['allow']['value'],'class="codeclisted" onfocus="this.className=\'it-mfocus codeclisted\';" onblur="this.className=\'it-mblur codeclisted\';"')?>
		<?=$form->slt(array('name' => 'codeclist','label' => false,'id' => 'it-iax-codeclist','key_val' => 'id','multiple' => true,'size' => 5,'field' => false,'key' => false),$protocol_elt['iax']['allow']['value'],'class="codeclisted" onfocus="this.className=\'it-mfocus codeclisted\';" onblur="this.className=\'it-mblur codeclisted\';"')?>

	</div>
	<div id="inout-codec">

		<a href="#" onclick="xivo_fm_move_selected('it-'+xivo_protocol+'-codeclist','it-'+xivo_protocol+'-codec'); return(false);" title="<?=$this->bbf('bt-incodec')?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-incodec'),'id="bt-incodec" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-'+xivo_protocol+'-codec','it-'+xivo_protocol+'-codeclist'); return(false);" title="<?=$this->bbf('bt-outcodec')?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outcodec'),'id="bt-outcodec" border="0"');?></a>

	</div>
	<div id="select-codec" class="txt-left">

		<?=$form->slt(array('name' => 'protocol[allow][]','label' => false,'id' => 'it-sip-codec','multiple' => true,'size' => 5,'field' => false,'key' => false),$this->varra('info',array('protocol','allow')),'class="codecselected" onfocus="this.className=\'it-mfocus codecselected\';" onblur="this.className=\'it-mblur codecselected\';"');?>

		<?=$form->slt(array('name' => 'protocol[allow][]','label' => false,'id' => 'it-iax-codec','multiple' => true,'size' => 5,'field' => false,'key' => false),$this->varra('info',array('protocol','allow')),'class="codecselected" onfocus="this.className=\'it-mfocus codecselected\';" onblur="this.className=\'it-mblur codecselected\';"');?>

		<div id="updown-codec" class="txt-left">

			<a href="#" onclick="xivo_fm_order_selected('it-'+xivo_protocol+'-codec',1); return(false);" title="<?=$this->bbf('bt-upcodec')?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt-upcodec'),'id="bt-upcodec" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-'+xivo_protocol+'-codec',-1); return(false);" title="<?=$this->bbf('bt-downcodec')?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt-downcodec'),'id="bt-downcodec" border="0"');?></a>

		</div>
	</div>
</div>

</div>
<div class="clearboth"></div>

<div id="sb-part-voicemail" class="b-nodisplay">

	<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-active'),'name' => 'voicemail-active','labelid' => 'voicemail-active','checked' => false),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" onclick="xivo_chg_attrib(\'fm_voicemail\',\'it-voicemail-fullname\',(this.checked == true ? 1 : 2))"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_voicemail_fullname'),'name' => 'voicemail[fullname]','labelid' => 'voicemail-fullname','size' => 15,'default' => $voicemail_elt['fullname']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_voicemail_password'),'name' => 'voicemail[password]','labelid' => 'voicemail-password','size' => 15,'default' => $voicemail_elt['password']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_voicemail_email'),'name' => 'voicemail[email]','labelid' => 'voicemail-email','size' => 15,'default' => $voicemail_elt['email']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail_attach'),'name' => 'voicemail[attach]','labelid' => 'voicemail-attach','default' => $voicemail_elt['attach']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail_delete'),'name' => 'voicemail[delete]','labelid' => 'voicemail-delete','default' => $voicemail_elt['delete']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

<div id="sb-part-advanced" class="b-nodisplay">

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_callerid'),'name' => 'protocol[callerid]','labelid' => 'protocol-callerid','size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->slt(array('desc' => $this->bbf('fm_userfeatures_musiconhold'),'name' => 'ufeatures[musiconhold]','labelid' => 'ufeatures-musiconhold','key' => 'category','empty' => true,'default' => $ufeatures_elt['musiconhold']['default']),$moh_list,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->slt(array('desc' => $this->bbf('fm_protocol_host'),'name' => 'protocol[host-dynamic]','labelid' => 'sip-protocol-host-dynamic','bbf' => 'fm_protocol_host-','key' => false,'value' => ($host_static === true ? 'static' : 'dynamic')),$protocol_elt['sip']['host-dynamic']['value'],'onchange="xivo_chg_attrib(\'fm_host\',\'fd-sip-protocol-host-static\',(this.value == \'dynamic\' ? 1 : 2))" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => '&nbsp;','name' => 'protocol[host-static]','labelid' => 'sip-protocol-host-static','size' => 15,'value' => $host),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->slt(array('desc' => $this->bbf('fm_protocol_host'),'name' => 'protocol[host-dynamic]','labelid' => 'iax-protocol-host-dynamic','bbf' => 'fm_protocol_host-','key' => false,'value' => ($host_static === true ? 'static' : 'dynamic')),$protocol_elt['iax']['host-dynamic']['value'],'onchange="xivo_chg_attrib(\'fm_host\',\'fd-iax-protocol-host-static\',(this.value == \'dynamic\' ? 1 : 2))" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => '&nbsp;','name' => 'protocol[host-static]','labelid' => 'iax-protocol-host-static','size' => 15,'value' => $host),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->slt(array('desc' => $this->bbf('fm_protocol_dtmfmode'),'name' => 'protocol[dtmfmode]','labelid' => 'protocol-dtmfmode','key' => false),$protocol_elt['sip']['dtmfmode']['value'],'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_canreinvite'),'name' => 'protocol[canreinvite]','labelid' => 'protocol-canreinvite','default' => $protocol_elt['sip']['canreinvite']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_context'),'name' => 'protocol[context]','labelid' => 'sip-protocol-context','default' => $protocol_elt['sip']['context']['default'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_context'),'name' => 'protocol[context]','labelid' => 'iax-protocol-context','default' => $protocol_elt['iax']['context']['default'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->slt(array('desc' => $this->bbf('fm_protocol_amaflags'),'name' => 'protocol[amaflags]','labelid' => 'sip-protocol-amaflags','key' => false,'default' => $protocol_elt['sip']['amaflags']['default']),$protocol_elt['sip']['amaflags']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->slt(array('desc' => $this->bbf('fm_protocol_amaflags'),'name' => 'protocol[amaflags]','labelid' => 'iax-protocol-amaflags','key' => false,'default' => $protocol_elt['sip']['amaflags']['default']),$protocol_elt['iax']['amaflags']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_accountcode'),'name' => 'protocol[accountcode]','labelid' => 'protocol-accountcode','size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_nat'),'name' => 'protocol[nat]','labelid' => 'protocol-nat','default' => $protocol_elt['sip']['nat']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_qualify'),'name' => 'protocol[qualify]','labelid' => 'sip-protocol-qualify','default' => $protocol_elt['sip']['qualify']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_qualify'),'name' => 'protocol[qualify]','labelid' => 'iax-protocol-qualify','default' => $protocol_elt['iax']['qualify']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<div id="comment" class="fm-field"><p><label id="lb-ufeatures-comment" for="it-ufeatures-comment"><?=$this->bbf('fm_userfeatures_comment')?></label></p>
<?=$form->textarea(array('field' => false,'label' => false,'name' => 'ufeatures[comment]','id' => 'it-ufeatures-comment','cols' => 60,'rows' => 5),null,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

</div>

	<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>

</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
