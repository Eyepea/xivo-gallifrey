<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->vars('info');
	$protocol_elt = $this->vars('protocol_elt');
	$ufeatures_elt = $this->vars('ufeatures_elt');
	$group_list = $this->vars('group_list');

	$ringgroup = xivo_bool($info['ufeatures']['ringgroup']);

	$vm_active = $this->varra('info',array('voicemail','commented'));

	if($vm_active !== null):
		$vm_active = xivo_bool($vm_active) === true ? false : true;
	endif;

	if((string) ($host = $this->varra('info',array('protocol','host-static'))) !== ''):
		$host_static = true;
	elseif(($host = $this->varra('info',array('protocol','host'))) === 'dynamic' || (string) $host === ''):
		$host_static = false;
	else:
		$host_static = true;
	endif;
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-group');">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'edit'));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>
<?=$form->hidden(array('name' => 'id','value' => $info['ufeatures']['id']));?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_callerid'),'name' => 'protocol[callerid]','labelid' => 'protocol-callerid','value' => $info['protocol']['callerid'],'size' => 15),'onchange="xivo_eid(\'it-voicemail-fullname\').value = this.tmp == true ? this.value : xivo_eid(\'it-voicemail-fullname\').value;" onfocus="this.tmp = xivo_eid(\'it-voicemail-fullname\').value == this.value ? true : false; this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_name'),'name' => 'protocol[name]','labelid' => 'protocol-name','value' => $info['protocol']['name'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_ufeatures_number'),'name' => 'ufeatures[number]','labelid' => 'ufeatures-number','value' => $info['ufeatures']['number'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_secret'),'name' => 'protocol[secret]','labelid' => 'protocol-secret','value' => $info['protocol']['secret'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_protocol'),'name' => 'protocol[protocol]','labelid' => 'protocol-protocol','key' => true,'value' => $info['ufeatures']['protocol']),$this->vars('protocol'),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" onchange="xivo_chgprotocol(this);"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_popupwidget'),'name' => 'ufeatures[popupwidget]','labelid' => 'ufeatures-popupwidget','default' => $ufeatures_elt['popupwidget']['default'],'checked' => $info['ufeatures']['popupwidget']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_userfeatures_ringseconds'),'name' => 'ufeatures[ringseconds]','labelid' => 'ufeatures-ringseconds','bbf' => array('key','fm_userfeatures_ringseconds-opt'),'key' => false,'default' => $ufeatures_elt['ringseconds']['default'],'value' => (int) $info['ufeatures']['ringseconds']),$ufeatures_elt['ringseconds']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_userfeatures_simultcalls'),'name' => 'ufeatures[simultcalls]','labelid' => 'ufeatures-simultcalls','key' => false,'default' => $ufeatures_elt['simultcalls']['default'],'value' => (int) $info['ufeatures']['simultcalls']),$ufeatures_elt['simultcalls']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_host'),'name' => 'protocol[host-dynamic]','labelid' => 'sip-protocol-host-dynamic','bbf' => 'fm_protocol_host-','key' => false,'value' => ($host_static === true ? 'static' : 'dynamic')),$protocol_elt['sip']['host-dynamic']['value'],'onchange="xivo_chg_attrib(\'fm_host\',\'fd-sip-protocol-host-static\',(this.value == \'dynamic\' ? 1 : 2))" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('name' => 'protocol[host-static]','labelid' => 'sip-protocol-host-static','size' => 15,'value' => ($host_static === true ? $host : '')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_host'),'name' => 'protocol[host-dynamic]','labelid' => 'iax-protocol-host-dynamic','bbf' => 'fm_protocol_host-','key' => false,'value' => ($host_static === true ? 'static' : 'dynamic')),$protocol_elt['iax']['host-dynamic']['value'],'onchange="xivo_chg_attrib(\'fm_host\',\'fd-iax-protocol-host-static\',(this.value == \'dynamic\' ? 1 : 2))" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('name' => 'protocol[host-static]','labelid' => 'iax-protocol-host-static','size' => 15,'value' => ($host_static === true ? $host : '')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_protocol_dtmfmode'),'name' => 'protocol[dtmfmode]','labelid' => 'protocol-dtmfmode','key' => false,'value' => $this->varra('info',array('protocol','dtmfmode'))),$protocol_elt['sip']['dtmfmode']['value'],'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
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
		<?=$form->slt(array('name' => 'group[]','label' => false,'id' => 'it-group','multiple' => true,'size' => 5,'field' => false,'key' => 'name','key_val' => 'id'),$info['protocol']['group'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
			</div>
		</div>
		<div class="clearboth">
		<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_ringgroup'),'name' => 'ufeatures[ringgroup]','labelid' => 'ufeatures-ringgroup','default' => $ufeatures_elt['ringgroup']['default'],'checked' => $ringgroup),(empty($info['protocol']['group']) === true ? 'disabled="disabled" ' : '').'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';" onclick="xivo_eid(\'ringgroup\').style.display = this.checked == true ? \'block\' : \'none\';"');?>

			<div id="ringgroup"<?=($ringgroup !== true ? ' class="b-nodisplay"' : '')?>>
		<?=$form->slt(array('desc' => $this->bbf('fm_usergroup'),'name' => 'usergroup','id' => 'it-usergroup','key' => 'name','key_val' => 'id','value' => $this->varra('info',array('usergroup','groupid'))),$info['protocol']['group'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
			</div>
		</div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add'),'</div>';
	endif;
?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail'),'name' => 'voicemail-active','id' => 'it-voicemail','checked' => $vm_active),'onclick="xivo_eid(\'voicemail\').style.display = this.checked == true ? \'block\' : \'none\';"');?>

<div id="voicemail"<?=($vm_active !== true ? ' class="b-nodisplay"' : '')?>>
	<?=$form->text(array('desc' => $this->bbf('fm_voicemail_fullname'),'name' => 'voicemail[fullname]','labelid' => 'voicemail-fullname','value' => $this->varra('info',array('voicemail','fullname')),'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_voicemail_password'),'name' => 'voicemail[password]','labelid' => 'voicemail-password','value' => $this->varra('info',array('voicemail','password')),'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_voicemail_email'),'name' => 'voicemail[email]','labelid' => 'voicemail-email','value' => $this->varra('info',array('voicemail','email')),'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail_attach'),'name' => 'voicemail[attach]','labelid' => 'voicemail-attach','checked' => $this->varra('info',array('voicemail','attach'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail_delete'),'name' => 'voicemail[delete]','labelid' => 'voicemail-delete','checked' => $this->varra('info',array('voicemail','delete'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<div id="advanced" class="b-nodisplay">
	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_canreinvite'),'name' => 'protocol[canreinvite]','labelid' => 'protocol-canreinvite','default' => $protocol_elt['sip']['canreinvite']['default'],'checked' => $this->varra('info',array('protocol','canreinvite'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_context'),'name' => 'protocol[context]','labelid' => 'sip-protocol-context','default' => $protocol_elt['sip']['context']['default'],'value' => $info['protocol']['context'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_context'),'name' => 'protocol[context]','labelid' => 'iax-protocol-context','default' => $protocol_elt['iax']['context']['default'],'value' => $info['protocol']['context'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->slt(array('desc' => $this->bbf('fm_protocol_amaflags'),'name' => 'protocol[amaflags]','labelid' => 'sip-protocol-amaflags','key' => false,'default' => $protocol_elt['sip']['amaflags']['default'],'value' => $info['protocol']['amaflags']),$protocol_elt['sip']['amaflags']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->slt(array('desc' => $this->bbf('fm_protocol_amaflags'),'name' => 'protocol[amaflags]','labelid' => 'iax-protocol-amaflags','key' => false,'default' => $protocol_elt['sip']['amaflags']['default'],'value' => $info['protocol']['amaflags']),$protocol_elt['iax']['amaflags']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_accountcode'),'name' => 'protocol[accountcode]','labelid' => 'protocol-accountcode','value' => $info['protocol']['accountcode'],'size' => 15),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_nat'),'name' => 'protocol[nat]','labelid' => 'protocol-nat','default' => $protocol_elt['sip']['nat']['default'],'checked' => $this->varra('info',array('protocol','nat'))),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_qualify'),'name' => 'protocol[qualify]','labelid' => 'sip-protocol-qualify','default' => $protocol_elt['sip']['qualify']['default'],'checked' => $info['protocol']['qualify']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_qualify'),'name' => 'protocol[qualify]','labelid' => 'iax-protocol-qualify','default' => $protocol_elt['iax']['qualify']['default'],'checked' => $info['protocol']['qualify']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<div id="comment" class="fm-field">
<?=$form->textarea(array('desc' => $this->bbf('fm_userfeatures_comment').'<br />','field' => false,'name' => 'ufeatures[comment]','labelid' => 'ufeatures-comment','cols' => 60,'rows' => 5),$info['ufeatures']['comment'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

	<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>

</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
