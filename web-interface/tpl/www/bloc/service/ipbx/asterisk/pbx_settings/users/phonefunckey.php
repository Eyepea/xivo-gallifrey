<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$count = $this->get_var('count');
	$element = $this->get_var('element');
	$list = $this->get_var('list');
	$error = $this->get_var('error');
	$info = $this->get_var('info');
	$fktype_list = $this->get_var('fktype_list');

	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('var xivo_phonefunckey_nb = 0;');
?>
<div class="sb-list">
<table cellspacing="0" cellpadding="0" border="0">
	<thead>
	<tr class="sb-top">
		<th class="th-left"><?=$this->bbf('col_phonefunckey-fknum');?></th>
		<th class="th-center"><?=$this->bbf('col_phonefunckey-type');?></th>
		<th class="th-center"><?=$this->bbf('col_phonefunckey-typeval');?></th>
		<th class="th-center"><?=$this->bbf('col_phonefunckey-supervision');?></th>
		<th class="th-right"><?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',$this->bbf('col_phonefunckey-add'),'border="0"'),'#',null,'onclick="xivo_table_list(\'phonefunckey\',this,0,++xivo_phonefunckey_nb); return(false);"',$this->bbf('col_phonefunckey-add'));?></th>
	</tr>
	</thead>
	<tbody id="phonefunckey">
<?php
if($count !== 0):
	for($i = 0;$i < $count;$i++):
		$ref = &$info['phonefunckey'][$i];

		if(isset($error['phonefunckey'][$i]) === true):
			$errdisplay = ' l-infos-error';
		else:
			$errdisplay = '';
		endif;
?>
	<tr class="fm-field<?=$errdisplay?>">
		<td class="td-left txt-center"><?=$form->select(array('field' => false,'name' => 'phonefunckey[fknum][]','id' => false,'label' => false,'key' => false,'default' => $element['phonefunckey']['fknum']['default'],'value' => $ref['fknum']),$element['phonefunckey']['fknum']['value']);?></td>
		<td><?=$form->select(array('field' => false,'name' => 'phonefunckey[type][]','id' => false,'label' => false,'key' => false,'default' => $element['phonefunckey']['type']['default'],'value' => $ref['type']),$element['phonefunckey']['type']['value']);?></td>
		<td>typeval
		</td>
		<td><?=$form->checkbox(array('field' => false,'name' => 'phonefunckey[supervision][]','id' => false,'label' => false,'default' => $element['phonefunckey']['supervision']['default'],'checked' => $ref['supervision']));?></td>
		<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',$this->bbf('opt_phonefunckey-delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'phonefunckey\',this,1); return(false);"',$this->bbf('opt_phonefunckey-delete'));?></td>
	</tr>

<?php
	endfor;
endif;
?>
	</tbody>
	<tfoot>
	<tr id="no-phonefunckey"<?=($count !== 0 ? ' class="b-nodisplay"' : '')?>>
		<td colspan="5" class="td-single"><?=$this->bbf('no_phonefunckey');?></td>
	</tr>
	</tfoot>
</table>
<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
	<tbody id="ex-phonefunckey">
	<tr class="fm-field">
		<td class="td-left txt-center"><?=$form->select(array('field' => false,'name' => 'phonefunckey[fknum][]','id' => false,'label' => false,'key' => false,'default' => $element['phonefunckey']['fknum']['default']),$element['phonefunckey']['fknum']['value'],'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');?></td>
		<td><?=$form->select(array('field' => false,'name' => 'phonefunckey[type][]','id' => 'it-phonefunckey-type','label' => false,'key' => false,'default' => $element['phonefunckey']['type']['default']),$element['phonefunckey']['type']['value'],'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);" onchange="xivo_chgphonefunckey(this);"');?></td>
		<td>
<?php	
if(empty($list['users']) === false):

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-user-typeval','label' => false,'key' => 'identity','altkey' => 'id','value' => $this->get_varra('phonefunckey','user')),$list['users']);

else:

	echo $url->href_html($this->bbf('create_user'),'service/ipbx/pbx_settings/users','act=add','id="fd-phonefunckey-user-typeval" style="display: none;"');

endif;

if(empty($list['groups']) === false):

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-group-typeval','label' => false,'key' => 'identity','altkey' => 'id','value' => $this->get_varra('phonefunckey','group')),$list['groups']);

else:

	echo $url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add','id="fd-phonefunckey-group-typeval" style="display: none;"');

endif;

if(empty($list['queues']) === false):

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-queue-typeval','label' => false,'key' => 'identity','altkey' => 'id','value' => $this->get_varra('phonefunckey','queue')),$list['queues']);

else:

	echo $url->href_html($this->bbf('create_queue'),'service/ipbx/pbx_settings/queues','act=add','id="fd-phonefunckey-queue-typeval" style="display: none;"');

endif;

if(empty($list['meetme']) === false):

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-meetme-typeval','label' => false,'key' => 'identity','altkey' => 'id','value' => $this->get_varra('phonefunckey','meetme')),$list['meetme']);

else:

	echo $url->href_html($this->bbf('create_meetme'),'service/ipbx/pbx_settings/meetme','act=add','id="fd-phonefunckey-meetme-typeval" style="display: none;"');

endif;

if(isset($fktype_list,$fktype_list['extension']) === true || empty($fktype_list['extension']) === false):

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-extension-typeval','label' => false,'key' => true,'value' => $this->get_varra('phonefunckey','extension')),$fktype_list['extension'],'style="display: none;"');

endif;

	echo $form->text(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-custom-typeval','label' => false,'size' => 15,'value' => $this->get_varra('phonefunckey','custom')),'style="display: none;"');

?>
</td>
		<td><?=$form->checkbox(array('field' => false,'name' => 'phonefunckey[supervision][]','id' => 'it-phonefunckey-supervision','label' => false,'default' => $element['phonefunckey']['supervision']['default']),'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');?></td>
		<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',$this->bbf('opt_phonefunckey-delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'phonefunckey\',this,1); return(false);"',$this->bbf('opt_phonefunckey-delete'));?></td>
	</tr>
	</tbody>
</table>
</div>
