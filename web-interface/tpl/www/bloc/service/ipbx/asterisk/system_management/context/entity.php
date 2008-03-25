<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$element = $this->get_var('element');
	$type = $this->get_var('type');
	$list = $this->get_var('list');
	$nb = $this->get_var('count');
	$err = $this->get_varra('error',array('contextentity',$type));
?>
<table cellspacing="0" cellpadding="0" border="0">
	<thead>
	<tr class="sb-top">
		<th class="th-left"><?=$this->bbf('col_contextentity_'.$type.'-typevalbeg');?></th>
		<th class="th-center"><?=$this->bbf('col_contextentity_'.$type.'-typevalend');?></th>
		<th class="th-right"><?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',$this->bbf('col_contextentity_'.$type.'-add'),'border="0"'),'#',null,'onclick="xivo_context_entity_enable_add(\''.$type.'\',this); return(false);"',$this->bbf('col_contextentity_'.$type.'-add'));?></th>
	</tr>
	</thead>
	<tbody id="contextentity-<?=$type?>">
<?php
if($list !== false):
	for($i = 0;$i < $nb;$i++):
		$ref = &$list[$i];

		if(isset($err[$i]) === true):
			$errdisplay = ' l-infos-error';
		else:
			$errdisplay = '';
		endif;
?>
	<tr class="fm-field<?=$errdisplay?>">
		<td class="td-left txt-center">
			<?=$form->text(array('field'	=> false,
					     'name'	=> 'contextentity['.$type.'][typevalbeg][]',
					     'id'	=> false,
					     'label'	=> false,
					     'size'	=> 15,
					     'value'	=> $ref['typevalbeg'],
					     'default'	=> $element['contextentity']['typevalbeg']['default']));?>
		</td>
		<td>
			<?=$form->text(array('field'	=> false,
					     'name'	=> 'contextentity['.$type.'][typevalend][]',
					     'id'	=> false,
					     'label'	=> false,
					     'size'	=> 15,
					     'value'	=> $ref['typevalend'],
					     'default'	=> $element['contextentity']['typevalend']['default']));?>
		</td>
		<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',$this->bbf('opt_contextentity_'.$type.'-delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'contextentity-'.$type.'\',this,1); return(false);"',$this->bbf('opt_contextentity_'.$type.'-delete'));?></td>
	</tr>
<?php
	endfor;
endif;
?>
	</tbody>
	<tfoot>
	<tr id="no-contextentity-<?=$type?>"<?=($list !== false ? ' class="b-nodisplay"' : '')?>>
		<td colspan="3" class="td-single"><?=$this->bbf('no_contextentity-'.$type);?></td>
	</tr>
	</tfoot>
</table>
<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
	<tbody id="ex-contextentity-<?=$type?>">
	<tr class="fm-field">
		<td class="td-left txt-center">
			<?=$form->text(array('field'	=> false,
					     'name'	=> 'contextentity['.$type.'][typevalbeg][]',
					     'id'	=> false,
					     'label'	=> false,
					     'size'	=> 15,
					     'default'	=> $element['contextentity']['typevalbeg']['default']),
				       'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');?>
		</td>
		<td>
			<?=$form->text(array('field'	=> false,
					     'name'	=> 'contextentity['.$type.'][typevalend][]',
					     'id'	=> false,
					     'label'	=> false,
					     'size'	=> 15,
					     'default'	=> $element['contextentity']['typevalend']['default']),
				       'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');?>
		</td>
		<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',$this->bbf('opt_contextentity_'.$type.'-delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'contextentity-'.$type.'\',this,1); return(false);"',$this->bbf('opt_contextentity_'.$type.'-delete'));?></td>
	</tr>
	</tbody>
</table>
