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
		<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-first');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-first'); return(false);"><?=$this->bbf('smenu_emergency');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-last',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-last'); return(false);"><?=$this->bbf('smenu_special');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>
	
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>

<div id="sb-part-first">

<div class="sb-list">
	<table cellspacing="0" cellpadding="0" border="0">
		<thead>
		<tr class="sb-top">
			<th class="th-left"><?=$this->bbf('col_emergency-trunk');?></th>
			<th class="th-center"><?=$this->bbf('col_emergency-extentype');?></th>
			<th class="th-center"><?=$this->bbf('col_emergency-exten');?></th>
			<th class="th-right"><?=$url->href_html($url->img_html('img/site/button/add.gif',$this->bbf('col_emergency-add'),'border="0"'),'#',null,'onclick="xivo_table_list(\'emergency\',this);"',$this->bbf('col_emergency-add'));?></th>
		</tr>
		</thead>
		<tbody id="emergency">
<?php
/*
	if($zmsg_nb !== 0):
		for($i = 0;$i < $zmsg['cnt'];$i++):
			$key = &$zmsg['keys'][$i];
			$val = &$emergency[$key];
?>
		<tr class="fm-field">
			<td class="td-left txt-left"><?=$form->text(array('field' => false,'name' => 'emergency[provider][]','id' => false,'label' => false,'value' => $val['name'],'default' => $element['emergency']['name']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?></td>
			<td><?=$form->select(array('field' => false,'name' => 'emergency[timezone][]','key' => true,'id' => false,'label' => false,'value' => $val['timezone'],'default' => $element['emergency']['timezone']['default']),$this->vars('timezone_list'),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?></td>
			<td><?=$form->text(array('field' => false,'name' => 'emergency[msg_format][]','id' => false,'label' => false,'size' => 25,'value' => $val['msg_format'],'default' => $element['emergency']['msg_format']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?></td>
			<td class="td-right txt-right"><?=$url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'timezone\',this,1);"',$this->bbf('opt_delete'));?></td>
		</tr>
<?php
		endfor;
	endif;
*/
?>
		</tbody>
		<tfoot>
		<tr id="no-emergency">
			<td colspan="4" class="td-single"><?=$this->bbf('no_emergency');?></td>
		</tr>
		</tfoot>
	</table>
	<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
		<tbody id="ex-emergency">
		<tr class="fm-field">
			<td class="td-left txt-left"><?=$form->text(array('field' => false,'name' => 'emergency[trunkid][]','id' => false,'label' => false,'default' => $element['trunkid']['default']),'disabled="disabled" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?></td>
			<td><?=$form->select(array('field' => false,'name' => 'emergency[extentype][]','key' => false,'id' => false,'label' => false,'default' => $element['extentype']['default']),$element['extentype']['value'],'onchange="xivo_extentype(this);" disabled="disabled" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?></td>
			<td>
				<div<?=($element['extentype']['default'] === 'extension' ? '' : ' class="b-nodisplay"')?>>
				<?=$form->text(array('field' => false,'name' => 'emergency[exten][]','id' => false,'label' => false,'size' => 15,'default' => $element['exten']['default']),'disabled="disabled" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
				</div>
				<div<?=($element['extentype']['default'] !== 'extension' ? '' : ' class="b-nodisplay"')?>>
				<?=$form->text(array('field' => false,'name' => 'emergency[rangebeg][]','id' => false,'label' => false,'size' => 5,'default' => $element['rangebeg']['default']),'disabled="disabled" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
				<?=$form->text(array('field' => false,'name' => 'emergency[rangeend][]','id' => false,'label' => false,'size' => 5,'default' => $element['rangeend']['default']),'disabled="disabled" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
				</div>
			</td>
			
			<td class="td-right txt-right"><?=$url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'emergency\',this,1);"',$this->bbf('opt_delete'));?></td>
		</tr>
		</tbody>
	</table>
</div>

</div>

<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>

</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
