<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');
	$dhtml = &$this->get_module('dhtml');

	$element = $this->vars('element');
	$info = $this->vars('info');
	$trunks_list = $this->vars('trunks_list');
	$smenu = $this->vars('fm_smenu');

	$outcall_js = array();

	if($this->vars('fm_save') === true):
		$outcall_js[] = 'xivo_form_success(\''.xivo_stript($this->bbf('fm_success-save')).'\');';
	endif;

	if(($smenu_tab = $this->vars('fm_smenu_tab')) !== '' && ($smenu_part = $this->vars('fm_smenu_part')) !== ''):
		$outcall_js[] = 'xivo_smenu[\'tab\'] = \''.xivo_stript($smenu_tab).'\';';
		$outcall_js[] = 'xivo_smenu[\'part\'] = \''.xivo_stript($smenu_part).'\';';

		if($smenu_part === 'sb-part-last'):
			$outcall_js[] = 'xivo_smenu[\'last\'] = true;';
		endif;
	endif;

	if($info['emergency'] !== false):
		$egency_nb = count($info['emergency']);
		$outcall_js[] = 'xivo_tlist[\'emergency\'] = new Array();';
		$outcall_js[] = 'xivo_tlist[\'emergency\'][\'cnt\'] = '.$egency_nb.';';
	else:
		$egency_nb = 0;
	endif;

	if($info['special'] !== false):
		$special_nb = count($info['special']);
		$outcall_js[] = 'xivo_tlist[\'special\'] = new Array();';
		$outcall_js[] = 'xivo_tlist[\'special\'][\'cnt\'] = '.$special_nb.';';
	else:
		$special_nb = 0;
	endif;

	$dhtml->write_js($outcall_js);
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

<?php

if($trunks_list === false):
	echo '<div class="txt-center">',$url->href_html($this->bbf('create_trunk'),'service/ipbx/trunk_management/sip','act=add'),'</div>';
else:

?>

<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_smenu_fmsubmit(this);">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => 1));?>
<?=$form->hidden(array('name' => 'fm_smenu-tab','value' => 'smenu-tab-1'));?>
<?=$form->hidden(array('name' => 'fm_smenu-part','value' => 'sb-part-first'));?>

<div id="sb-part-first" class="b-nodisplay">

<div class="sb-list">
	<table cellspacing="0" cellpadding="0" border="0">
		<thead>
		<tr class="sb-top">
			<th class="th-left"><?=$this->bbf('col_emergency-trunk');?></th>
			<th class="th-center"><?=$this->bbf('col_emergency-extenmode');?></th>
			<th class="th-center"><?=$this->bbf('col_emergency-exten');?></th>
			<th class="th-right"><?=$url->href_html($url->img_html('img/site/button/add.gif',$this->bbf('col_emergency-add'),'border="0"'),'#',null,'onclick="xivo_table_list(\'emergency\',this);"',$this->bbf('col_emergency-add'));?></th>
		</tr>
		</thead>
		<tbody id="emergency">
<?php
	if($egency_nb !== 0):
		for($i = 0;$i < $egency_nb;$i++):
			$ref = &$info['emergency'][$i];

			if($ref['extenumbers']['extenmode'] === 'range'):
				$extendisplay = ' class="b-nodisplay"';
				$rangedisplay = '';
			else:
				$extendisplay = '';
				$rangedisplay = ' class="b-nodisplay"';
			endif;

			if(isset($ref['_error']) === true):
				$errdisplay = ' l-infos-error';
			else:
				$errdisplay = '';
			endif;
?>
		<tr class="fm-field<?=$errdisplay?>">
			<td class="td-left txt-left">
<?php
			if(isset($ref['generaloutcall']['id']) === true):
				$id = $ref['generaloutcall']['id'];
			else:
				$id = '';
			endif;

			echo $form->hidden(array('name' => 'emergency[id][]','value' => $id));

			echo $form->select(array('field' => false,'name' => 'emergency[trunkfeaturesid][]','id' => false,'label' => false,'browse' => 'trunk','key' => 'name','altkey' => 'trunkfeaturesid','optgroup' => array('key' => true,'bbf' => array('concat','fm_emergency-trunk-opt-')),'value' => $ref['generaloutcall']['trunkfeaturesid'],'default' => $element['generaloutcall']['trunkfeaturesid']['default']),$trunks_list);
?>
			</td>
			<td><?=$form->select(array('field' => false,'name' => 'emergency[extenmode][]','key' => false,'id' => false,'label' => false,'bbf' => array('concatkey','fm_emergency-extenmode-opt-'),'value' => $ref['extenumbers']['extenmode'],'default' => $element['extenumbers']['extenmode']['default']),$element['extenumbers']['extenmode']['value'],'onchange="xivo_extenmode(this);"');?></td>
			<td>
				<div<?=$extendisplay?>>
				<?=$form->text(array('field' => false,'name' => 'emergency[exten][]','id' => false,'label' => false,'size' => 15,'value' => $ref['extenumbers']['exten'],'default' => $element['extenumbers']['exten']['default']));?>
				</div>
				<div<?=$rangedisplay?>>
				<?=$form->text(array('field' => false,'name' => 'emergency[rangebeg][]','id' => false,'label' => false,'size' => 5,'value' => $ref['extenumbers']['rangebeg'],'default' => $element['extenumbers']['rangebeg']['default']));?>
				<?=$form->text(array('field' => false,'name' => 'emergency[rangeend][]','id' => false,'label' => false,'size' => 5,'value' => $ref['extenumbers']['rangeend'],'default' => $element['extenumbers']['rangeend']['default']));?>
				</div>
			</td>
			
			<td class="td-right txt-right"><?=$url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_emergency-delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'emergency\',this,1);"',$this->bbf('opt_emergency-delete'));?></td>
		</tr>

<?php
		endfor;
	endif;
?>
		</tbody>
		<tfoot>
		<tr id="no-emergency"<?=($egency_nb !== 0 ? ' class="b-nodisplay"' : '')?>>
			<td colspan="4" class="td-single"><?=$this->bbf('no_emergency');?></td>
		</tr>
		</tfoot>
	</table>
	<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
		<tbody id="ex-emergency">
		<tr class="fm-field">
			<td class="td-left txt-left"><?=$form->select(array('field' => false,'name' => 'emergency[trunkfeaturesid][]','id' => false,'label' => false,'browse' => 'trunk','key' => 'name','altkey' => 'trunkfeaturesid','optgroup' => array('key' => true,'bbf' => array('concat','fm_emergency-trunk-opt-')),'default' => $element['generaloutcall']['trunkfeaturesid']['default']),$trunks_list,'disabled="disabled"');?></td>
			<td><?=$form->select(array('field' => false,'name' => 'emergency[extenmode][]','key' => false,'id' => false,'label' => false,'bbf' => array('concatkey','fm_emergency-extenmode-opt-'),'default' => $element['extenumbers']['extenmode']['default']),$element['extenumbers']['extenmode']['value'],'onchange="xivo_extenmode(this);" disabled="disabled"');?></td>
			<td>
				<div<?=($element['extenumbers']['extenmode']['default'] === 'extension' ? '' : ' class="b-nodisplay"')?>>
				<?=$form->text(array('field' => false,'name' => 'emergency[exten][]','id' => false,'label' => false,'size' => 15,'default' => $element['extenumbers']['exten']['default']),'disabled="disabled"');?>
				</div>
				<div<?=($element['extenumbers']['extenmode']['default'] !== 'extension' ? '' : ' class="b-nodisplay"')?>>
				<?=$form->text(array('field' => false,'name' => 'emergency[rangebeg][]','id' => false,'label' => false,'size' => 5,'default' => $element['extenumbers']['rangebeg']['default']),'disabled="disabled"');?>
				<?=$form->text(array('field' => false,'name' => 'emergency[rangeend][]','id' => false,'label' => false,'size' => 5,'default' => $element['extenumbers']['rangeend']['default']),'disabled="disabled"');?>
				</div>
			</td>
			
			<td class="td-right txt-right"><?=$url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'emergency\',this,1);"',$this->bbf('opt_delete'));?></td>
		</tr>
		</tbody>
	</table>
</div>

</div>

<div id="sb-part-last" class="b-nodisplay">

<div class="sb-list">
	<table cellspacing="0" cellpadding="0" border="0">
		<thead>
		<tr class="sb-top">
			<th class="th-left"><?=$this->bbf('col_special-trunk');?></th>
			<th class="th-center"><?=$this->bbf('col_special-extenmode');?></th>
			<th class="th-center"><?=$this->bbf('col_special-exten');?></th>
			<th class="th-right"><?=$url->href_html($url->img_html('img/site/button/add.gif',$this->bbf('col_special-add'),'border="0"'),'#',null,'onclick="xivo_table_list(\'special\',this);"',$this->bbf('col_special-add'));?></th>
		</tr>
		</thead>
		<tbody id="special">
<?php
	if($special_nb !== 0):
		for($i = 0;$i < $special_nb;$i++):
			$ref = &$info['special'][$i];

			if($ref['extenumbers']['extenmode'] === 'range'):
				$extendisplay = ' class="b-nodisplay"';
				$rangedisplay = '';
			else:
				$extendisplay = '';
				$rangedisplay = ' class="b-nodisplay"';
			endif;

			if(isset($ref['_error']) === true):
				$errdisplay = ' l-infos-error';
			else:
				$errdisplay = '';
			endif;
?>
		<tr class="fm-field<?=$errdisplay?>">
			<td class="td-left txt-left">
<?php
			if(isset($ref['generaloutcall']['id']) === true):
				$id = $ref['generaloutcall']['id'];
			else:
				$id = '';
			endif;

			echo $form->hidden(array('name' => 'special[id][]','value' => $id));

			echo $form->select(array('field' => false,'name' => 'special[trunkfeaturesid][]','id' => false,'label' => false,'browse' => 'trunk','key' => 'name','altkey' => 'trunkfeaturesid','optgroup' => array('key' => true,'bbf' => array('concat','fm_special-trunk-opt-')),'value' => $ref['generaloutcall']['trunkfeaturesid'],'default' => $element['generaloutcall']['trunkfeaturesid']['default']),$trunks_list);
?>
			</td>
			<td><?=$form->select(array('field' => false,'name' => 'special[extenmode][]','key' => false,'id' => false,'label' => false,'bbf' => array('concatkey','fm_special-extenmode-opt-'),'value' => $ref['extenumbers']['extenmode'],'default' => $element['extenumbers']['extenmode']['default']),$element['extenumbers']['extenmode']['value'],'onchange="xivo_extenmode(this);"');?></td>
			<td>
				<div<?=$extendisplay?>>
				<?=$form->text(array('field' => false,'name' => 'special[exten][]','id' => false,'label' => false,'size' => 15,'value' => $ref['extenumbers']['exten'],'default' => $element['extenumbers']['exten']['default']));?>
				</div>
				<div<?=$rangedisplay?>>
				<?=$form->text(array('field' => false,'name' => 'special[rangebeg][]','id' => false,'label' => false,'size' => 5,'value' => $ref['extenumbers']['rangebeg'],'default' => $element['extenumbers']['rangebeg']['default']));?>
				<?=$form->text(array('field' => false,'name' => 'special[rangeend][]','id' => false,'label' => false,'size' => 5,'value' => $ref['extenumbers']['rangeend'],'default' => $element['extenumbers']['rangeend']['default']));?>
				</div>
			</td>
			
			<td class="td-right txt-right"><?=$url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_emergency-delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'special\',this,1);"',$this->bbf('opt_emergency-delete'));?></td>
		</tr>

<?php
		endfor;
	endif;
?>
		</tbody>
		<tfoot>
		<tr id="no-special"<?=($special_nb !== 0 ? ' class="b-nodisplay"' : '')?>>
			<td colspan="4" class="td-single"><?=$this->bbf('no_special');?></td>
		</tr>
		</tfoot>
	</table>
	<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
		<tbody id="ex-special">
		<tr class="fm-field">
			<td class="td-left txt-left"><?=$form->select(array('field' => false,'name' => 'special[trunkfeaturesid][]','id' => false,'label' => false,'browse' => 'trunk','key' => 'name','altkey' => 'trunkfeaturesid','optgroup' => array('key' => true,'bbf' => array('concat','fm_special-trunk-opt-')),'default' => $element['generaloutcall']['trunkfeaturesid']['default']),$trunks_list,'disabled="disabled"');?></td>
			<td><?=$form->select(array('field' => false,'name' => 'special[extenmode][]','key' => false,'id' => false,'label' => false,'bbf' => array('concatkey','fm_special-extenmode-opt-'),'default' => $element['extenumbers']['extenmode']['default']),$element['extenumbers']['extenmode']['value'],'onchange="xivo_extenmode(this);" disabled="disabled"');?></td>
			<td>
				<div<?=($element['extenumbers']['extenmode']['default'] === 'extension' ? '' : ' class="b-nodisplay"')?>>
				<?=$form->text(array('field' => false,'name' => 'special[exten][]','id' => false,'label' => false,'size' => 15,'default' => $element['extenumbers']['exten']['default']),'disabled="disabled"');?>
				</div>
				<div<?=($element['extenumbers']['extenmode']['default'] !== 'extension' ? '' : ' class="b-nodisplay"')?>>
				<?=$form->text(array('field' => false,'name' => 'special[rangebeg][]','id' => false,'label' => false,'size' => 5,'default' => $element['extenumbers']['rangebeg']['default']),'disabled="disabled"');?>
				<?=$form->text(array('field' => false,'name' => 'special[rangeend][]','id' => false,'label' => false,'size' => 5,'default' => $element['extenumbers']['rangeend']['default']),'disabled="disabled"');?>
				</div>
			</td>
			
			<td class="td-right txt-right"><?=$url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_special-delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'special\',this,1);"',$this->bbf('opt_special-delete'));?></td>
		</tr>
		</tbody>
	</table>
</div>

</div>

<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>

</form>
<?php
	endif;
?>

	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
