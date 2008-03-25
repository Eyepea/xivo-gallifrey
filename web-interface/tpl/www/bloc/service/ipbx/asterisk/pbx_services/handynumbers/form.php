<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$type = $this->get_var('type');
	$trunkslist = $this->get_var('trunkslist');
	$count = $this->get_var('count');
	$element = $this->get_var('element');
	$error = $this->get_var('error');
	$info = $this->get_var('info');
?>
<table cellspacing="0" cellpadding="0" border="0">
	<thead>
	<tr class="sb-top">
		<th class="th-left"><?=$this->bbf('col_'.$type.'-trunk');?></th>
		<th class="th-center"><?=$this->bbf('col_'.$type.'-exten');?></th>
		<th class="th-right"><?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',$this->bbf('col_'.$type.'-add'),'border="0"'),'#',null,'onclick="xivo_table_list(\''.$type.'\',this); return(false);"',$this->bbf('col_'.$type.'-add'));?></th>
	</tr>
	</thead>
	<tbody id="<?=$type?>">
<?php
if($count !== 0):
	for($i = 0;$i < $count;$i++):
		$ref = &$info[$type][$i];

		if(isset($error[$type][$i]) === true):
			$errdisplay = ' l-infos-error';
		else:
			$errdisplay = '';
		endif;
?>
	<tr class="fm-field<?=$errdisplay?>">
		<td class="td-left txt-center"><?=$form->select(array('field' => false,'name' => $type.'[trunkfeaturesid][]','id' => false,'label' => false,'browse' => 'trunk','key' => 'name','altkey' => 'trunkfeaturesid','invalid' => true,'optgroup' => array('key' => true,'bbf' => array('concat','fm_'.$type.'-trunk-opt-')),'value' => $ref['trunkfeaturesid'],'default' => $element['handynumbers']['trunkfeaturesid']['default']),$trunkslist);?></td>
		<td><?=$form->text(array('field' => false,'name' => $type.'[exten][]','id' => false,'label' => false,'size' => 15,'value' => $ref['exten'],'default' => $element['handynumbers']['exten']['default']));?></td>
		<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',$this->bbf('opt_'.$type.'-delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\''.$type.'\',this,1); return(false);"',$this->bbf('opt_'.$type.'-delete'));?></td>
	</tr>

<?php
	endfor;
endif;
?>
	</tbody>
	<tfoot>
	<tr id="no-<?=$type?>"<?=($count !== 0 ? ' class="b-nodisplay"' : '')?>>
		<td colspan="3" class="td-single"><?=$this->bbf('no_'.$type);?></td>
	</tr>
	</tfoot>
</table>
<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
	<tbody id="ex-<?=$type?>">
	<tr class="fm-field">
		<td class="td-left txt-center"><?=$form->select(array('field' => false,'name' => $type.'[trunkfeaturesid][]','id' => false,'label' => false,'browse' => 'trunk','key' => 'name','altkey' => 'trunkfeaturesid','optgroup' => array('key' => true,'bbf' => array('concat','fm_'.$type.'-trunk-opt-')),'default' => $element['handynumbers']['trunkfeaturesid']['default']),$trunkslist,'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');?></td>
		<td><?=$form->text(array('field' => false,'name' => $type.'[exten][]','id' => false,'label' => false,'size' => 15,'default' => $element['handynumbers']['exten']['default']),'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');?></td>
		<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',$this->bbf('opt_'.$type.'-delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\''.$type.'\',this,1); return(false);"',$this->bbf('opt_'.$type.'-delete'));?></td>
	</tr>
	</tbody>
</table>
