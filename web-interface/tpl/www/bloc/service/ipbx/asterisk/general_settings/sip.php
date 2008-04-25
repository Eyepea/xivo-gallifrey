<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');

	$element = $this->get_var('element');
	$moh_list = $this->get_var('moh_list');
	$context_list = $this->get_var('context_list');

	if($this->get_var('fm_save') === true):
		$dhtml = &$this->get_module('dhtml');
		$dhtml->write_js('xivo_form_success(\''.$dhtml->escape($this->bbf('fm_success-save')).'\');');
	endif;
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>

<div class="sb-smenu">
	<ul>
		<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-first');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_general');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-network');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_network');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-signalling');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_signalling');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-4" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-default');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_default');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-5" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-last',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_realtime');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-codec');">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => 1));?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_bindport'),'name' => 'bindport','labelid' => 'bindport','value' => $this->get_varra('info',array('bindport','var_val')),'default' => $element['bindport']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_bindaddr'),'name' => 'bindaddr','labelid' => 'bindaddr','size' => 15,'value' => $this->get_varra('info',array('bindaddr','var_val')),'default' => $element['bindaddr']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_videosupport'),'name' => 'videosupport','labelid' => 'videosupport','checked' => $this->get_varra('info',array('videosupport','var_val')),'default' => $element['videosupport']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_autocreatepeer'),'name' => 'autocreatepeer','labelid' => 'autocreatepeer','checked' => $this->get_varra('info',array('autocreatepeer','var_val')),'default' => $element['autocreatepeer']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_allowguest'),'name' => 'allowguest','labelid' => 'allowguest','key' => false,'bbf' => array('concatvalue','fm_allowguest-opt-'),'value' => $this->get_varra('info',array('allowguest','var_val')),'default' => $element['allowguest']['default']),$element['allowguest']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_promiscredir'),'name' => 'promiscredir','labelid' => 'promiscredir','checked' => $this->get_varra('info',array('promiscredir','var_val')),'default' => $element['promiscredir']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_autodomain'),'name' => 'autodomain','labelid' => 'autodomain','checked' => $this->get_varra('info',array('autodomain','var_val')),'default' => $element['autodomain']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_domain'),'name' => 'domain','labelid' => 'domain','size' => 15,'value' => $this->get_varra('info',array('domain','var_val')),'default' => $element['domain']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_allowexternaldomains'),'name' => 'allowexternaldomains','labelid' => 'allowexternaldomains','checked' => $this->get_varra('info',array('allowexternaldomains','var_val')),'default' => $element['allowexternaldomains']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_usereqphone'),'name' => 'usereqphone','labelid' => 'usereqphone','checked' => $this->get_varra('info',array('usereqphone','var_val')),'default' => $element['usereqphone']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_realm'),'name' => 'realm','labelid' => 'realm','size' => 15,'value' => $this->get_varra('info',array('realm','var_val')),'default' => $element['realm']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_alwaysauthreject'),'name' => 'alwaysauthreject','labelid' => 'alwaysauthreject','checked' => $this->get_varra('info',array('alwaysauthreject','var_val')),'default' => $element['alwaysauthreject']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_useragent'),'name' => 'useragent','labelid' => 'useragent','size' => 15,'value' => $this->get_varra('info',array('useragent','var_val')),'default' => $element['useragent']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_checkmwi'),'name' => 'checkmwi','labelid' => 'checkmwi','key' => false,'bbf' => array('mixkey','fm_checkmwi-opt'),'value' => $this->get_varra('info',array('checkmwi','var_val')),'default' => $element['checkmwi']['default']),$element['checkmwi']['value']);?>

<?php

if($context_list !== false):
	echo $form->select(array('desc' => $this->bbf('fm_regcontext'),'name' => 'regcontext','labelid' => 'regcontext','key' => 'identity','altkey' => 'name','empty' => true,'default' => $element['regcontext']['default'],'value' => $this->get_varra('info',array('regcontext','var_val'))),$context_list);
endif;

?>

<?=$form->text(array('desc' => $this->bbf('fm_callerid'),'name' => 'callerid','labelid' => 'callerid','size' => 15,'value' => $this->get_varra('info',array('callerid','var_val')),'default' => $element['callerid']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_fromdomain'),'name' => 'fromdomain','labelid' => 'fromdomain','size' => 15,'value' => $this->get_varra('info',array('fromdomain','var_val')),'default' => $element['fromdomain']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_sipdebug'),'name' => 'sipdebug','labelid' => 'sipdebug','checked' => $this->get_varra('info',array('sipdebug','var_val')),'default' => $element['sipdebug']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_dumphistory'),'name' => 'dumphistory','labelid' => 'dumphistory','checked' => $this->get_varra('info',array('dumphistory','var_val')),'default' => $element['dumphistory']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_recordhistory'),'name' => 'recordhistory','labelid' => 'recordhistory','checked' => $this->get_varra('info',array('recordhistory','var_val')),'default' => $element['recordhistory']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_callevents'),'name' => 'callevents','labelid' => 'callevents','checked' => $this->get_varra('info',array('callevents','var_val')),'default' => $element['callevents']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_tos'),'name' => 'tos','labelid' => 'tos','key' => false,'value' => $this->get_varra('info',array('tos','var_val')),'default' => $element['tos']['default']),$element['tos']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_ospauth'),'name' => 'ospauth','labelid' => 'ospauth','key' => false,'bbf' => array('concatvalue','fm_ospauth-opt-'),'value' => $this->get_varra('info',array('ospauth','var_val')),'default' => $element['ospauth']['default']),$element['ospauth']['value']);?>

</div>

<div id="sb-part-network" class="b-nodisplay">

<?=$form->text(array('desc' => $this->bbf('fm_localnet'),'name' => 'localnet','labelid' => 'localnet','size' => 15,'value' => $this->get_varra('info',array('localnet','var_val')),'default' => $element['localnet']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_externip'),'name' => 'externip','labelid' => 'externip','size' => 15,'value' => $this->get_varra('info',array('externip','var_val')),'default' => $element['externip']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_externhost'),'name' => 'externhost','labelid' => 'externhost','size' => 15,'value' => $this->get_varra('info',array('externhost','var_val')),'default' => $element['externhost']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_externrefresh'),'name' => 'externrefresh','labelid' => 'externrefresh','bbf' => array('mixkey','fm_externrefresh-opt','paramarray'),'value' => $this->get_varra('info',array('externrefresh','var_val')),'default' => $element['externrefresh']['default']),$element['externrefresh']['value']);?>

<?=$form->text(array('desc' => $this->bbf('fm_outboundproxy'),'name' => 'outboundproxy','labelid' => 'outboundproxy','size' => 15,'value' => $this->get_varra('info',array('outboundproxy','var_val')),'default' => $element['outboundproxy']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_outboundproxyport'),'name' => 'outboundproxyport','labelid' => 'outboundproxyport','value' => $this->get_varra('info',array('outboundproxyport','var_val')),'default' => $element['outboundproxyport']['default']));?>

</div>

<div id="sb-part-signalling" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_relaxdtmf'),'name' => 'relaxdtmf','labelid' => 'relaxdtmf','checked' => $this->get_varra('info',array('relaxdtmf','var_val')),'default' => $element['relaxdtmf']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_compactheaders'),'name' => 'compactheaders','labelid' => 'compactheaders','checked' => $this->get_varra('info',array('compactheaders','var_val')),'default' => $element['compactheaders']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_rtptimeout'),'name' => 'rtptimeout','labelid' => 'rtptimeout','key' => false,'bbf' => array('mixkey','fm_rtptimeout-opt'),'value' => $this->get_varra('info',array('rtptimeout','var_val')),'default' => $element['rtptimeout']['default']),$element['rtptimeout']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_rtpholdtimeout'),'name' => 'rtpholdtimeout','labelid' => 'rtpholdtimeout','key' => false,'bbf' => array('mixkey','fm_rtptimeout-opt'),'value' => $this->get_varra('info',array('rtpholdtimeout','var_val')),'default' => $element['rtpholdtimeout']['default']),$element['rtpholdtimeout']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_rtpkeepalive'),'name' => 'rtpkeepalive','labelid' => 'rtpkeepalive','key' => false,'bbf' => array('mixkey','fm_rtpkeepalive-opt'),'value' => $this->get_varra('info',array('rtpkeepalive','var_val')),'default' => $element['rtpkeepalive']['default']),$element['rtpkeepalive']['value']);?>

<?=$form->text(array('desc' => $this->bbf('fm_notifymimetype'),'name' => 'notifymimetype','labelid' => 'notifymimetype','size' => 15,'value' => $this->get_varra('info',array('notifymimetype','var_val')),'default' => $element['notifymimetype']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_srvlookup'),'name' => 'srvlookup','labelid' => 'srvlookup','checked' => $this->get_varra('info',array('srvlookup','var_val')),'default' => $element['srvlookup']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_pedantic'),'name' => 'pedantic','labelid' => 'pedantic','checked' => $this->get_varra('info',array('pedantic','var_val')),'default' => $element['pedantic']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_maxexpiry'),'name' => 'maxexpiry','labelid' => 'maxexpiry','bbf' => array('mixkey','fm_maxexpiry-opt','paramarray'),'value' => $this->get_varra('info',array('maxexpiry','var_val')),'default' => $element['maxexpiry']['default']),$element['maxexpiry']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_defaultexpiry'),'name' => 'defaultexpiry','labelid' => 'defaultexpiry','bbf' => array('mixkey','fm_defaultexpiry-opt','paramarray'),'value' => $this->get_varra('info',array('defaultexpiry','var_val')),'default' => $element['defaultexpiry']['default']),$element['defaultexpiry']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_registertimeout'),'name' => 'registertimeout','labelid' => 'registertimeout','bbf' => array('mixkey','fm_registertimeout-opt','paramarray'),'value' => $this->get_varra('info',array('registertimeout','var_val')),'default' => $element['registertimeout']['default']),$element['registertimeout']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_registerattempts'),'name' => 'registerattempts','labelid' => 'registerattempts','bbf' => array('mixkey','fm_registerattempts-opt','paramarray'),'value' => $this->get_varra('info',array('registerattempts','var_val')),'default' => $element['registerattempts']['default']),$element['registerattempts']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_notifyringing'),'name' => 'notifyringing','labelid' => 'notifyringing','checked' => $this->get_varra('info',array('notifyringing','var_val')),'default' => $element['notifyringing']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_codec-disallow'),'name' => 'disallow','labelid' => 'disallow','key' => false,'bbf' => array('concatvalue','fm_codec-disallow-opt-')),$element['disallow']['value']);?>

<div id="codeclist" class="fm-field fm-multilist"><p><label id="lb-codeclist" for="it-codeclist"><?=$this->bbf('fm_codec-allow');?></label></p>
	<div class="slt-outlist">
		<?=$form->select(array('name' => 'codeclist','label' => false,'id' => 'it-codeclist','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_codec_name_type-'),$element['allow']['value']);?>
	</div>
	<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-codeclist','it-codec'); return(false);" title="<?=$this->bbf('bt_incodec');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt_incodec'),'class="bt-inlist" id="bt-incodec" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-codec','it-codeclist'); return(false);" title="<?=$this->bbf('bt_outcodec');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt_outcodec'),'class="bt-outlist" id="bt-outcodec" border="0"');?></a>
	</div>
	<div class="slt-inlist">

		<?=$form->select(array('name' => 'allow[]','label' => false,'id' => 'it-codec','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_codec_name_type-'),$this->get_varra('info',array('allow','var_val')));?>

		<div class="bt-updown">

			<a href="#" onclick="xivo_fm_order_selected('it-codec',1); return(false);" title="<?=$this->bbf('bt_upcodec');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt_upcodec'),'class="bt-uplist" id="bt-upcodec" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-codec',-1); return(false);" title="<?=$this->bbf('bt_downcodec');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt_downcodec'),'class="bt-downlist" id="bt-downcodec" border="0"');?></a>

		</div>

	</div>
</div>
<div class="clearboth"></div>

</div>

<div id="sb-part-default" class="b-nodisplay">

<?php

if($context_list !== false):
	echo $form->select(array('desc' => $this->bbf('fm_context'),'name' => 'context','labelid' => 'context','key' => 'identity','altkey' => 'name','empty' => true,'default' => $element['context']['default'],'value' => $this->get_varra('info',array('context','var_val'))),$context_list);
endif;

?>

<?=$form->select(array('desc' => $this->bbf('fm_nat'),'name' => 'nat','labelid' => 'nat','key' => false,'bbf' => array('concatvalue','fm_nat-opt-'),'value' => $this->get_varra('info',array('nat','var_val')),'default' => $element['nat']['default']),$element['nat']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_dtmfmode'),'name' => 'dtmfmode','labelid' => 'dtmfmode','key' => false,'value' => $this->get_varra('info',array('dtmfmode','var_val')),'default' => $element['dtmfmode']['default']),$element['dtmfmode']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_qualify'),'name' => 'qualify','labelid' => 'qualify','key' => false,'bbf' => array('mixkey','fm_qualify-opt'),'value' => $this->get_varra('info',array('qualify','var_val')),'default' => $element['qualify']['default']),$element['qualify']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_useclientcode'),'name' => 'useclientcode','labelid' => 'useclientcode','checked' => $this->get_varra('info',array('useclientcode','var_val')),'default' => $element['useclientcode']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_progressinband'),'name' => 'progressinband','labelid' => 'progressinband','key' => false,'bbf' => array('concatvalue','fm_progressinband-opt-'),'value' => $this->get_varra('info',array('progressinband','var_val')),'default' => $element['progressinband']['default']),$element['progressinband']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_language'),'name' => 'language','labelid' => 'language','key' => false,'value' => $this->get_varra('info',array('language','var_val')),'default' => $element['language']['default']),$element['language']['value']);?>

<?php

if($moh_list !== false):
	echo $form->select(array('desc' => $this->bbf('fm_musiconhold'),'name' => 'musiconhold','labelid' => 'musiconhold','key' => 'category','value' => $this->get_varra('info',array('musiconhold','var_val')),'default' => $element['musiconhold']['default']),$moh_list);
endif;

?>

<?=$form->text(array('desc' => $this->bbf('fm_vmexten'),'name' => 'vmexten','labelid' => 'vmexten','value' => $this->get_varra('info',array('vmexten','var_val')),'default' => $element['vmexten']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trustrpid'),'name' => 'trustrpid','labelid' => 'trustrpid','checked' => $this->get_varra('info',array('trustrpid','var_val')),'default' => $element['trustrpid']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_sendrpid'),'name' => 'sendrpid','labelid' => 'sendrpid','checked' => $this->get_varra('info',array('sendrpid','var_val')),'default' => $element['sendrpid']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_canreinvite'),'name' => 'canreinvite','labelid' => 'canreinvite','key' => false,'bbf' => array('concatvalue','fm_canreinvite-opt-'),'value' => $this->get_varra('info',array('canreinvite','var_val')),'default' => $element['canreinvite']['default']),$element['canreinvite']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_insecure'),'name' => 'insecure','labelid' => 'insecure','empty' => true,'bbf' => array('concatvalue','fm_insecure-opt-'),'value' => $this->get_varra('info',array('insecure','var_val')),'default' => $element['insecure']['default']),$element['insecure']['value']);?>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_rtcachefriends'),'name' => 'rtcachefriends','labelid' => 'rtcachefriends','checked' => $this->get_varra('info',array('rtcachefriends','var_val')),'default' => $element['rtcachefriends']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_rtupdate'),'name' => 'rtupdate','labelid' => 'rtupdate','checked' => $this->get_varra('info',array('rtupdate','var_val')),'default' => $element['rtupdate']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_ignoreregexpire'),'name' => 'ignoreregexpire','labelid' => 'ignoreregexpire','checked' => $this->get_varra('info',array('ignoreregexpire','var_val')),'default' => $element['ignoreregexpire']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_rtautoclear'),'name' => 'rtautoclear','labelid' => 'rtautoclear','bbf' => array('mixkey','fm_rtautoclear-opt','paramarray'),'value' => $this->get_varra('info',array('rtautoclear','var_val')),'default' => $element['rtautoclear']['default']),$element['rtautoclear']['value']);?>

</div>

		<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
