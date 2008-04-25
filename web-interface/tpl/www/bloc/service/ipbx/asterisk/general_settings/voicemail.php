<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');
	$dhtml = &$this->get_module('dhtml');

	$element = $this->get_var('element');
	$context_list = $this->get_var('context_list');
	$err = $this->get_var('error');

	if($this->get_var('fm_save') === true):
		$dhtml->write_js('xivo_form_success(\''.$dhtml->escape($this->bbf('fm_success-save')).'\');');
	endif;

	$format = $this->get_varra('voicemail',array('format','var_val'));

	if(is_array($format) === true && empty($format) === false):
		$attachformat = true;
	else:
		$attachformat = false;
	endif;

	$zonemessages = $this->get_var('zonemessages');

	if(is_array($zonemessages) === true && ($zmsg_nb = count($zonemessages)) > 0):
		$zmsg_js = array();
		$zmsg_js[0] = 'xivo_tlist[\'timezone\'] = new Array();';
		$zmsg_js[1] = 'xivo_tlist[\'timezone\'][\'cnt\'] = '.$zmsg_nb.';';

		$dhtml->write_js($zmsg_js);
	else:
		$zmsg_nb = 0;
	endif;
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>

<div class="sb-smenu">
	<ul>
		<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-first');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_general');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-voicemenu');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_voicemenu');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-email');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_email');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-4" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-pager');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_pager');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-5" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-timezone');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_timezones');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-6" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-adsi');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_adsi');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-7" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-last',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_advanced');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>
	
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-voicemail-format');">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => 1));?>

<div id="sb-part-first">

<?=$form->select(array('desc' => $this->bbf('fm_voicemail-maxmsg'),'name' => 'voicemail[maxmsg]','labelid' => 'voicemail-maxmsg','key' => false,'value' => $this->get_varra('voicemail',array('maxmsg','var_val')),'default' => $element['voicemail']['maxmsg']['default']),$element['voicemail']['maxmsg']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_voicemail-silencethreshold'),'name' => 'voicemail[silencethreshold]','labelid' => 'voicemail-silencethreshold','key' => false,'value' => $this->get_varra('voicemail',array('silencethreshold','var_val')),'default' => $element['voicemail']['silencethreshold']['default']),$element['voicemail']['silencethreshold']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_voicemail-minmessage'),'name' => 'voicemail[minmessage]','labelid' => 'voicemail-minmessage','bbf' => array('mixkey','fm_voicemail-minmessage-opt','paramarray'),'value' => $this->get_varra('voicemail',array('minmessage','var_val')),'default' => $element['voicemail']['minmessage']['default']),$element['voicemail']['minmessage']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_voicemail-maxmessage'),'name' => 'voicemail[maxmessage]','labelid' => 'voicemail-maxmessage','bbf' => array('mixkey','fm_voicemail-maxmessage-opt','paramarray'),'value' => $this->get_varra('voicemail',array('maxmessage','var_val')),'default' => $element['voicemail']['maxmessage']['default']),$element['voicemail']['maxmessage']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_voicemail-maxsilence'),'name' => 'voicemail[maxsilence]','labelid' => 'voicemail-maxsilence','key' => false,'bbf' => array('mixkey','fm_voicemail-maxsilence-opt'),'value' => $this->get_varra('voicemail',array('maxsilence','var_val')),'default' => $element['voicemail']['maxsilence']['default']),$element['voicemail']['maxsilence']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-review'),'name' => 'voicemail[review]','labelid' => 'voicemail-review','checked' => $this->get_varra('voicemail',array('review','var_val')),'default' => $element['voicemail']['review']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-operator'),'name' => 'voicemail[operator]','labelid' => 'voicemail-operator','checked' => $this->get_varra('voicemail',array('operator','var_val')),'default' => $element['voicemail']['operator']['default']));?>

<div id="formatlist" class="fm-field fm-multilist"><p><label id="lb-formatlist" for="it-formatlist"><?=$this->bbf('fm_voicemail-format');?></label></p>
	<div class="slt-outlist">
		<?=$form->select(array('name' => 'voicemail[formatlist]','label' => false,'id' => 'it-voicemail-formatlist','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_format_name_info-'),$element['voicemail']['format']['value']);?>
	</div>
	<div class="inout-list">

		<a href="#" onclick="xivo_informat(); return(false);" title="<?=$this->bbf('bt_informat');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt_informat'),'class="bt-inlist" id="bt-informat" border="0"');?></a><br />

		<a href="#" onclick="xivo_outformat(); return(false);" title="<?=$this->bbf('bt_outformat');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt_outformat'),'class="bt-outlist" id="bt-outformat" border="0"');?></a>
	</div>
	<div class="slt-inlist">

		<?=$form->select(array('name' => 'voicemail[format][]','label' => false,'id' => 'it-voicemail-format','multiple' => true,'size' => 5,'field' => false,'key' => false,'bbf' => 'ast_format_name_info-'),$format);?>

	</div>
</div>
<div class="clearboth"></div>

</div>

<div id="sb-part-voicemenu" class="b-nodisplay">

<?=$form->select(array('desc' => $this->bbf('fm_voicemail-maxlogins'),'name' => 'voicemail[maxlogins]','labelid' => 'voicemail-maxlogins','key' => false,'value' => $this->get_varra('voicemail',array('maxlogins','var_val')),'default' => $element['voicemail']['maxlogins']['default']),$element['voicemail']['maxlogins']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-envelope'),'name' => 'voicemail[envelope]','labelid' => 'voicemail-envelope','checked' => $this->get_varra('voicemail',array('envelope','var_val')),'default' => $element['voicemail']['envelope']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-saycid'),'name' => 'voicemail[saycid]','labelid' => 'voicemail-saycid','checked' => $this->get_varra('voicemail',array('saycid','var_val')),'default' => $element['voicemail']['saycid']['default']));?>

<?php

if($context_list !== false):
	echo $form->select(array('desc' => $this->bbf('fm_voicemail-cidinternalcontexts'),'name' => 'voicemail[cidinternalcontexts]','labelid' => 'voicemail-cidinternalcontexts','key' => 'identity','altkey' => 'name','empty' => true,'default' => $element['voicemail']['cidinternalcontexts']['default'],'value' => $this->get_varra('voicemail',array('cidinternalcontexts','var_val'))),$context_list);
endif;

?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-sayduration'),'name' => 'voicemail[sayduration]','labelid' => 'voicemail-sayduration','checked' => $this->get_varra('voicemail',array('sayduration','var_val')),'default' => $element['voicemail']['sayduration']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_voicemail-saydurationm'),'name' => 'voicemail[saydurationm]','labelid' => 'voicemail-saydurationm','key' => false,'bbf' => array('mixkey','fm_voicemail-saydurationm-opt'),'value' => $this->get_varra('voicemail',array('saydurationm','var_val')),'default' => $element['voicemail']['saydurationm']['default']),$element['voicemail']['saydurationm']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-forcename'),'name' => 'voicemail[forcename]','labelid' => 'voicemail-forcename','checked' => $this->get_varra('voicemail',array('forcename','var_val')),'default' => $element['voicemail']['forcename']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-forcegreetings'),'name' => 'voicemail[forcegreetings]','labelid' => 'voicemail-forcegreetings','checked' => $this->get_varra('voicemail',array('forcegreetings','var_val')),'default' => $element['voicemail']['forcegreetings']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_voicemail-maxgreet'),'name' => 'voicemail[maxgreet]','labelid' => 'voicemail-maxgreet','bbf' => array('mixkey','fm_voicemail-maxgreet-opt','paramarray'),'value' => $this->get_varra('voicemail',array('maxgreet','var_val')),'default' => $element['voicemail']['maxgreet']['default']),$element['voicemail']['maxgreet']['value']);?>

<?=$form->select(array('desc' => $this->bbf('fm_voicemail-skipms'),'name' => 'voicemail[skipms]','labelid' => 'voicemail-skipms','bbf' => array('mixvalue','fm_voicemail-skipms-opt'),'value' => $this->get_varra('voicemail',array('skipms','var_val')),'default' => $element['voicemail']['skipms']['default']),$element['voicemail']['skipms']['value']);?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-sendvoicemail'),'name' => 'voicemail[sendvoicemail]','labelid' => 'voicemail-sendvoicemail','checked' => $this->get_varra('voicemail',array('sendvoicemail','var_val')),'default' => $element['voicemail']['sendvoicemail']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-usedirectory'),'name' => 'voicemail[usedirectory]','labelid' => 'voicemail-usedirectory','checked' => $this->get_varra('voicemail',array('usedirectory','var_val')),'default' => $element['voicemail']['usedirectory']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-nextaftercmd'),'name' => 'voicemail[nextaftercmd]','labelid' => 'voicemail-nextaftercmd','checked' => $this->get_varra('voicemail',array('nextaftercmd','var_val')),'default' => $element['voicemail']['nextaftercmd']['default']));?>

<?php

if($context_list !== false):
	echo $form->select(array('desc' => $this->bbf('fm_voicemail-dialout'),'name' => 'voicemail[dialout]','labelid' => 'voicemail-dialout','key' => 'identity','altkey' => 'name','empty' => true,'default' => $element['voicemail']['dialout']['default'],'value' => $this->get_varra('voicemail',array('dialout','var_val'))),$context_list);

	echo $form->select(array('desc' => $this->bbf('fm_voicemail-callback'),'name' => 'voicemail[callback]','labelid' => 'voicemail-callback','key' => 'identity','altkey' => 'name','empty' => true,'default' => $element['voicemail']['callback']['default'],'value' => $this->get_varra('voicemail',array('callback','var_val'))),$context_list);

	echo $form->select(array('desc' => $this->bbf('fm_voicemail-exitcontext'),'name' => 'voicemail[exitcontext]','labelid' => 'voicemail-exitcontext','key' => 'identity','altkey' => 'name','empty' => true,'default' => $element['voicemail']['exitcontext']['default'],'value' => $this->get_varra('voicemail',array('exitcontext','var_val'))),$context_list);
endif;

?>

</div>

<div id="sb-part-email" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-attach'),'name' => 'voicemail[attach]','labelid' => 'voicemail-attach','checked' => $this->get_varra('voicemail',array('attach','var_val')),'default' => $element['voicemail']['attach']['default']));?>

<?=$form->select(array('desc' => $this->bbf('fm_voicemail-attachformat'),'name' => 'voicemail[attachformat]','labelid' => 'voicemail-attachformat','key' => false,'bbf' => 'ast_format_name_info-'),$format,($attachformat === false ? 'class="it-disabled" disabled="disabled"' : ''));?>

<?=$form->text(array('desc' => $this->bbf('fm_voicemail-mailcmd'),'name' => 'voicemail[mailcmd]','labelid' => 'voicemail-mailcmd','size' => 15,'value' => $this->get_varra('voicemail',array('mailcmd','var_val')),'default' => $element['voicemail']['mailcmd']['default']),'class="it-readonly" readonly="readonly"');?>

<?=$form->text(array('desc' => $this->bbf('fm_voicemail-charset'),'name' => 'voicemail[charset]','labelid' => 'voicemail-charset','size' => 15,'value' => $this->get_varra('voicemail',array('charset','var_val')),'default' => $element['voicemail']['charset']['default']),'class="it-readonly" readonly="readonly"');?>

<?=$form->text(array('desc' => $this->bbf('fm_voicemail-serveremail'),'name' => 'voicemail[serveremail]','labelid' => 'voicemail-serveremail','size' => 15,'value' => $this->get_varra('voicemail',array('serveremail','var_val')),'default' => $element['voicemail']['serveremail']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_voicemail-fromstring'),'name' => 'voicemail[fromstring]','labelid' => 'voicemail-fromstring','size' => 15,'value' => $this->get_varra('voicemail',array('fromstring','var_val')),'default' => $element['voicemail']['fromstring']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_voicemail-emaildateformat'),'name' => 'voicemail[emaildateformat]','labelid' => 'voicemail-emaildateformat','size' => 15,'value' => $this->get_varra('voicemail',array('emaildateformat','var_val')),'default' => $element['voicemail']['emaildateformat']['default']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-pbxskip'),'name' => 'voicemail[pbxskip]','labelid' => 'voicemail-pbxskip','checked' => $this->get_varra('voicemail',array('pbxskip','var_val')),'default' => $element['voicemail']['pbxskip']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_voicemail-emailsubject'),'name' => 'voicemail[emailsubject]','labelid' => 'voicemail-emailsubject','size' => 15,'value' => $this->get_varra('voicemail',array('emailsubject','var_val')),'default' => $element['voicemail']['emailsubject']['default']));?>

<div class="fm-field fm-description"><p><label id="lb-emailbody" for="it-emailbody"><?=$this->bbf('fm_voicemail-emailbody');?></label></p>
<?=$form->textarea(array('field' => false,'name' => 'voicemail[emailbody]','label' => false,'id' => 'it-voicemail-emailbody','cols' => 60,'rows' => 10),$this->get_varra('voicemail',array('emailbody','var_val')));?>
</div>

</div>

<div id="sb-part-pager" class="b-nodisplay">

<?=$form->text(array('desc' => $this->bbf('fm_voicemail-pagerfromstring'),'name' => 'voicemail[pagerfromstring]','labelid' => 'voicemail-pagerfromstring','size' => 15,'value' => $this->get_varra('voicemail',array('pagerfromstring','var_val')),'default' => $element['voicemail']['pagerfromstring']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_voicemail-pagersubject'),'name' => 'voicemail[pagersubject]','labelid' => 'voicemail-pagersubject','size' => 15,'value' => $this->get_varra('voicemail',array('pagersubject','var_val')),'default' => $element['voicemail']['pagersubject']['default']));?>

<div class="fm-field fm-description"><p><label id="lb-pagerbody" for="it-pagerbody"><?=$this->bbf('fm_voicemail-pagerbody');?></label></p>
<?=$form->textarea(array('field' => false,'name' => 'voicemail[pagerbody]','label' => false,'id' => 'it-voicemail-pagerbody','cols' => 60,'rows' => 4),$this->get_varra('voicemail',array('pagerbody','var_val')));?>
</div>

</div>

<div id="sb-part-timezone" class="b-nodisplay">

<div class="sb-list">
	<table cellspacing="0" cellpadding="0" border="0">
		<thead>
		<tr class="sb-top">
			<th class="th-left"><?=$this->bbf('col_timezone-name');?></th>
			<th class="th-center"><?=$this->bbf('col_timezone-timezone');?></th>
			<th class="th-center"><?=$this->bbf('col_timezone-options');?></th>
			<th class="th-right"><?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',$this->bbf('col_timezone-add'),'border="0"'),'#',null,'onclick="xivo_table_list(\'timezone\',this); return(false);"',$this->bbf('col_timezone-add'));?></th>
		</tr>
		</thead>
		<tbody id="timezone">
<?php
	if($zmsg_nb > 0):
		reset($zonemessages);

		foreach($zonemessages as $key => $val):

			if(isset($err['zonemessages'][$key]) === true):
				$errdisplay = ' l-infos-error';
			else:
				$errdisplay = '';
			endif;
?>
		<tr class="fm-field<?=$errdisplay?>">
			<td class="td-left"><?=$form->text(array('field' => false,'name' => 'zonemessages[name][]','id' => false,'label' => false,'value' => $val['name'],'default' => $element['zonemessages']['name']['default']));?></td>
			<td><?=$form->select(array('field' => false,'name' => 'zonemessages[timezone][]','key' => true,'id' => false,'label' => false,'value' => $val['timezone'],'default' => $element['zonemessages']['timezone']['default']),$this->get_var('timezone_list'));?></td>
			<td><?=$form->text(array('field' => false,'name' => 'zonemessages[msg_format][]','id' => false,'label' => false,'size' => 25,'value' => $val['msg_format'],'default' => $element['zonemessages']['msg_format']['default']));?></td>
			<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',$this->bbf('opt_delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'timezone\',this,1); return(false);"',$this->bbf('opt_delete'));?></td>
		</tr>
<?php
		endforeach;
	endif;
?>
		</tbody>
		<tfoot>
		<tr id="no-timezone"<?=($zmsg_nb > 0 ? ' class="b-nodisplay"' : '')?>>
			<td colspan="4" class="td-single"><?=$this->bbf('no_timezone');?></td>
		</tr>
		</tfoot>
	</table>
	<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
		<tbody id="ex-timezone">
		<tr class="fm-field">
			<td class="td-left"><?=$form->text(array('field' => false,'name' => 'zonemessages[name][]','id' => false,'label' => false,'default' => $element['zonemessages']['name']['default']),'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');?></td>
			<td><?=$form->select(array('field' => false,'name' => 'zonemessages[timezone][]','key' => true,'id' => false,'label' => false,'default' => $element['zonemessages']['timezone']['default']),$this->get_var('timezone_list'),'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');?></td>
			<td><?=$form->text(array('field' => false,'name' => 'zonemessages[msg_format][]','id' => false,'label' => false,'size' => 25,'default' => $element['zonemessages']['msg_format']['default']),'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');?></td>
			<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',$this->bbf('opt_delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'timezone\',this,1); return(false);"',$this->bbf('opt_delete'));?></td>
		</tr>
		</tbody>
	</table>
</div>

</div>

<div id="sb-part-adsi" class="b-nodisplay">

<?=$form->text(array('desc' => $this->bbf('fm_voicemail-adsifdn'),'name' => 'voicemail[adsifdn]','labelid' => 'voicemail-adsifdn','value' => $this->get_varra('voicemail',array('adsifdn','var_val')),'default' => $element['voicemail']['adsifdn']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_voicemail-adsisec'),'name' => 'voicemail[adsisec]','labelid' => 'voicemail-adsisec','value' => $this->get_varra('voicemail',array('adsisec','var_val')),'default' => $element['voicemail']['adsisec']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_voicemail-adsiver'),'name' => 'voicemail[adsiver]','labelid' => 'voicemail-adsiver','size' => 5,'value' => $this->get_varra('voicemail',array('adsiver','var_val')),'default' => $element['voicemail']['adsiver']['default']));?>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_voicemail-searchcontexts'),'name' => 'voicemail[searchcontexts]','labelid' => 'voicemail-searchcontexts','checked' => $this->get_varra('voicemail',array('searchcontexts','var_val')),'default' => $element['voicemail']['searchcontexts']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_voicemail-externpass'),'name' => 'voicemail[externpass]','labelid' => 'voicemail-externpass','size' => 15,'value' => $this->get_varra('voicemail',array('externpass','var_val')),'default' => $element['voicemail']['externpass']['default']),'class="it-readonly" readonly="readonly"');?>

<?=$form->text(array('desc' => $this->bbf('fm_voicemail-externnotify'),'name' => 'voicemail[externnotify]','labelid' => 'voicemail-externnotify','size' => 15,'value' => $this->get_varra('voicemail',array('externnotify','var_val')),'default' => $element['voicemail']['externnotify']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_voicemail-odbcstorage'),'name' => 'voicemail[odbcstorage]','labelid' => 'voicemail-odbcstorage','size' => 15,'value' => $this->get_varra('voicemail',array('odbcstorage','var_val')),'default' => $element['voicemail']['odbcstorage']['default']));?>

<?=$form->text(array('desc' => $this->bbf('fm_voicemail-odbctable'),'name' => 'voicemail[odbctable]','labelid' => 'voicemail-odbctable','size' => 15,'value' => $this->get_varra('voicemail',array('odbctable','var_val')),'default' => $element['voicemail']['odbctable']['default']));?>

</div>

<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>

</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
