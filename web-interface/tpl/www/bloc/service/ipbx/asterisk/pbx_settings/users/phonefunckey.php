<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$element = $this->get_var('element');
	$list = $this->get_var('destination_list');
	$error = $this->get_var('error');
	$info = $this->get_var('info');
	$fktype_list = $this->get_var('fktype_list');
	$bsfilter_list = $this->get_var('bsfilter_list');

	if(empty($info['phonefunckey']) === false):
		$nb = count($info['phonefunckey']);
	else:
		$nb = 0;
	endif;

	$phonefunckey_js = array();
	$phonefunckey_js[] = 'xivo_tlist[\'phonefunckey\'] = new Array();';
	$phonefunckey_js[] = 'xivo_tlist[\'phonefunckey\'][\'cnt\'] = '.$nb.';';

	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js($phonefunckey_js);
?>
<div class="sb-list">
<table cellspacing="0" cellpadding="0" border="0">
	<thead>
	<tr class="sb-top">
		<th class="th-left"><?=$this->bbf('col_phonefunckey-fknum');?></th>
		<th class="th-center"><?=$this->bbf('col_phonefunckey-type');?></th>
		<th class="th-center"><?=$this->bbf('col_phonefunckey-typeval');?></th>
		<th class="th-center"><?=$this->bbf('col_phonefunckey-supervision');?></th>
		<th class="th-right"><?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',$this->bbf('col_phonefunckey-add'),'border="0"'),'#',null,'onclick="xivo_table_list(\'phonefunckey\',this,0,true); xivo_chgphonefunckey(xivo_eid(\'it-phonefunckey-type-\'+xivo_tlist[\'phonefunckey\'][\'idcnt\'])); return(false);"',$this->bbf('col_phonefunckey-add'));?></th>
	</tr>
	</thead>
	<tbody id="phonefunckey">
<?php
if($nb !== 0):
	for($i = 0;$i < $nb;$i++):
		$ref = &$info['phonefunckey'][$i];

		if(isset($error['phonefunckey'][$i]) === true):
			$errdisplay = ' l-infos-error';
		else:
			$errdisplay = '';
		endif;
?>
	<tr class="fm-field<?=$errdisplay?>">
		<td class="td-left txt-center"><?=$form->select(array('field' => false,'name' => 'phonefunckey[fknum][]','id' => false,'label' => false,'key' => false,'default' => $element['phonefunckey']['fknum']['default'],'value' => $ref['fknum']),$element['phonefunckey']['fknum']['value']);?></td>
		<td><?=$form->select(array('field' => false,'name' => 'phonefunckey[type][]','id' => 'it-phonefunckey-type-'.$i,'label' => false,'key' => false,'bbf' => array('concatkey','fm_phonefunckey_type-opt-'),'default' => $element['phonefunckey']['type']['default'],'value' => $ref['type']),$element['phonefunckey']['type']['value'],'onchange="xivo_chgphonefunckey(this);"');?></td>
		<td>
<?php	
if(empty($list['users']) === false):

	if($ref['type'] === 'user'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-user-typeval-'.$i,'label' => false,'key' => 'identity','altkey' => 'id','invalid' => $invalid,'value' => $ref['user']),$list['users']);

else:

	echo $url->href_html($this->bbf('create_user'),'service/ipbx/pbx_settings/users','act=add','id="fd-phonefunckey-user-typeval-'.$i.'"');

endif;

if(empty($list['groups']) === false):

	if($ref['type'] === 'group'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-group-typeval-'.$i,'label' => false,'key' => 'identity','altkey' => 'id','invalid' => $invalid,'value' => $ref['group']),$list['groups']);

else:

	echo $url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add','id="fd-phonefunckey-group-typeval-'.$i.'"');

endif;

if(empty($list['queues']) === false):

	if($ref['type'] === 'queue'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-queue-typeval-'.$i,'label' => false,'key' => 'identity','altkey' => 'id','invalid' => $invalid,'value' => $ref['queue']),$list['queues']);

else:

	echo $url->href_html($this->bbf('create_queue'),'service/ipbx/pbx_settings/queues','act=add','id="fd-phonefunckey-queue-typeval-'.$i.'"');

endif;

if(empty($list['meetme']) === false):

	if($ref['type'] === 'meetme'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-meetme-typeval-'.$i,'label' => false,'key' => 'identity','altkey' => 'id','invalid' => $invalid,'value' => $ref['meetme']),$list['meetme']);

else:

	echo $url->href_html($this->bbf('create_meetme'),'service/ipbx/pbx_settings/meetme','act=add','id="fd-phonefunckey-meetme-typeval-'.$i.'"');

endif;

if($fktype_list !== false && isset($fktype_list['extension']) === true && empty($fktype_list['extension']) === false):

	if($ref['type'] === 'extension'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-extension-typeval-'.$i,'label' => false,'key' => true,'bbf' => array('concatkey','fm_phonefunckey_extension-typeval-opt-'),'invalid' => $invalid,'value' => $ref['extension']),$fktype_list['extension']);

endif;

if(empty($bsfilter_list) === false):

	if($ref['type'] === 'bosssecretary'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-bosssecretary-typeval-'.$i,'label' => false,'key' => 'callfilteridentity','altkey' => 'id','invalid' => $invalid,'value' => $ref['bosssecretary']),$bsfilter_list);

else:

	echo $url->href_html($this->bbf('create_callfilter'),'service/ipbx/call_management/callfilter','act=add','id="fd-phonefunckey-bosssecretary-typeval-'.$i.'"');
	
endif;

	echo $form->text(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-custom-typeval-'.$i,'label' => false,'size' => 15,'value' => $ref['custom']));

?>
		</td>
		<td><?=$form->select(array('field' => false,'name' => 'phonefunckey[supervision][]','id' => 'it-phonefunckey-supervision-'.$i,'label' => false,'bbf' => array('concatkey','fm_phonefunckey_supervision-opt-'),'default' => $element['phonefunckey']['supervision']['default'],'value' => $ref['supervision']),$element['phonefunckey']['supervision']['value'],'onfocus="(this.className != xivo_fm_disabled_class ? xivo_fm_set_onfocus(this) : false)" onblur="(this.className != xivo_fm_disabled_class ? xivo_fm_set_onblur(this) : false)"');?></td>
		<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',$this->bbf('opt_phonefunckey-delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'phonefunckey\',this,1); return(false);"',$this->bbf('opt_phonefunckey-delete'));?></td>
	</tr>
<?php
	endfor;
endif;
?>
	</tbody>
	<tfoot>
	<tr id="no-phonefunckey"<?=($nb !== 0 ? ' class="b-nodisplay"' : '')?>>
		<td colspan="5" class="td-single"><?=$this->bbf('no_phonefunckey');?></td>
	</tr>
	</tfoot>
</table>
<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
	<tbody id="ex-phonefunckey">
	<tr class="fm-field">
		<td class="td-left txt-center"><?=$form->select(array('field' => false,'name' => 'phonefunckey[fknum][]','id' => false,'label' => false,'key' => false,'default' => $element['phonefunckey']['fknum']['default']),$element['phonefunckey']['fknum']['value'],'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');?></td>
		<td><?=$form->select(array('field' => false,'name' => 'phonefunckey[type][]','id' => 'it-phonefunckey-type','label' => false,'key' => false,'bbf' => array('concatkey','fm_phonefunckey_type-opt-'),'default' => $element['phonefunckey']['type']['default']),$element['phonefunckey']['type']['value'],'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);" onchange="xivo_chgphonefunckey(this);"');?></td>
		<td>
<?php	
if(empty($list['users']) === false):

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-user-typeval','label' => false,'key' => 'identity','altkey' => 'id'),$list['users'],'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');

else:

	echo $url->href_html($this->bbf('create_user'),'service/ipbx/pbx_settings/users','act=add','id="fd-phonefunckey-user-typeval" style="display: none;"');

endif;

if(empty($list['groups']) === false):

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-group-typeval','label' => false,'key' => 'identity','altkey' => 'id'),$list['groups'],'disabled="disabled" style="display: none;" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');

else:

	echo $url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add','id="fd-phonefunckey-group-typeval" style="display: none;"');

endif;

if(empty($list['queues']) === false):

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-queue-typeval','label' => false,'key' => 'identity','altkey' => 'id'),$list['queues'],'disabled="disabled" style="display: none;" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');

else:

	echo $url->href_html($this->bbf('create_queue'),'service/ipbx/pbx_settings/queues','act=add','id="fd-phonefunckey-queue-typeval" style="display: none;"');

endif;

if(empty($list['meetme']) === false):

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-meetme-typeval','label' => false,'key' => 'identity','altkey' => 'id'),$list['meetme'],'disabled="disabled" style="display: none;" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');

else:

	echo $url->href_html($this->bbf('create_meetme'),'service/ipbx/pbx_settings/meetme','act=add','id="fd-phonefunckey-meetme-typeval" style="display: none;"');

endif;

if($fktype_list !== false && isset($fktype_list['extension']) === true && empty($fktype_list['extension']) === false):

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-extension-typeval','label' => false,'key' => true,'bbf' => array('concatkey','fm_phonefunckey_extension-typeval-opt-')),$fktype_list['extension'],'disabled="disabled" style="display: none;" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');

endif;

if(empty($bsfilter_list) === false):

	echo $form->select(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-bosssecretary-typeval','label' => false,'key' => 'callfilteridentity','altkey' => 'id'),$bsfilter_list,'disabled="disabled" style="display: none;" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');

else:

	echo $url->href_html($this->bbf('create_callfilter'),'service/ipbx/call_management/callfilter','act=add','id="fd-phonefunckey-bosssecretary-typeval" style="display: none;"');
	
endif;

	echo $form->text(array('field' => false,'name' => 'phonefunckey[typeval][]','id' => 'it-phonefunckey-custom-typeval','label' => false,'size' => 15),'disabled="disabled" style="display: none;" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');

?>
		</td>
		<td><?=$form->select(array('field' => false,'name' => 'phonefunckey[supervision][]','id' => 'it-phonefunckey-supervision','label' => false,'bbf' => array('concatkey','fm_phonefunckey_supervision-opt-'),'default' => $element['phonefunckey']['supervision']['default']),$element['phonefunckey']['supervision']['value'],'disabled="disabled" onfocus="(this.className != xivo_fm_disabled_class ? xivo_fm_set_onfocus(this) : false)" onblur="(this.className != xivo_fm_disabled_class ? xivo_fm_set_onblur(this) : false)"');?></td>
		<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',$this->bbf('opt_phonefunckey-delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'phonefunckey\',this,1); return(false);"',$this->bbf('opt_phonefunckey-delete'));?></td>
	</tr>
	</tbody>
</table>
</div>
