<?php
	$form = &$this->get_module('form');
	$url = $this->get_module('url');

	$protocol_elt = $this->vars('protocol_elt');
	$ufeatures_elt = $this->vars('ufeatures_elt');
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
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-group'); xivo_fm_select('it-'+xivo_protocol+'-codec');">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'add'));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_callerid'),'name' => 'protocol[callerid]','labelid' => 'protocol-callerid','size' => 15),'onchange="xivo_eid(\'it-voicemail-fullname\').value = this.tmp == true ? this.value : xivo_eid(\'it-voicemail-fullname\').value;" onfocus="this.tmp = xivo_eid(\'it-voicemail-fullname\').value == this.value ? true : false; this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_name'),'name' => 'protocol[name]','labelid' => 'protocol-name','size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_ufeatures_number'),'name' => 'ufeatures[number]','labelid' => 'ufeatures-number','size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_secret'),'name' => 'protocol[secret]','labelid' => 'protocol-secret','size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_protocol'),'name' => 'protocol[protocol]','labelid' => 'protocol-protocol','key' => true),$this->vars('protocol'),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" onchange="xivo_chgprotocol(this);"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_popupwidget'),'name' => 'ufeatures[popupwidget]','labelid' => 'ufeatures-popupwidget','default' => $ufeatures_elt['popupwidget']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_userfeatures_ringseconds'),'name' => 'ufeatures[ringseconds]','labelid' => 'ufeatures-ringseconds','bbf' => array('key','fm_userfeatures_ringseconds-opt'),'key' => false,'default' => $ufeatures_elt['ringseconds']['default']),$ufeatures_elt['ringseconds']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_userfeatures_simultcalls'),'name' => 'ufeatures[simultcalls]','labelid' => 'ufeatures-simultcalls','key' => false,'default' => $ufeatures_elt['simultcalls']['default']),$ufeatures_elt['simultcalls']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_userfeatures_musiconhold'),'name' => 'ufeatures[musiconhold]','labelid' => 'ufeatures-musiconhold','key' => 'category','empty' => true,'default' => $ufeatures_elt['musiconhold']['default']),$moh_list,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_host'),'name' => 'protocol[host-dynamic]','labelid' => 'sip-protocol-host-dynamic','bbf' => 'fm_protocol_host-','key' => false,'value' => ($host_static === true ? 'static' : 'dynamic')),$protocol_elt['sip']['host-dynamic']['value'],'onchange="xivo_chg_attrib(\'fm_host\',\'fd-sip-protocol-host-static\',(this.value == \'dynamic\' ? 1 : 2))" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('name' => 'protocol[host-static]','labelid' => 'sip-protocol-host-static','size' => 15,'value' => $host),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_host'),'name' => 'protocol[host-dynamic]','labelid' => 'iax-protocol-host-dynamic','bbf' => 'fm_protocol_host-','key' => false,'value' => ($host_static === true ? 'static' : 'dynamic')),$protocol_elt['iax']['host-dynamic']['value'],'onchange="xivo_chg_attrib(\'fm_host\',\'fd-iax-protocol-host-static\',(this.value == \'dynamic\' ? 1 : 2))" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('name' => 'protocol[host-static]','labelid' => 'iax-protocol-host-static','size' => 15,'value' => $host),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_dtmfmode'),'name' => 'protocol[dtmfmode]','labelid' => 'protocol-dtmfmode','key' => false),$protocol_elt['sip']['dtmfmode']['value'],'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>

<?php
	if($this->vars('group') === true):
?>
		<div id="grouplist" class="fm-field"><label id="lb-grouplist" for="it-grouplist"><?=$this->bbf('fm_group')?></label><br />
			<div>
		<?=$form->slt(array('name' => 'grouplist','label' => false,'id' => 'it-grouplist','key_val' => 'id','key' => 'name','multiple' => true,'size' => 5,'field' => false),$group_list,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"')?>
			</div>
			<div id="inout-group">
		<?=$form->button(array('name' => 'ingroup','id' => 'it-bt-ingroup','value' => $this->bbf('fm_bt-ingroup')),'onclick="xivo_ingroup();"')?>
		<?=$form->button(array('name' => 'outgroup','id' => 'it-bt-outgroup','value' => $this->bbf('fm_bt-outgroup')),'onclick="xivo_outgroup();"');?>
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

<?=$form->checkbox(array('desc' => $this->bbf('fm_codec-active'),'name' => 'codec-active','labelid' => 'codec-active','checked' => false),'onclick="xivo_eid(\'codec\').style.display = this.checked == true ? \'block\' : \'none\';"');?>

<div id="codec" class="b-nodisplay">

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_codec-disallow'),'name' => 'protocol[disallow]','labelid' => 'sip-protocol-disallow','key' => false),$protocol_elt['sip']['disallow']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_codec-disallow'),'name' => 'protocol[disallow]','labelid' => 'iax-protocol-disallow','key' => false),$protocol_elt['iax']['disallow']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<div id="codeclist" class="fm-field"><label id="lb-codeclist" for="it-codeclist" onmouseover="this.for = 'it-'+xivo_protocol+'-codeclist';"><?=$this->bbf('fm_protocol_codec-allow')?></label><br />
	<div>
		<?=$form->slt(array('name' => 'codeclist','label' => false,'id' => 'it-sip-codeclist','key_val' => 'id','multiple' => true,'size' => 5,'field' => false,'key' => false),$protocol_elt['sip']['allow']['value'],'class="codeclisted" onfocus="this.className=\'it-mfocus codeclisted\';" onblur="this.className=\'it-mblur codeclisted\';"')?>
		<?=$form->slt(array('name' => 'codeclist','label' => false,'id' => 'it-iax-codeclist','key_val' => 'id','multiple' => true,'size' => 5,'field' => false,'key' => false),$protocol_elt['iax']['allow']['value'],'class="codeclisted" onfocus="this.className=\'it-mfocus codeclisted\';" onblur="this.className=\'it-mblur codeclisted\';"')?>

	</div>
	<div id="inout-codec">
		<?=$form->button(array('name' => 'incodec','id' => 'it-bt-incodec','value' => $this->bbf('fm_bt-incodec')),'onclick="xivo_fm_move_selected(\'it-\'+xivo_protocol+\'-codeclist\',\'it-\'+xivo_protocol+\'-codec\');"')?>
	
		<?=$form->button(array('name' => 'outcodec','id' => 'it-bt-outcodec','value' => $this->bbf('fm_bt-outcodec')),'onclick="xivo_fm_move_selected(\'it-\'+xivo_protocol+\'-codec\',\'it-\'+xivo_protocol+\'-codeclist\');"');?>
	</div>
	<div id="select-codec" class="txt-left">

		<?=$form->slt(array('name' => 'protocol[allow][]','label' => false,'id' => 'it-sip-codec','multiple' => true,'size' => 5,'field' => false,'key' => false),$this->varra('info',array('protocol','allow')),'class="codecselected" onfocus="this.className=\'it-mfocus codecselected\';" onblur="this.className=\'it-mblur codecselected\';"');?>

		<?=$form->slt(array('name' => 'protocol[allow][]','label' => false,'id' => 'it-iax-codec','multiple' => true,'size' => 5,'field' => false,'key' => false),$this->varra('info',array('protocol','allow')),'class="codecselected" onfocus="this.className=\'it-mfocus codecselected\';" onblur="this.className=\'it-mblur codecselected\';"');?>

		<div id="updown-codec" class="txt-left">
			<?=$form->button(array('name' => 'upcodec','id' => 'it-bt-upcodec','value' => '&uarr;','schars' => true),'onclick="xivo_fm_order_selected(\'it-\'+xivo_protocol+\'-codec\',1);"')?>

			<?=$form->button(array('name' => 'downcodec','id' => 'it-bt-downcodec','value' => '&darr;','schars' => true),'onclick="xivo_fm_order_selected(\'it-\'+xivo_protocol+\'codec\',-1);"');?>
		</div>
	</div>
</div>

</div>
<div class="clearboth"></div>

<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail'),'name' => 'voicemail-active','labelid' => 'voicemail','checked' => false),'onclick="xivo_eid(\'voicemail\').style.display = this.checked == true ? \'block\' : \'none\';"');?>

<div id="voicemail" class="b-nodisplay">
	<?=$form->text(array('desc' => $this->bbf('fm_voicemail_fullname'),'name' => 'voicemail[fullname]','labelid' => 'voicemail-fullname','size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_voicemail_password'),'name' => 'voicemail[password]','labelid' => 'voicemail-password','size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_voicemail_email'),'name' => 'voicemail[email]','labelid' => 'voicemail-email','size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail_attach'),'name' => 'voicemail[attach]','labelid' => 'voicemail-attach','checked' => false),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail_delete'),'name' => 'voicemail[delete]','labelid' => 'voicemail-delete','checked' => false),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<div id="advanced" class="b-nodisplay">
	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_canreinvite'),'name' => 'protocol[canreinvite]','labelid' => 'protocol-canreinvite','default' => $protocol_elt['sip']['canreinvite']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_context'),'name' => 'protocol[context]','labelid' => 'sip-protocol-context','default' => $protocol_elt['sip']['context']['default'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_context'),'name' => 'protocol[context]','labelid' => 'iax-protocol-context','default' => $protocol_elt['iax']['context']['default'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->slt(array('desc' => $this->bbf('fm_protocol_amaflags'),'name' => 'protocol[amaflags]','labelid' => 'sip-protocol-amaflags','key' => false,'default' => $protocol_elt['sip']['amaflags']['default']),$protocol_elt['sip']['amaflags']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->slt(array('desc' => $this->bbf('fm_protocol_amaflags'),'name' => 'protocol[amaflags]','labelid' => 'iax-protocol-amaflags','key' => false,'default' => $protocol_elt['sip']['amaflags']['default']),$protocol_elt['iax']['amaflags']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_accountcode'),'name' => 'protocol[accountcode]','labelid' => 'protocol-accountcode','size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_nat'),'name' => 'protocol[nat]','labelid' => 'protocol-nat','default' => $protocol_elt['sip']['nat']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_qualify'),'name' => 'protocol[qualify]','labelid' => 'sip-protocol-qualify','default' => $protocol_elt['sip']['qualify']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_qualify'),'name' => 'protocol[qualify]','labelid' => 'iax-protocol-qualify','default' => $protocol_elt['iax']['qualify']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<div id="comment" class="fm-field">
<?=$form->textarea(array('desc' => $this->bbf('fm_userfeatures_comment').'<br />','field' => false,'name' => 'ufeatures[comment]','labelid' => 'ufeatures-comment','cols' => 60,'rows' => 5),null,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

	<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>

</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
