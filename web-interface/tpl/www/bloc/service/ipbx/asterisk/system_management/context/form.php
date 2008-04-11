<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->get_var('info');
	$element = $this->get_var('element');

	$user_nb = $group_nb = $queue_nb = $meetme_nb = $incall_nb = 0;
	$user_list = $group_list = $queue_list = $meetme_list = $incall_list = false;

	if(xivo_issa('contextentity',$info) === true):

		$context_js = array();

		if(xivo_issa('user',$info['contextentity']) === true
		&& ($user_nb = count($info['contextentity']['user'])) > 0):
			$user_list = $info['contextentity']['user'];
			$context_js[] = 'xivo_tlist[\'contextentity-user\'] = new Array();';
			$context_js[] = 'xivo_tlist[\'contextentity-user\'][\'cnt\'] = '.$user_nb.';';
		endif;

		if(xivo_issa('group',$info['contextentity']) === true
		&& ($group_nb = count($info['contextentity']['group'])) > 0):
			$group_list = $info['contextentity']['group'];
			$context_js[] = 'xivo_tlist[\'contextentity-group\'] = new Array();';
			$context_js[] = 'xivo_tlist[\'contextentity-group\'][\'cnt\'] = '.$group_nb.';';
		endif;

		if(xivo_issa('queue',$info['contextentity']) === true
		&& ($queue_nb = count($info['contextentity']['queue'])) > 0):
			$queue_list = $info['contextentity']['queue'];
			$context_js[] = 'xivo_tlist[\'contextentity-queue\'] = new Array();';
			$context_js[] = 'xivo_tlist[\'contextentity-queue\'][\'cnt\'] = '.$queue_nb.';';
		endif;

		if(xivo_issa('meetme',$info['contextentity']) === true
		&& ($meetme_nb = count($info['contextentity']['meetme'])) > 0):
			$meetme_list = $info['contextentity']['meetme'];
			$context_js[] = 'xivo_tlist[\'contextentity-meetme\'] = new Array();';
			$context_js[] = 'xivo_tlist[\'contextentity-meetme\'][\'cnt\'] = '.$meetme_nb.';';
		endif;

		if(xivo_issa('incall',$info['contextentity']) === true
		&& ($incall_nb = count($info['contextentity']['incall'])) > 0):
			$incall_list = $info['contextentity']['incall'];
			$context_js[] = 'xivo_tlist[\'contextentity-incall\'] = new Array();';
			$context_js[] = 'xivo_tlist[\'contextentity-incall\'][\'cnt\'] = '.$incall_nb.';';
		endif;

		if(isset($context_js[0]) === true):
			$dhtml = &$this->get_module('dhtml');
			$dhtml->write_js($context_js);
		endif;

	endif;

	$incall_err = $this->get_varra('error',array('contextentity','incall'));
?>

<div id="sb-part-first">

<?php

	echo $form->text(array('desc'		=> $this->bbf('fm_context_name'),
			       'name'		=> 'context[name]',
			       'labelid'	=> 'context-name',
			       'size'		=> 15,
			       'default'	=> $element['context']['name']['default'],
			       'value'		=> $info['context']['name']));

	echo $form->text(array('desc'		=> $this->bbf('fm_context_displayname'),
			       'name'		=> 'context[displayname]',
			       'labelid'	=> 'context-displayname',
			       'size'		=> 15,
			       'default'	=> $element['context']['displayname']['default'],
			       'value'		=> $info['context']['displayname']));

	if(($entities = $this->get_var('entities')) !== false):
		echo $form->select(array('desc'		=> $this->bbf('fm_context_entity'),
					 'name'		=> 'context[entity]',
					 'labelid'	=> 'context-entity',
					 'invalid'	=> ($this->get_var('act') === 'edit'),
					 'key'		=> 'displayname',
					 'altkey'	=> 'name',
					 'empty'	=> true,
					 'default'	=> $element['context']['entity']['default'],
					 'value'	=> $info['context']['entity']),$entities,'onchange="xivo_context_entity_status(this.form,this.value.length === 0);"');
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_entity'),'xivo/configuration/manage/entity','act=add'),
			'</div>';
	endif;
?>

<div class="fm-field fm-description">
	<p>
		<label id="lb-context-description" for="it-context-description"><?=$this->bbf('fm_context_description');?></label>
	</p>
	<?=$form->textarea(array('field'	=> false,
				 'label'	=> false,
				 'name'		=> 'context[description]',
				 'id'		=> 'it-context-description',
				 'cols'		=> 60,
				 'rows'		=> 5,
				 'default'	=> $element['context']['description']['default']),
			   $info['context']['description']);?>
</div>

</div>

<div id="sb-part-user" class="b-nodisplay">
	<div class="sb-list">
<?=$this->file_include('bloc/service/ipbx/asterisk/system_management/context/entity',array('type'	=> 'user',
											    'count'	=> $user_nb,
											    'list'	=> $user_list));?>
	</div>
</div>

<div id="sb-part-group" class="b-nodisplay">
	<div class="sb-list">
<?=$this->file_include('bloc/service/ipbx/asterisk/system_management/context/entity',array('type'	=> 'group',
											    'count'	=> $group_nb,
											    'list'	=> $group_list));?>
	</div>
</div>

<div id="sb-part-queue" class="b-nodisplay">
	<div class="sb-list">
<?=$this->file_include('bloc/service/ipbx/asterisk/system_management/context/entity',array('type'	=> 'queue',
											    'count'	=> $queue_nb,
											    'list'	=> $queue_list));?>
	</div>
</div>

<div id="sb-part-meetme" class="b-nodisplay">
	<div class="sb-list">
<?=$this->file_include('bloc/service/ipbx/asterisk/system_management/context/entity',array('type'	=> 'meetme',
											    'count'	=> $meetme_nb,
											    'list'	=> $meetme_list));?>
	</div>
</div>

<div id="sb-part-last" class="b-nodisplay">
	<div class="sb-list">
	<table cellspacing="0" cellpadding="0" border="0">
		<thead>
		<tr class="sb-top">
			<th class="th-left"><?=$this->bbf('col_contextentity_incall-typevalbeg');?></th>
			<th class="th-center"><?=$this->bbf('col_contextentity_incall-typevalend');?></th>
			<th class="th-center"><?=$this->bbf('col_contextentity_incall-didlength');?></th>
			<th class="th-right"><?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',$this->bbf('col_contextentity_incall-add'),'border="0"'),'#',null,'onclick="xivo_context_entity_enable_add(\'incall\',this); return(false);"',$this->bbf('col_contextentity_incall-add'));?></th>
		</tr>
		</thead>
		<tbody id="contextentity-incall">
<?php
if($incall_list !== false):
	for($i = 0;$i < $incall_nb;$i++):
		$ref = &$incall_list[$i];

		if(isset($incall_err[$i]) === true):
			$errdisplay = ' l-infos-error';
		else:
			$errdisplay = '';
		endif;
?>
		<tr class="fm-field<?=$errdisplay?>">
			<td class="td-left txt-center">
				<?=$form->text(array('field'	=> false,
						     'name'	=> 'contextentity[incall][typevalbeg][]',
						     'id'	=> false,
						     'label'	=> false,
						     'size'	=> 15,
						     'value'	=> $ref['typevalbeg'],
						     'default'	=> $element['contextentity']['typevalbeg']['default']));?>
			</td>
			<td>
				<?=$form->text(array('field'	=> false,
						     'name'	=> 'contextentity[incall][typevalend][]',
						     'id'	=> false,
						     'label'	=> false,
						     'size'	=> 15,
						     'value'	=> $ref['typevalend'],
						     'default'	=> $element['contextentity']['typevalend']['default']));?>
			</td>
			<td>
				<?=$form->select(array('field'		=> false,
						       'name'		=> 'contextentity[incall][didlength][]',
						       'id'		=> false,
						       'label'		=> false,
						       'key'		=> false,
						       'value'		=> $ref['didlength'],
						       'default'	=> $element['contextentity']['didlength']['default']),
					         $element['contextentity']['didlength']['value']);?>
			</td>
			<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',$this->bbf('opt_contextentity_incall-delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'contextentity-incall\',this,1); return(false);"',$this->bbf('opt_contextentity_incall-delete'));?></td>
		</tr>

<?php
	endfor;
endif;
?>
		</tbody>
		<tfoot>
		<tr id="no-contextentity-incall"<?=($incall_list !== false ? ' class="b-nodisplay"' : '')?>>
			<td colspan="4" class="td-single"><?=$this->bbf('no_contextentity-incall');?></td>
		</tr>
		</tfoot>
	</table>
	<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
		<tbody id="ex-contextentity-incall">
		<tr class="fm-field">
			<td class="td-left txt-center">
				<?=$form->text(array('field'	=> false,
						     'name'	=> 'contextentity[incall][typevalbeg][]',
						     'id'	=> false,
						     'label'	=> false,
						     'size'	=> 15,
						     'default'	=> $element['contextentity']['typevalend']['default']),
					       'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');?>
			</td>
			<td>
				<?=$form->text(array('field'	=> false,
						     'name'	=> 'contextentity[incall][typevalend][]',
						     'id'	=> false,
						     'label'	=> false,
						     'size'	=> 15,
						     'default'	=> $element['contextentity']['typevalend']['default']),
					       'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');?>
			</td>
			<td>
				<?=$form->select(array('field'		=> false,
						       'name'		=> 'contextentity[incall][didlength][]',
						       'id'		=> false,
						       'label'		=> false,
						       'key'		=> false,
						       'default'	=> $element['contextentity']['didlength']['default']),
						 $element['contextentity']['didlength']['value'],
						 'disabled="disabled" onfocus="xivo_fm_set_onfocus(this);" onblur="xivo_fm_set_onblur(this);"');?></td>
			<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',$this->bbf('opt_contextentity_incall-delete'),'border="0"'),'#',null,'onclick="xivo_table_list(\'contextentity-incall\',this,1); return(false);"',$this->bbf('opt_contextentity_incall-delete'));?></td>
		</tr>
		</tbody>
	</table>
	</div>
</div>
