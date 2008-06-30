<?php
	$form = &$this->get_module('form');
	$url = $this->get_module('url');

	$info = $this->get_var('info');
	$element = $this->get_var('element');
	$context_list = $this->get_var('context_list');

	$allow = $info['protocol']['allow'];

	$codec_active = empty($allow) === false;

	$protocol_disable = (bool) $this->get_varra('info',array('protocol','commented'));

	$reg_active = $this->get_varra('info',array('register','commented'));

	if($reg_active !== null):
		$reg_active = xivo_bool($reg_active) === false;
	endif;

	if(($host = (string) $info['protocol']['host']) === ''
	|| in_array($host,$element['protocol']['host']['value'],true) === true):
		$host_static = false;
	else:
		$host_static = true;
	endif;
?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_protocol_name'),'name' => 'protocol[name]','labelid' => 'protocol-name','size' => 15,'default' => $element['protocol']['name']['default'],'value' => $info['protocol']['name']),'onchange="xivo_eid(\'it-protocol-username\').value = xivo_trunk_type == \'friend\' ? this.value : xivo_eid(\'it-protocol-username\').value;" onfocus="xivo_eid(\'it-protocol-username\').value = xivo_trunk_type == \'friend\' ? this.value : xivo_eid(\'it-protocol-username\').value; xivo_fm_set_onfocus(this);"');?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_username'),'name' => 'protocol[username]','labelid' => 'protocol-username','size' => 15,'default' => $element['protocol']['username']['default'],'value' => $info['protocol']['username']));?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_secret'),'name' => 'protocol[secret]','labelid' => 'protocol-secret','size' => 15,'default' => $element['protocol']['secret']['default'],'value' => $info['protocol']['secret']));?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_callerid'),'name' => 'protocol[callerid]','labelid' => 'protocol-callerid','size' => 15,'default' => $element['protocol']['callerid']['default'],'value' => $info['protocol']['callerid']));?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_calllimit'),'name' => 'protocol[call-limit]','labelid' => 'protocol-calllimit','size' => 10,'value' => $info['protocol']['call-limit']));?>

<?=$form->select(array('desc' => $this->bbf('fm_protocol_host'),'name' => 'protocol[host-dynamic]','labelid' => 'protocol-host-dynamic','bbf' => 'fm_protocol_host-','key' => false,'default' => $element['protocol']['host']['default'],'value' => ($host_static === true ? 'static' : $host)),$element['protocol']['host-dynamic']['value'],'onchange="xivo_chg_attrib(\'fm_host\',\'fd-protocol-host-static\',(this.value != \'static\' ? 0 : 1))"');?>

<?=$form->text(array('desc' => '&nbsp;','name' => 'protocol[host-static]','labelid' => 'protocol-host-static','size' => 15,'default' => $element['protocol']['host-static']['default'],'value' => ($host_static === true ? $host : '')));?>

<?=$form->select(array('desc' => $this->bbf('fm_protocol_type'),'name' => 'protocol[type]','labelid' => 'protocol-type','bbf' => 'fm_protocol_type-','key' => false,'default' => $element['protocol']['type']['default'],'value' => $info['protocol']['type']),$element['protocol']['type']['value'],'onchange="xivo_ast_chg_trunk_type(this);"');?>

<?php

if($context_list !== false):
	echo $form->select(array('desc' => $this->bbf('fm_protocol_context'),'name' => 'protocol[context]','labelid' => 'protocol-context','key' => 'identity','altkey' => 'name','empty' => true,'default' => $element['protocol']['context']['default'],'value' => $info['protocol']['context']),$context_list);
endif;

?>

</div>

<div id="sb-part-register" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_register'),'name' => 'register-active','labelid' => 'register-active','checked' => $reg_active,'disabled' => $protocol_disable),'onclick="xivo_chg_attrib(\'fm_register\',\'it-register-username\',(this.checked == true ? 0 : 1))"');?>

<?=$form->text(array('desc' => $this->bbf('fm_register_username'),'name' => 'register[username]','labelid' => 'register-username','size' => 15,'default' => $element['register']['username']['default'],'value' => $this->get_varra('info',array('register','username'))));?>

<?=$form->text(array('desc' => $this->bbf('fm_register_password'),'name' => 'register[password]','labelid' => 'register-password','size' => 15,'default' => $element['register']['password']['default'],'value' => $this->get_varra('info',array('register','password'))));?>

<?=$form->text(array('desc' => $this->bbf('fm_register_host'),'name' => 'register[host]','labelid' => 'register-host','size' => 15,'default' => $element['register']['host']['default'],'value' => $this->get_varra('info',array('register','host'))));?>

<?=$form->text(array('desc' => $this->bbf('fm_register_port'),'name' => 'register[port]','labelid' => 'register-port','size' => 15,'default' => $element['register']['port']['default'],'value' => $this->get_varra('info',array('register','port'))));?>

</div>

<div id="sb-part-codec" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_codec-custom'),'name' => 'codec-active','labelid' => 'codec-active','checked' => $codec_active),'onclick="xivo_chg_attrib(\'fm_codec\',\'it-protocol-disallow\',(this.checked == true ? 0 : 1))"');?>

<?=$form->select(array('desc' => $this->bbf('fm_protocol_codec-disallow'),'name' => 'protocol[disallow]','labelid' => 'protocol-disallow','bbf' => array('concatvalue','fm_protocol_codec-disallow-opt-'),'key' => false,'default' => $element['protocol']['disallow']['default']),$element['protocol']['disallow']['value']);?>

<div id="codeclist" class="fm-field fm-multilist"><p><label id="lb-codeclist" for="it-codeclist"><?=$this->bbf('fm_protocol_codec-allow');?></label></p>
	<div class="slt-outlist">
		<?=$form->select(array('name' => 'codeclist','label' => false,'id' => 'it-codeclist','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_codec_name_type-'),$element['protocol']['allow']['value']);?>
	</div>
	<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-codeclist','it-codec'); return(xivo_free_focus());" title="<?=$this->bbf('bt_incodec');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt_incodec'),'class="bt-inlist" id="bt-incodec" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-codec','it-codeclist'); return(xivo_free_focus());" title="<?=$this->bbf('bt_outcodec');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt_outcodec'),'class="bt-outlist" id="bt-outcodec" border="0"');?></a>
	</div>
	<div class="slt-inlist">

		<?=$form->select(array('name' => 'protocol[allow][]','label' => false,'id' => 'it-codec','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_codec_name_type-'),$allow);?>

		<div class="bt-updown">

			<a href="#" onclick="xivo_fm_order_selected('it-codec',1); return(xivo_free_focus());" title="<?=$this->bbf('bt_upcodec');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt_upcodec'),'class="bt-uplist" id="bt-upcodec" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-codec',-1); return(xivo_free_focus());" title="<?=$this->bbf('bt_downcodec');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt_downcodec'),'class="bt-downlist" id="bt-downcodec" border="0"');?></a>

		</div>

	</div>
</div>
<div class="clearboth"></div>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?=$form->text(array('desc' => $this->bbf('fm_protocol_port'),'name' => 'protocol[port]','labelid' => 'protocol-port','size' => 15,'default' => $element['protocol']['port']['default'],'value' => $info['protocol']['port']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_notransfer'),'name' => 'protocol[notransfer]','labelid' => 'protocol-notransfer','default' => $element['protocol']['notransfer']['default'],'checked' => $info['protocol']['notransfer']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_trunk'),'name' => 'protocol[trunk]','labelid' => 'protocol-trunk','default' => $element['protocol']['trunk']['default'],'checked' => $info['protocol']['trunk']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_protocol_qualify'),'name' => 'protocol[qualify]','labelid' => 'protocol-qualify','default' => $element['protocol']['qualify']['default'],'checked' => $info['protocol']['qualify']));?>

</div>
