<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->get_var('info');
	$element = $this->get_var('element');

	$moh_list = $this->get_var('moh_list');
	$autoprov_list = $this->get_var('autoprov_list');
	$rightcall = $this->get_var('rightcall');

	if(($vm_active = $info['voicemail']['commented']) !== null):
		$vm_active = xivo_bool($vm_active) === true ? false : true;
	endif;

	if(isset($info['protocol']['allow']) === true):
		$allow = $info['protocol']['allow'];
	else:
		$allow = array();
	endif;

	if(empty($allow) === true):
		$codec_active = false;
	else:
		$codec_active = true;
	endif;

	if(isset($info['protocol']) === true):
		$host = (string) xivo_ak('host',$info['protocol'],true);
	else:
		$host = '';
	endif;

	if($host === '' || in_array($host,$element['protocol']['sip']['host']['value'],true) === true):
		$sip_host_static = false;
	else:
		$sip_host_static = true;
	endif;

	if($host === '' || in_array($host,$element['protocol']['iax']['host']['value'],true) === true):
		$iax_host_static = false;
	else:
		$iax_host_static = true;
	endif;

	if(($outcallerid = (string) $info['ufeatures']['outcallerid']) === ''
	|| in_array($outcallerid,$element['ufeatures']['outcallerid']['value'],true) === true):
		$outcallerid_custom = false;
	else:
		$outcallerid_custom = true;
	endif;

	if(empty($info['autoprov']) === true || $info['autoprov']['vendor'] === ''):
		$vendormodel = '';
	else:
		$vendormodel = $info['autoprov']['vendor'].'.'.$info['autoprov']['model'];
	endif;

	if(isset($info['protocol']) === true):
		$context = (string) xivo_ak('context',$info['protocol'],true);
		$amaflags = (string) xivo_ak('amaflags',$info['protocol'],true);
		$qualify = (string) xivo_ak('qualify',$info['protocol'],true);
	else:
		$context = $amaflags = $qualify = '';
	endif;
?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_userfeatures_firstname'),'name' => 'ufeatures[firstname]','labelid' => 'ufeatures-firstname','value' => $info['ufeatures']['firstname'],'size' => 15),'onchange="xivo_chgname();" onfocus="xivo_cpyname(); xivo_fm_set_onfocus(this);" onblur="xivo_chgname(); xivo_fm_set_onblur(this);"');?>

<?=$form->text(array('desc' => $this->bbf('fm_userfeatures_lastname'),'name' => 'ufeatures[lastname]','labelid' => 'ufeatures-lastname','value' => $info['ufeatures']['lastname'],'size' => 15),'onchange="xivo_chgname();" onfocus="xivo_cpyname(); xivo_fm_set_onfocus(this);" onblur="xivo_chgname(); xivo_fm_set_onblur(this);"');?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_name'),'name' => 'protocol[name]','labelid' => 'protocol-name','value' => $info['protocol']['name'],'size' => 15));?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_interface'),'name' => 'protocol[interface]','labelid' => 'protocol-interface','default' => $element['protocol']['custom']['interface']['default'],'value' => $this->get_varra('info',array('protocol','interface')),'size' => 15));?>

<?=$form->text(array('desc' => $this->bbf('fm_ufeatures_number'),'name' => 'ufeatures[number]','labelid' => 'ufeatures-number','value' => $info['ufeatures']['number'],'size' => 15),'onchange="xivo_chgname();" onfocus="xivo_cpyname(); xivo_fm_set_onfocus(this);"');?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_secret'),'name' => 'protocol[secret]','labelid' => 'protocol-secret','value' => $this->get_varra('info',array('protocol','secret')),'size' => 15));?>

<?=$form->select(array('desc' => $this->bbf('fm_userfeatures_ringseconds'),'name' => 'ufeatures[ringseconds]','labelid' => 'ufeatures-ringseconds','bbf' => array('mixkey','fm_userfeatures_ringseconds-opt'),'key' => false,'default' => $element['ufeatures']['ringseconds']['default'],'value' => (isset($info['ufeatures']['ringseconds']) === true ? (int) $info['ufeatures']['ringseconds'] : null)),$element['ufeatures']['ringseconds']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_userfeatures_simultcalls'),'name' => 'ufeatures[simultcalls]','labelid' => 'ufeatures-simultcalls','key' => false,'default' => $element['ufeatures']['simultcalls']['default'],'value' => (isset($info['ufeatures']['simultcalls']) === true ? (int) $info['ufeatures']['simultcalls'] : null)),$element['ufeatures']['simultcalls']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_protocol_protocol'),'name' => 'protocol[protocol]','labelid' => 'protocol-protocol','bbf' => array('concatkey','fm_protocol_protocol-opt-'),'key' => false,'default' => $element['ufeatures']['protocol']['default'],'value' => $info['ufeatures']['protocol']),$element['ufeatures']['protocol']['value'],'onchange="xivo_chg_protocol(this.value);"');?>

</div>

<div id="sb-part-group" class="b-nodisplay">
	<?=$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/groups');?>
</div>

<div id="sb-part-codec" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_codec-custom'),'name' => 'codec-active','labelid' => 'codec-active','checked' => $codec_active),'onclick="xivo_chg_attrib(\'fm_codec\',\'it-\'+xivo_protocol+\'-protocol-disallow\',(this.checked == true ? 0 : 1))"');?>

<?=$form->select(array('desc' => $this->bbf('fm_protocol_codec-disallow'),'name' => 'protocol[disallow]','labelid' => 'sip-protocol-disallow','key' => false,'bbf' => array('concatvalue','fm_protocol_codec-disallow-opt-')),$element['protocol']['sip']['disallow']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_protocol_codec-disallow'),'name' => 'protocol[disallow]','labelid' => 'iax-protocol-disallow','key' => false,'bbf' => array('concatvalue','fm_protocol_codec-disallow-opt-')),$element['protocol']['iax']['disallow']['value']);?>

<div id="codeclist" class="fm-field fm-multilist">
	<p>
		<label id="lb-codeclist" for="it-codeclist" onclick="xivo_eid('it-'+xivo_protocol+'-codeclist').focus();">
			<?=$this->bbf('fm_protocol_codec-allow');?>
		</label>
	</p>
	<div class="slt-outlist">
		<?=$form->select(array('name' => 'codeclist','label' => false,'id' => 'it-sip-codeclist','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_codec_name_type-'),$element['protocol']['sip']['allow']['value']);?>
		<?=$form->select(array('name' => 'codeclist','label' => false,'id' => 'it-iax-codeclist','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_codec_name_type-'),$element['protocol']['iax']['allow']['value']);?>
	</div>
	<div class="inout-list">
		<a href="#" onclick="xivo_fm_move_selected('it-'+xivo_protocol+'-codeclist','it-'+xivo_protocol+'-codec'); return(false);" title="<?=$this->bbf('bt-incodec');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-incodec'),'class="bt-inlist" id="bt-incodec" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-'+xivo_protocol+'-codec','it-'+xivo_protocol+'-codeclist'); return(false);" title="<?=$this->bbf('bt-outcodec');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outcodec'),'class="bt-outlist" id="bt-outcodec" border="0"');?></a>
	</div>
	<div class="slt-inlist">

		<?=$form->select(array('name' => 'protocol[allow][]','label' => false,'id' => 'it-sip-codec','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_codec_name_type-'),$allow);?>

		<?=$form->select(array('name' => 'protocol[allow][]','label' => false,'id' => 'it-iax-codec','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_codec_name_type-'),$allow);?>

		<div class="bt-updown">

			<a href="#" onclick="xivo_fm_order_selected('it-'+xivo_protocol+'-codec',1); return(false);" title="<?=$this->bbf('bt-upcodec');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt-upcodec'),'class="bt-uplist" id="bt-upcodec" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-'+xivo_protocol+'-codec',-1); return(false);" title="<?=$this->bbf('bt-downcodec');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt-downcodec'),'class="bt-downlist" id="bt-downcodec" border="0"');?></a>

		</div>

	</div>
</div>
<div class="clearboth"></div>

</div>

<div id="sb-part-voicemail" class="b-nodisplay">

	<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-active'),'name' => 'voicemail-active','labelid' => 'voicemail-active','checked' => $vm_active),'onclick="xivo_enable_voicemail();"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_voicemail_fullname'),'name' => 'voicemail[fullname]','labelid' => 'voicemail-fullname','value' => $info['voicemail']['fullname'],'size' => 15));?>

	<?=$form->text(array('desc' => $this->bbf('fm_voicemail_password'),'name' => 'voicemail[password]','labelid' => 'voicemail-password','value' => $info['voicemail']['password'],'size' => 15));?>

	<?=$form->text(array('desc' => $this->bbf('fm_voicemail_email'),'name' => 'voicemail[email]','labelid' => 'voicemail-email','value' => $info['voicemail']['email'],'size' => 15));?>

<?php
	if(($zmsg = $this->get_var('zonemessages')) !== false):

		echo $form->select(array('desc' => $this->bbf('fm_voicemail_tz'),'name' => 'voicemail[tz]','labelid' => 'voicemail-tz','key' => 'name','default' => $element['voicemail']['tz']['default'],'value' => $info['voicemail']['tz']),$zmsg);

	endif;
?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_skipvoicemailpass'),'name' => 'ufeatures[skipvoicemailpass]','labelid' => 'ufeatures-skipvoicemailpass','default' => $element['ufeatures']['skipvoicemailpass']['default'],'checked' => $info['ufeatures']['skipvoicemailpass']));?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail_attach'),'name' => 'voicemail[attach]','labelid' => 'voicemail-attach','checked' => $info['voicemail']['attach']));?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail_deletevoicemail'),'name' => 'voicemail[deletevoicemail]','labelid' => 'voicemail-deletevoicemail','checked' => $info['voicemail']['deletevoicemail']));?>

</div>

<div id="sb-part-service" class="b-nodisplay">

	<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_enableclient'),'name' => 'ufeatures[enableclient]','labelid' => 'ufeatures-enableclient','default' => $element['ufeatures']['enableclient']['default'],'checked' => $info['ufeatures']['enableclient']));?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_enablehint'),'name' => 'ufeatures[enablehint]','labelid' => 'ufeatures-enablehint','default' => $element['ufeatures']['enablehint']['default'],'checked' => $info['ufeatures']['enablehint']));?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_enablevoicemail'),'name' => 'ufeatures[enablevoicemail]','labelid' => 'ufeatures-enablevoicemail','default' => $element['ufeatures']['enablevoicemail']['default'],'checked' => $info['ufeatures']['enablevoicemail']));?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_enablexfer'),'name' => 'ufeatures[enablexfer]','labelid' => 'ufeatures-enablexfer','default' => $element['ufeatures']['enablexfer']['default'],'checked' => $info['ufeatures']['enablexfer']));?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_enableautomon'),'name' => 'ufeatures[enableautomon]','labelid' => 'ufeatures-enableautomon','default' => $element['ufeatures']['enableautomon']['default'],'checked' => $info['ufeatures']['enableautomon']));?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_callrecord'),'name' => 'ufeatures[callrecord]','labelid' => 'ufeatures-callrecord','default' => $element['ufeatures']['callrecord']['default'],'checked' => $info['ufeatures']['callrecord']));?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_callfilter'),'name' => 'ufeatures[callfilter]','labelid' => 'ufeatures-callfilter','default' => $element['ufeatures']['callfilter']['default'],'checked' => $info['ufeatures']['callfilter']));?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_enablednd'),'name' => 'ufeatures[enablednd]','labelid' => 'ufeatures-enablednd','default' => $element['ufeatures']['enablednd']['default'],'checked' => $info['ufeatures']['enablednd']));?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_enablerna'),'name' => 'ufeatures[enablerna]','labelid' => 'ufeatures-enablerna','default' => $element['ufeatures']['enablerna']['default'],'checked' => $info['ufeatures']['enablerna']),'onchange="xivo_chg_attrib(\'fm_enablerna\',\'it-ufeatures-destrna\',(this.checked == false ? 0 : 1))"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_userfeatures_destrna'),'name' => 'ufeatures[destrna]','labelid' => 'ufeatures-destrna','value' => $info['ufeatures']['destrna'],'size' => 15));?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_enablebusy'),'name' => 'ufeatures[enablebusy]','labelid' => 'ufeatures-enablebusy','default' => $element['ufeatures']['enablebusy']['default'],'checked' => $info['ufeatures']['enablebusy']),'onchange="xivo_chg_attrib(\'fm_enablebusy\',\'it-ufeatures-destbusy\',(this.checked == false ? 0 : 1))"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_userfeatures_destbusy'),'name' => 'ufeatures[destbusy]','labelid' => 'ufeatures-destbusy','value' => $info['ufeatures']['destbusy'],'size' => 15));?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_userfeatures_enableunc'),'name' => 'ufeatures[enableunc]','labelid' => 'ufeatures-enableunc','default' => $element['ufeatures']['enableunc']['default'],'checked' => $info['ufeatures']['enableunc']),'onchange="xivo_chg_attrib(\'fm_enableunc\',\'it-ufeatures-destunc\',(this.checked == false ? 0 : 1))"');?>

	<?=$form->text(array('desc' => $this->bbf('fm_userfeatures_destunc'),'name' => 'ufeatures[destunc]','labelid' => 'ufeatures-destunc','value' => $info['ufeatures']['destunc'],'size' => 15));?>

	<?=$form->select(array('desc' => $this->bbf('fm_userfeatures_bsfilter'),'name' => 'ufeatures[bsfilter]','labelid' => 'ufeatures-bsfilter','bbf' => array('concatkey','fm_userfeatures_bsfilter-opt-'),'key' => false,'default' => $element['ufeatures']['bsfilter']['default'],'value' => $info['ufeatures']['bsfilter']),$element['ufeatures']['bsfilter']['value']);?>

</div>

<div id="sb-part-autoprov" class="b-nodisplay">

<?php
	if($this->get_var('act') === 'edit'):

	echo $form->select(array('desc' => $this->bbf('fm_autoprov_modact'),'name' => 'autoprov[modact]','labelid' => 'autoprov-modact','bbf' => 'fm_autoprov_modact-','key' => false,'empty' => true),$element['autoprov']['modact']['value'],'onchange="xivo_chg_attrib(\'fm_autoprov-\'+xivo_protocol,\'it-autoprov-modact\',(this.value != \'\' ? 0 : 1));"');

	endif;

	if(is_array($info['autoprov']) === false
	|| $vendormodel === ''
	|| (int) xivo_ak('iduserfeatures',$info['autoprov'],true) === 0):
?>

	<?=$form->select(array('desc' => $this->bbf('fm_autoprov_vendormodel'),'name' => 'autoprov[vendormodel]','labelid' => 'autoprov-vendormodel','optgroup' => array('key' => 'name'),'empty' => true,'key' => 'label','altkey' => 'path','value' => $vendormodel),$autoprov_list);?>

	<?=$form->text(array('desc' => $this->bbf('fm_autoprov_macaddr'),'name' => 'autoprov[macaddr]','labelid' => 'autoprov-macaddr','value' => $info['autoprov']['macaddr'],'size' => 15));?>

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

	echo $this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey');
?>
</div>

<div id="sb-part-dialstatus" class="b-nodisplay">

	<fieldset id="fld-dialstatus-noanswer">
		<legend><?=$this->bbf('fld-dialstatus-noanswer');?></legend>
		<?=$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/dialstatus',array('status' => 'noanswer'));?>
	</fieldset>

	<fieldset id="fld-dialstatus-busy">
		<legend><?=$this->bbf('fld-dialstatus-busy');?></legend>
		<?=$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/dialstatus',array('status' => 'busy'));?>
	</fieldset>

	<fieldset id="fld-dialstatus-congestion">
		<legend><?=$this->bbf('fld-dialstatus-congestion');?></legend>
		<?=$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/dialstatus',array('status' => 'congestion'));?>
	</fieldset>

	<fieldset id="fld-dialstatus-chanunavail">
		<legend><?=$this->bbf('fld-dialstatus-chanunavail');?></legend>
		<?=$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/dialstatus',array('status' => 'chanunavail'));?>
	</fieldset>

</div>

<div id="sb-part-rightcall" class="b-nodisplay">

<?php
	if($rightcall['list'] !== false):
?>
		<div id="rightcalllist" class="fm-field fm-multilist">
			<div class="slt-outlist">

		<?=$form->select(array('name' => 'rightcalllist','label' => false,'id' => 'it-rightcalllist','browse' => 'rightcall','key' => 'name','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$rightcall['list']);?>

			</div>
			<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-rightcalllist','it-rightcall'); return(false);" title="<?=$this->bbf('bt-inrightcall');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-inrightcall'),'class="bt-inlist" id="bt-inrightcall" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-rightcall','it-rightcalllist'); return(false);" title="<?=$this->bbf('bt-outrightcall');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outrightcall'),'class="bt-outlist" id="bt-outrightcall" border="0"');?></a>

			</div>
			<div class="slt-inlist">

		<?=$form->select(array('name' => 'rightcall[]','label' => false,'id' => 'it-rightcall','browse' => 'rightcall','key' => 'name','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$rightcall['slt']);?>

			</div>
		</div>
		<div class="clearboth"></div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_rightcall'),'service/ipbx/call_management/rightcall','act=add'),'</div>';
	endif;
?>

</div>

<div id="sb-part-last" class="b-nodisplay">

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_callerid'),'name' => 'protocol[callerid]','labelid' => 'protocol-callerid','value' => $this->get_varra('info',array('protocol','callerid')),'size' => 15,'notag' => false));?>

	<?=$form->select(array('desc' => $this->bbf('fm_ufeatures_outcallerid'),'name' => 'ufeatures[outcallerid-type]','labelid' => 'ufeatures-outcallerid-type','bbf' => 'fm_ufeatures_outcallerid-opt-','key' => false,'value' => ($outcallerid_custom === true ? 'custom' : $outcallerid)),$element['ufeatures']['outcallerid-type']['value'],'onchange="xivo_chg_attrib(\'fm_outcallerid\',\'fd-ufeatures-outcallerid-custom\',(this.value != \'custom\' ? 0 : 1))"');?>

	<?=$form->text(array('desc' => '&nbsp;','name' => 'ufeatures[outcallerid-custom]','labelid' => 'ufeatures-outcallerid-custom','value' => ($outcallerid_custom === true ? $outcallerid : ''),'size' => 15));?>

<?php
	if($moh_list !== false):
		echo $form->select(array('desc' => $this->bbf('fm_userfeatures_musiconhold'),'name' => 'ufeatures[musiconhold]','labelid' => 'ufeatures-musiconhold','key' => 'category','default' => $element['ufeatures']['musiconhold']['default'],'value' => $info['ufeatures']['musiconhold']),$moh_list);
	endif;
?>

	<?=$form->select(array('desc' => $this->bbf('fm_protocol_host'),'name' => 'protocol[host-dynamic]','labelid' => 'sip-protocol-host-dynamic','bbf' => 'fm_protocol_host-','key' => false,'value' => ($sip_host_static === true ? 'static' : $host)),$element['protocol']['sip']['host-dynamic']['value'],'onchange="xivo_chg_attrib(\'fm_host\',\'fd-sip-protocol-host-static\',(this.value != \'static\' ? 0 : 1))"');?>

	<?=$form->text(array('desc' => '&nbsp;','name' => 'protocol[host-static]','labelid' => 'sip-protocol-host-static','size' => 15,'value' => ($sip_host_static === true ? $host : '')));?>

	<?=$form->select(array('desc' => $this->bbf('fm_protocol_host'),'name' => 'protocol[host-dynamic]','labelid' => 'iax-protocol-host-dynamic','bbf' => 'fm_protocol_host-','key' => false,'value' => ($iax_host_static === true ? 'static' : $host)),$element['protocol']['iax']['host-dynamic']['value'],'onchange="xivo_chg_attrib(\'fm_host\',\'fd-iax-protocol-host-static\',(this.value != \'static\' ? 0 : 1))"');?>

	<?=$form->text(array('desc' => '&nbsp;','name' => 'protocol[host-static]','labelid' => 'iax-protocol-host-static','size' => 15,'value' => ($iax_host_static === true ? $host : '')));?>

	<?=$form->select(array('desc' => $this->bbf('fm_protocol_dtmfmode'),'name' => 'protocol[dtmfmode]','labelid' => 'protocol-dtmfmode','key' => false,'default' => $element['protocol']['sip']['dtmfmode']['default'],'value' => $this->get_varra('info',array('protocol','dtmfmode'))),$element['protocol']['sip']['dtmfmode']['value']);?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_canreinvite'),'name' => 'protocol[canreinvite]','labelid' => 'protocol-canreinvite','default' => $element['protocol']['sip']['canreinvite']['default'],'checked' => $this->get_varra('info',array('protocol','canreinvite'))));?>

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_context'),'name' => 'protocol[context]','labelid' => 'sip-protocol-context','default' => $element['protocol']['sip']['context']['default'],'value' => $context,'size' => 15));?>

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_context'),'name' => 'protocol[context]','labelid' => 'iax-protocol-context','default' => $element['protocol']['iax']['context']['default'],'value' => $context,'size' => 15));?>

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_context'),'name' => 'protocol[context]','labelid' => 'custom-protocol-context','default' => $element['protocol']['custom']['context']['default'],'value' => $context,'size' => 15));?>

	<?=$form->select(array('desc' => $this->bbf('fm_protocol_amaflags'),'name' => 'protocol[amaflags]','labelid' => 'sip-protocol-amaflags','bbf' => 'fm_protocol_amaflags-opt-','key' => false,'default' => $element['protocol']['sip']['amaflags']['default'],'value' => $amaflags),$element['protocol']['sip']['amaflags']['value']);?>

	<?=$form->select(array('desc' => $this->bbf('fm_protocol_amaflags'),'name' => 'protocol[amaflags]','labelid' => 'iax-protocol-amaflags','bbf' => 'fm_protocol_amaflags-opt-','key' => false,'default' => $element['protocol']['sip']['amaflags']['default'],'value' => $amaflags),$element['protocol']['iax']['amaflags']['value']);?>

	<?=$form->text(array('desc' => $this->bbf('fm_protocol_accountcode'),'name' => 'protocol[accountcode]','labelid' => 'protocol-accountcode','value' => $this->get_varra('info',array('protocol','accountcode')),'size' => 15));?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_nat'),'name' => 'protocol[nat]','labelid' => 'protocol-nat','default' => $element['protocol']['sip']['nat']['default'],'checked' => $this->get_varra('info',array('protocol','nat'))));?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_qualify'),'name' => 'protocol[qualify]','labelid' => 'sip-protocol-qualify','default' => $element['protocol']['sip']['qualify']['default'],'checked' => $qualify));?>

	<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_qualify'),'name' => 'protocol[qualify]','labelid' => 'iax-protocol-qualify','default' => $element['protocol']['iax']['qualify']['default'],'checked' => $qualify));?>

<div class="fm-field fm-description"><p><label id="lb-ufeatures-description" for="it-ufeatures-description"><?=$this->bbf('fm_userfeatures_description');?></label></p>
<?=$form->textarea(array('field' => false,'label' => false,'name' => 'ufeatures[description]','id' => 'it-ufeatures-description','cols' => 60,'rows' => 5,'default' => $element['ufeatures']['description']['default']),$info['ufeatures']['description']);?>
</div>

</div>
