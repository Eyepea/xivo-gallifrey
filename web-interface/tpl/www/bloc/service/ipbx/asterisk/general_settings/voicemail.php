<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$element = $this->vars('element');

	if($this->vars('fm_save') === true):
		$dhtml = &$this->get_module('dhtml');
		$dhtml->write_js('xivo_form_success(\''.xivo_stript($this->bbf('fm_success-save')).'\');');
	endif;
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>

<div class="sb-smenu">
	<ul>
		<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-general');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-general'); return(false);"><?=$this->bbf('smenu_general');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-voicemenu');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-voicemenu'); return(false);"><?=$this->bbf('smenu_voicemenu');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-email');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-email'); return(false);"><?=$this->bbf('smenu_email');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-4" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-pager');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-pager'); return(false);"><?=$this->bbf('smenu_pager');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-5" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-adsi');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-adsi'); return(false);"><?=$this->bbf('smenu_adsi');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-6" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-advanced',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-advanced'); return(false);"><?=$this->bbf('smenu_advanced');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>
	
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-format');">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>

<div id="sb-part-general">

<?=$form->slt(array('desc' => $this->bbf('fm_maxmsg'),'name' => 'maxmsg','labelid' => 'maxmsg','key' => false,'value' => xivo_cast_except($this->varra('info','maxmsg'),null,'uint'),'default' => $element['maxmsg']['default']),$element['maxmsg']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_silencethreshold'),'name' => 'silencethreshold','labelid' => 'silencethreshold','key' => false,'value' => xivo_cast_except($this->varra('info','silencethreshold'),null,'uint'),'default' => $element['silencethreshold']['default']),$element['silencethreshold']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_minmessage'),'name' => 'minmessage','labelid' => 'minmessage','bbf' => array('mixkey','fm_minmessage-opt','paramarray'),'value' => xivo_cast_except($this->varra('info','minmessage'),null,'uint'),'default' => $element['minmessage']['default']),$element['minmessage']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_maxmessage'),'name' => 'maxmessage','labelid' => 'maxmessage','bbf' => array('mixkey','fm_maxmessage-opt','paramarray'),'value' => xivo_cast_except($this->varra('info','maxmessage'),null,'uint'),'default' => $element['maxmessage']['default']),$element['maxmessage']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_maxsilence'),'name' => 'maxsilence','labelid' => 'maxsilence','key' => false,'bbf' => array('mixkey','fm_maxsilence-opt'),'value' => xivo_cast_except($this->varra('info','maxsilence'),null,'uint'),'default' => $element['maxsilence']['default']),$element['maxsilence']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_review'),'name' => 'review','labelid' => 'review','checked' => $this->varra('info','review'),'default' => $element['review']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_operator'),'name' => 'operator','labelid' => 'operator','checked' => $this->varra('info','operator'),'default' => $element['operator']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<div id="formatlist" class="fm-field fm-multilist"><p><label id="lb-formatlist" for="it-formatlist"><?=$this->bbf('fm_format');?></label></p>
	<div class="slt-outlist">
		<?=$form->slt(array('name' => 'formatlist','label' => false,'id' => 'it-formatlist','key_val' => 'id','multiple' => true,'size' => 5,'field' => false,'key' => false),$element['format']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
	</div>
	<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-formatlist','it-format'); return(false);" title="<?=$this->bbf('bt-informat');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-informat'),'class="bt-inlist" id="bt-informat" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-format','it-formatlist'); return(false);" title="<?=$this->bbf('bt-outformat');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outformat'),'class="bt-outlist" id="bt-outformat" border="0"');?></a>
	</div>
	<div class="slt-inlist">

		<?=$form->slt(array('name' => 'format[]','label' => false,'id' => 'it-format','multiple' => true,'size' => 5,'field' => false,'key' => false),$this->varra('info','format'),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

	</div>
</div>
<div class="clearboth"></div>

</div>

<div id="sb-part-voicemenu" class="b-nodisplay">

<?=$form->slt(array('desc' => $this->bbf('fm_maxlogins'),'name' => 'maxlogins','labelid' => 'maxlogins','key' => false,'value' => xivo_cast_except($this->varra('info','maxlogins'),null,'uint'),'default' => $element['maxlogins']['default']),$element['maxlogins']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_envelope'),'name' => 'envelope','labelid' => 'envelope','checked' => $this->varra('info','envelope'),'default' => $element['envelope']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_saycid'),'name' => 'saycid','labelid' => 'saycid','checked' => $this->varra('info','saycid'),'default' => $element['saycid']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_cidinternalcontexts'),'name' => 'cidinternalcontexts','labelid' => 'cidinternalcontexts','size' => 15,'value' => $this->varra('info','cidinternalcontexts'),'default' => $element['cidinternalcontexts']['default']),'class="it-readonly" readonly="readonly"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_sayduration'),'name' => 'sayduration','labelid' => 'sayduration','checked' => $this->varra('info','sayduration'),'default' => $element['sayduration']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_saydurationm'),'name' => 'saydurationm','labelid' => 'saydurationm','key' => false,'bbf' => array('mixkey','fm_saydurationm-opt'),'value' => xivo_cast_except($this->varra('info','saydurationm'),null,'uint'),'default' => $element['saydurationm']['default']),$element['saydurationm']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_forcename'),'name' => 'forcename','labelid' => 'forcename','checked' => $this->varra('info','forcename'),'default' => $element['forcename']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_forcegreetings'),'name' => 'forcegreetings','labelid' => 'forcegreetings','checked' => $this->varra('info','forcegreetings'),'default' => $element['forcegreetings']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_maxgreet'),'name' => 'maxgreet','labelid' => 'maxgreet','bbf' => array('mixkey','fm_maxgreet-opt','paramarray'),'value' => xivo_cast_except($this->varra('info','maxgreet'),null,'uint'),'default' => $element['maxgreet']['default']),$element['maxgreet']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_skipms'),'name' => 'skipms','labelid' => 'skipms','bbf' => array('mixkey','fm_skipms-opt'),'value' => xivo_cast_except($this->varra('info','skipms'),null,'uint'),'default' => $element['skipms']['default']),$element['skipms']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_sendvoicemail'),'name' => 'sendvoicemail','labelid' => 'sendvoicemail','checked' => $this->varra('info','sendvoicemail'),'default' => $element['sendvoicemail']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_usedirectory'),'name' => 'usedirectory','labelid' => 'usedirectory','checked' => $this->varra('info','usedirectory'),'default' => $element['usedirectory']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_nextaftercmd'),'name' => 'nextaftercmd','labelid' => 'nextaftercmd','checked' => $this->varra('info','nextaftercmd'),'default' => $element['nextaftercmd']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_dialout'),'name' => 'dialout','labelid' => 'dialout','size' => 15,'value' => $this->varra('info','dialout'),'default' => $element['dialout']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_callback'),'name' => 'callback','labelid' => 'callback','size' => 15,'value' => $this->varra('info','callback'),'default' => $element['callback']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_exitcontext'),'name' => 'exitcontext','labelid' => 'exitcontext','size' => 15,'value' => $this->varra('info','exitcontext'),'default' => $element['exitcontext']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

<div id="sb-part-email" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_attach'),'name' => 'attach','labelid' => 'attach','checked' => $this->varra('info','attach'),'default' => $element['attach']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_mailcmd'),'name' => 'mailcmd','labelid' => 'mailcmd','size' => 15,'value' => $this->varra('info','mailcmd'),'default' => $element['mailcmd']['default']),'class="it-readonly" readonly="readonly"');?>

<?=$form->text(array('desc' => $this->bbf('fm_charset'),'name' => 'charset','labelid' => 'charset','size' => 15,'value' => $this->varra('info','charset'),'default' => $element['charset']['default']),'class="it-readonly" readonly="readonly"');?>

<?=$form->text(array('desc' => $this->bbf('fm_serveremail'),'name' => 'serveremail','labelid' => 'serveremail','size' => 15,'value' => $this->varra('info','serveremail'),'default' => $element['serveremail']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_fromstring'),'name' => 'fromstring','labelid' => 'fromstring','size' => 15,'value' => $this->varra('info','fromstring'),'default' => $element['fromstring']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_emaildateformat'),'name' => 'emaildateformat','labelid' => 'emaildateformat','size' => 15,'value' => $this->varra('info','emaildateformat'),'default' => $element['emaildateformat']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_pbxskip'),'name' => 'pbxskip','labelid' => 'pbxskip','checked' => $this->varra('info','pbxskip'),'default' => $element['pbxskip']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_emailsubject'),'name' => 'emailsubject','labelid' => 'emailsubject','size' => 15,'value' => $this->varra('info','emailsubject'),'default' => $element['emailsubject']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<div id="emailbody" class="fm-field"><p><label id="lb-emailbody" for="it-emailbody"><?=$this->bbf('fm_emailbody');?></label></p>
<?=$form->textarea(array('field' => false,'name' => 'emailbody','label' => false,'id' => 'it-emailbody','cols' => 60,'rows' => 10),$this->varra('info','emailbody'),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

</div>

<div id="sb-part-pager" class="b-nodisplay">

<?=$form->text(array('desc' => $this->bbf('fm_pagerfromstring'),'name' => 'pagerfromstring','labelid' => 'pagerfromstring','size' => 15,'value' => $this->varra('info','pagerfromstring'),'default' => $element['pagerfromstring']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_pagersubject'),'name' => 'pagersubject','labelid' => 'pagersubject','size' => 15,'value' => $this->varra('info','pagersubject'),'default' => $element['pagersubject']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<div id="pagerbody" class="fm-field txt-center"><p><label id="lb-pagerbody" for="it-pagerbody"><?=$this->bbf('fm_pagerbody');?></label></p>
<?=$form->textarea(array('field' => false,'name' => 'pagerbody','label' => false,'id' => 'it-pagerbody','cols' => 60,'rows' => 4),$this->varra('info','pagerbody'),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

</div>

<div id="sb-part-adsi" class="b-nodisplay">

<?=$form->text(array('desc' => $this->bbf('fm_adsifdn'),'name' => 'adsifdn','labelid' => 'adsifdn','value' => $this->varra('info','adsifdn'),'default' => $element['adsifdn']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_adsisec'),'name' => 'adsisec','labelid' => 'adsisec','value' => $this->varra('info','adsisec'),'default' => $element['adsisec']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_adsiver'),'name' => 'adsiver','labelid' => 'adsiver','size' => 15,'value' => $this->varra('info','adsiver'),'default' => $element['adsiver']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

<div id="sb-part-advanced" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_searchcontexts'),'name' => 'searchcontexts','labelid' => 'searchcontexts','checked' => $this->varra('info','searchcontexts'),'default' => $element['searchcontexts']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_externpass'),'name' => 'externpass','labelid' => 'externpass','size' => 15,'value' => $this->varra('info','externpass'),'default' => $element['externpass']['default']),'class="it-readonly" readonly="readonly"');?>

<?=$form->text(array('desc' => $this->bbf('fm_externnotify'),'name' => 'externnotify','labelid' => 'externnotify','size' => 15,'value' => $this->varra('info','externnotify'),'default' => $element['externnotify']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_odbcstorage'),'name' => 'odbcstorage','labelid' => 'odbcstorage','size' => 15,'value' => $this->varra('info','odbcstorage'),'default' => $element['odbcstorage']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_odbctable'),'name' => 'odbctable','labelid' => 'odbctable','size' => 15,'value' => $this->varra('info','odbctable'),'default' => $element['odbctable']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

</div>

<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>

</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
