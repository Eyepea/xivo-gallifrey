<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$element = $this->vars('element');
	$moh_list = $this->vars('moh_list');

	if($this->vars('fm_save') === true):
		$dhtml = &$this->get_module('dhtml');
		$dhtml->write_js('xivo_form_success(\''.xivo_stript($this->bbf('fm_success-save')).'\');');
	endif;

	if(($ntos = xivo_uint($this->varra('info','tos'))) !== 0):
		$tos = $ntos;
	else:
		$tos = $this->varra('info','tos');
	endif;

	if(($nqualify = xivo_uint($this->varra('info','qualify'))) !== 0):
		$qualify = $nqualify;
	else:
		$qualify = $this->varra('info','qualify');
	endif;

	if(($nrtautoclear = xivo_uint($this->varra('info','rtautoclear'))) !== 0):
		$rtautoclear = $nrtautoclear;
	else:
		$rtautoclear = $this->varra('info','rtautoclear');
	endif;
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>

<div class="sb-smenu">
	<ul>
		<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-general');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-general'); return(false);"><?=$this->bbf('smenu_general');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-network');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-network'); return(false);"><?=$this->bbf('smenu_network');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-signalling');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-signalling'); return(false);"><?=$this->bbf('smenu_signalling');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-4" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-default');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-default'); return(false);"><?=$this->bbf('smenu_default');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-5" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-realtime',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-realtime'); return(false);"><?=$this->bbf('smenu_realtime');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-codec');">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>

<div id="sb-part-general">

<?=$form->text(array('desc' => $this->bbf('fm_bindport'),'name' => 'bindport','labelid' => 'bindport','value' => $this->varra('info','bindport'),'default' => $element['bindport']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_bindaddr'),'name' => 'bindaddr','labelid' => 'bindaddr','size' => 15,'value' => $this->varra('info','bindaddr'),'default' => $element['bindaddr']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_videosupport'),'name' => 'videosupport','labelid' => 'videosupport','checked' => $this->varra('info','videosupport'),'default' => $element['videosupport']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_autocreatepeer'),'name' => 'autocreatepeer','labelid' => 'autocreatepeer','checked' => $this->varra('info','autocreatepeer'),'default' => $element['autocreatepeer']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_allowguest'),'name' => 'allowguest','labelid' => 'allowguest','key' => false,'bbf' => array('concatvalue','fm_allowguest-opt-'),'value' => $this->varra('info','allowguest'),'default' => $element['allowguest']['default']),$element['allowguest']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_promiscredir'),'name' => 'promiscredir','labelid' => 'promiscredir','checked' => $this->varra('info','promiscredir'),'default' => $element['promiscredir']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_autodomain'),'name' => 'autodomain','labelid' => 'autodomain','checked' => $this->varra('info','autodomain'),'default' => $element['autodomain']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_domain'),'name' => 'domain','labelid' => 'domain','size' => 15,'value' => $this->varra('info','domain'),'default' => $element['domain']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_allowexternaldomains'),'name' => 'allowexternaldomains','labelid' => 'allowexternaldomains','checked' => $this->varra('info','allowexternaldomains'),'default' => $element['allowexternaldomains']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_usereqphone'),'name' => 'usereqphone','labelid' => 'usereqphone','checked' => $this->varra('info','usereqphone'),'default' => $element['usereqphone']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_realm'),'name' => 'realm','labelid' => 'realm','size' => 15,'value' => $this->varra('info','realm'),'default' => $element['realm']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_alwaysauthreject'),'name' => 'alwaysauthreject','labelid' => 'alwaysauthreject','checked' => $this->varra('info','alwaysauthreject'),'default' => $element['alwaysauthreject']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_useragent'),'name' => 'useragent','labelid' => 'useragent','size' => 15,'value' => $this->varra('info','useragent'),'default' => $element['useragent']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_checkmwi'),'name' => 'checkmwi','labelid' => 'checkmwi','key' => false,'bbf' => array('mixkey','fm_checkmwi-opt'),'value' => xivo_cast_except($this->varra('info','checkmwi'),null,'uint'),'default' => $element['checkmwi']['default']),$element['checkmwi']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_regcontext'),'name' => 'regcontext','labelid' => 'regcontext','size' => 15,'value' => $this->varra('info','regcontext'),'default' => $element['regcontext']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_callerid'),'name' => 'callerid','labelid' => 'callerid','size' => 15,'value' => $this->varra('info','callerid'),'default' => $element['callerid']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_fromdomain'),'name' => 'fromdomain','labelid' => 'fromdomain','size' => 15,'value' => $this->varra('info','fromdomain'),'default' => $element['fromdomain']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_sipdebug'),'name' => 'sipdebug','labelid' => 'sipdebug','checked' => $this->varra('info','sipdebug'),'default' => $element['sipdebug']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_dumphistory'),'name' => 'dumphistory','labelid' => 'dumphistory','checked' => $this->varra('info','dumphistory'),'default' => $element['dumphistory']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_recordhistory'),'name' => 'recordhistory','labelid' => 'recordhistory','checked' => $this->varra('info','recordhistory'),'default' => $element['recordhistory']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_callevents'),'name' => 'callevents','labelid' => 'callevents','checked' => $this->varra('info','callevents'),'default' => $element['callevents']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_tos'),'name' => 'tos','labelid' => 'tos','key' => false,'value' => $tos,'default' => $element['tos']['default']),$element['tos']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_ospauth'),'name' => 'ospauth','labelid' => 'ospauth','key' => false,'bbf' => array('concatvalue','fm_ospauth-opt-'),'value' => $this->varra('info','ospauth'),'default' => $element['ospauth']['default']),$element['ospauth']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

<div id="sb-part-network" class="b-nodisplay">

<?=$form->text(array('desc' => $this->bbf('fm_localnet'),'name' => 'localnet','labelid' => 'localnet','size' => 15,'value' => $this->varra('info','localnet'),'default' => $element['localnet']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_externip'),'name' => 'externip','labelid' => 'externip','size' => 15,'value' => $this->varra('info','externip'),'default' => $element['externip']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_externhost'),'name' => 'externhost','labelid' => 'externhost','size' => 15,'value' => $this->varra('info','externhost'),'default' => $element['externhost']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_externrefresh'),'name' => 'externrefresh','labelid' => 'externrefresh','bbf' => array('mixkey','fm_externrefresh-opt','paramarray'),'value' => xivo_cast_except($this->varra('info','externrefresh'),null,'uint'),'default' => $element['externrefresh']['default']),$element['externrefresh']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_outboundproxy'),'name' => 'outboundproxy','labelid' => 'outboundproxy','size' => 15,'value' => $this->varra('info','outboundproxy'),'default' => $element['outboundproxy']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_outboundproxyport'),'name' => 'outboundproxyport','labelid' => 'outboundproxyport','value' => $this->varra('info','outboundproxyport'),'default' => $element['outboundproxyport']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

<div id="sb-part-signalling" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_relaxdtmf'),'name' => 'relaxdtmf','labelid' => 'relaxdtmf','checked' => $this->varra('info','relaxdtmf'),'default' => $element['relaxdtmf']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_compactheaders'),'name' => 'compactheaders','labelid' => 'compactheaders','checked' => $this->varra('info','compactheaders'),'default' => $element['compactheaders']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_rtptimeout'),'name' => 'rtptimeout','labelid' => 'rtptimeout','key' => false,'bbf' => array('mixkey','fm_rtptimeout-opt'),'value' => xivo_cast_except($this->varra('info','rtptimeout'),null,'uint'),'default' => $element['rtptimeout']['default']),$element['rtptimeout']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_rtpholdtimeout'),'name' => 'rtpholdtimeout','labelid' => 'rtpholdtimeout','key' => false,'bbf' => array('mixkey','fm_rtptimeout-opt'),'value' => xivo_cast_except($this->varra('info','rtpholdtimeout'),null,'uint'),'default' => $element['rtpholdtimeout']['default']),$element['rtpholdtimeout']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_rtpkeepalive'),'name' => 'rtpkeepalive','labelid' => 'rtpkeepalive','key' => false,'bbf' => array('mixkey','fm_rtpkeepalive-opt'),'value' => xivo_cast_except($this->varra('info','rtpkeepalive'),null,'uint'),'default' => $element['rtpkeepalive']['default']),$element['rtpkeepalive']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_notifymimetype'),'name' => 'notifymimetype','labelid' => 'notifymimetype','key' => false,'value' => $this->varra('info','notifymimetype'),'default' => $element['notifymimetype']['default']),$element['notifymimetype']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_srvlookup'),'name' => 'srvlookup','labelid' => 'srvlookup','checked' => $this->varra('info','srvlookup'),'default' => $element['srvlookup']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_pedantic'),'name' => 'pedantic','labelid' => 'pedantic','checked' => $this->varra('info','pedantic'),'default' => $element['pedantic']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_maxexpiry'),'name' => 'maxexpiry','labelid' => 'maxexpiry','bbf' => array('mixkey','fm_maxexpiry-opt','paramarray'),'value' => xivo_cast_except($this->varra('info','maxexpiry'),null,'uint'),'default' => $element['maxexpiry']['default']),$element['maxexpiry']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_defaultexpiry'),'name' => 'defaultexpiry','labelid' => 'defaultexpiry','bbf' => array('mixkey','fm_defaultexpiry-opt','paramarray'),'value' => xivo_cast_except($this->varra('info','defaultexpiry'),null,'uint'),'default' => $element['defaultexpiry']['default']),$element['defaultexpiry']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_registertimeout'),'name' => 'registertimeout','labelid' => 'registertimeout','bbf' => array('mixkey','fm_registertimeout-opt','paramarray'),'value' => xivo_cast_except($this->varra('info','registertimeout'),null,'uint'),'default' => $element['registertimeout']['default']),$element['registertimeout']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_registerattempts'),'name' => 'registerattempts','labelid' => 'registerattempts','bbf' => array('mixkey','fm_registerattempts-opt','paramarray'),'value' => xivo_cast_except($this->varra('info','registerattempts'),null,'uint'),'default' => $element['registerattempts']['default']),$element['registerattempts']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_notifyringing'),'name' => 'notifyringing','labelid' => 'notifyringing','checked' => $this->varra('info','notifyringing'),'default' => $element['notifyringing']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_codec-disallow'),'name' => 'disallow','labelid' => 'disallow','key' => false,'bbf' => array('concatvalue','fm_codec-disallow-opt-')),$element['disallow']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<div id="codeclist" class="fm-field fm-multilist"><p><label id="lb-codeclist" for="it-codeclist"><?=$this->bbf('fm_codec-allow');?></label></p>
	<div class="slt-outlist">
		<?=$form->slt(array('name' => 'codeclist','label' => false,'id' => 'it-codeclist','key_val' => 'id','multiple' => true,'size' => 5,'field' => false,'key' => false),$element['allow']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
	</div>
	<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-codeclist','it-codec'); return(false);" title="<?=$this->bbf('bt-incodec');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-incodec'),'class="bt-inlist" id="bt-incodec" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-codec','it-codeclist'); return(false);" title="<?=$this->bbf('bt-outcodec');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outcodec'),'class="bt-outlist" id="bt-outcodec" border="0"');?></a>
	</div>
	<div class="slt-inlist">

		<?=$form->slt(array('name' => 'allow[]','label' => false,'id' => 'it-codec','multiple' => true,'size' => 5,'field' => false,'key' => false),$this->varra('info','allow'),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

		<div class="bt-updown">

			<a href="#" onclick="xivo_fm_order_selected('it-codec',1); return(false);" title="<?=$this->bbf('bt-upcodec');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt-upcodec'),'class="bt-uplist" id="bt-upcodec" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-codec',-1); return(false);" title="<?=$this->bbf('bt-downcodec');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt-downcodec'),'class="bt-downlist" id="bt-downcodec" border="0"');?></a>

		</div>

	</div>
</div>
<div class="clearboth"></div>

</div>

<div id="sb-part-default" class="b-nodisplay">

<?=$form->text(array('desc' => $this->bbf('fm_context'),'name' => 'context','labelid' => 'context','size' => 15,'value' => $this->varra('info','context'),'default' => $element['context']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_nat'),'name' => 'nat','labelid' => 'nat','key' => false,'bbf' => array('concatvalue','fm_nat-opt-'),'value' => $this->varra('info','nat'),'default' => $element['nat']['default']),$element['nat']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_dtmfmode'),'name' => 'dtmfmode','labelid' => 'dtmfmode','key' => false,'value' => $this->varra('info','dtmfmode'),'default' => $element['dtmfmode']['default']),$element['dtmfmode']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_qualify'),'name' => 'qualify','labelid' => 'qualify','key' => false,'bbf' => array('mixkey','fm_qualify-opt'),'value' => $qualify,'default' => $element['qualify']['default']),$element['qualify']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_useclientcode'),'name' => 'useclientcode','labelid' => 'useclientcode','checked' => $this->varra('info','useclientcode'),'default' => $element['useclientcode']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_progressinband'),'name' => 'progressinband','labelid' => 'progressinband','key' => false,'bbf' => array('concatvalue','fm_progressinband-opt-'),'value' => $this->varra('info','progressinband'),'default' => $element['progressinband']['default']),$element['progressinband']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_language'),'name' => 'language','labelid' => 'language','key' => false,'value' => $this->varra('info','language'),'default' => $element['language']['default']),$element['language']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?php
	if($moh_list !== false):
		echo $form->slt(array('desc' => $this->bbf('fm_musiconhold'),'name' => 'musiconhold','labelid' => 'musiconhold','key' => 'category','value' => $this->varra('info','musiconhold'),'default' => $element['musiconhold']['default']),$moh_list,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');
	endif;
?>

<?=$form->text(array('desc' => $this->bbf('fm_vmexten'),'name' => 'vmexten','labelid' => 'vmexten','value' => $this->varra('info','vmexten'),'default' => $element['vmexten']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_trustrpid'),'name' => 'trustrpid','labelid' => 'trustrpid','checked' => $this->varra('info','trustrpid'),'default' => $element['trustrpid']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_sendrpid'),'name' => 'sendrpid','labelid' => 'sendrpid','checked' => $this->varra('info','sendrpid'),'default' => $element['sendrpid']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_canreinvite'),'name' => 'canreinvite','labelid' => 'canreinvite','key' => false,'bbf' => array('concatvalue','fm_canreinvite-opt-'),'value' => $this->varra('info','canreinvite'),'default' => $element['canreinvite']['default']),$element['canreinvite']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_insecure'),'name' => 'insecure','labelid' => 'insecure','empty' => true,'bbf' => array('concatvalue','fm_insecure-opt-'),'value' => $this->varra('info','insecure'),'default' => $element['insecure']['default']),$element['insecure']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

<div id="sb-part-realtime" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_rtcachefriends'),'name' => 'rtcachefriends','labelid' => 'rtcachefriends','checked' => $this->varra('info','rtcachefriends'),'default' => $element['rtcachefriends']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_rtupdate'),'name' => 'rtupdate','labelid' => 'rtupdate','checked' => $this->varra('info','rtupdate'),'default' => $element['rtupdate']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_ignoreregexpire'),'name' => 'ignoreregexpire','labelid' => 'ignoreregexpire','checked' => $this->varra('info','ignoreregexpire'),'default' => $element['ignoreregexpire']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_rtautoclear'),'name' => 'rtautoclear','labelid' => 'rtautoclear','bbf' => array('mixkey','fm_rtautoclear-opt','paramarray'),'value' => $rtautoclear,'default' => $element['rtautoclear']['default']),$element['rtautoclear']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

		<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
