<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$element = $this->get_var('element','meetmeguest');

$info = $this->get_var('info','meetmeguest');
$error = $this->get_var('error','meetmeguest');

if(empty($info) === false):
	$nb = count($info);
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('dwho.dom.set_table_list(\'meetmeguest\','.$nb.');');
else:
	$nb = 0;
endif;

?>
<div class="sb-list">
	<table cellspacing="0" cellpadding="0" border="0">
		<thead>
		<tr class="sb-top">
			<th class="th-left"><?=$this->bbf('col_meetmeguest-fullname');?></th>
			<th class="th-center"><?=$this->bbf('col_meetmeguest-telephonenumber');?></th>
			<th class="th-center"><?=$this->bbf('col_meetmeguest-email');?></th>
			<th class="th-center"><?=$this->bbf('col_meetmeguest-sendemail');?></th>
			<th class="th-right">
				<?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
								  $this->bbf('col_meetmeguest-add'),
								  'border="0"'),
						   '#',
						   null,
						   'onclick="dwho.dom.make_table_list(\'meetmeguest\',this); return(dwho.dom.free_focus());"',
						   $this->bbf('col_meetmeguest-add'));?>
			</th>
		</tr>
		</thead>
		<tbody id="meetmeguest">
<?php

if($nb > 0):
	for($i = 0;$i < $nb;$i++):
		$ref = $info[$i];

		if(isset($error[$i]) === true):
			$errdisplay = ' l-infos-error';
		else:
			$errdisplay = '';
		endif;
?>
		<tr class="fm-paragraph<?=$errdisplay?>">
			<td class="td-left">
				<?=$form->text(array('paragraph'	=> false,
						     'name'		=> 'meetmeguest[fullname][]',
						     'id'		=> false,
						     'label'		=> false,
						     'size'		=> 20,
						     'value'		=> $ref['fullname'],
						     'default'		=> $element['fullname']['default']));?>
			</td>
			<td>
				<?=$form->text(array('paragraph'	=> false,
						     'name'		=> 'meetmeguest[telephonenumber][]',
						     'id'		=> false,
						     'label'		=> false,
						     'size'		=> 10,
						     'value'		=> $ref['telephonenumber'],
						     'default'		=> $element['telephonenumber']['default']));?>
			</td>
			<td>
				<?=$form->text(array('paragraph'	=> false,
						     'name'		=> 'meetmeguest[email][]',
						     'id'		=> false,
						     'label'		=> false,
						     'size'		=> 25,
						     'value'		=> $ref['email'],
						     'default'		=> $element['email']['default']));?>
			</td>
			<td>
				<?=$form->select(array('paragraph'	=> false,
						       'name'		=> 'meetmeguest[sendemail][]',
						       'id'		=> false,
						       'label'		=> false,
						       'key'		=> false,
						       'bbf'		=> 'fm_bool-opt',
						       'bbfopt'		=> array('argmode'	=> 'paramvalue'),
						       'selected'	=> (isset($ref['sendemail']) === true ? $ref['sendemail'] : null),
						       'default'	=> $element['sendemail']['default']),
						 $element['sendemail']['value']);?>
			</td>
			<td class="td-right">
				<?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
								  $this->bbf('opt_delete'),
								  'border="0"'),
						   '#',
						   null,
						   'onclick="dwho.dom.make_table_list(\'meetmeguest\',this,1); return(dwho.dom.free_focus());"',
						   $this->bbf('opt_delete'));?>
			</td>
		</tr>
<?php
		endfor;
	endif;
?>
		</tbody>
		<tfoot>
		<tr id="no-meetmeguest"<?=($nb > 0 ? ' class="b-nodisplay"' : '')?>>
			<td colspan="5" class="td-single"><?=$this->bbf('no_meetmeguest');?></td>
		</tr>
		</tfoot>
	</table>
	<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
		<tbody id="ex-meetmeguest">
		<tr class="fm-paragraph">
			<td class="td-left">
				<?=$form->text(array('paragraph'	=> false,
						     'name'		=> 'meetmeguest[fullname][]',
						     'id'		=> false,
						     'label'		=> false,
						     'size'		=> 20,
						     'disabled'		=> true,
						     'default'		=> $element['fullname']['default']));?>
			</td>
			<td>
				<?=$form->text(array('paragraph'	=> false,
						     'name'		=> 'meetmeguest[telephonenumber][]',
						     'id'		=> false,
						     'label'		=> false,
						     'size'		=> 10,
						     'disabled'		=> true,
						     'default'		=> $element['telephonenumber']['default']));?>
			</td>
			<td>
				<?=$form->text(array('paragraph'	=> false,
						     'name'		=> 'meetmeguest[email][]',
						     'id'		=> false,
						     'label'		=> false,
						     'size'		=> 25,
						     'disabled'		=> true,
						     'default'		=> $element['email']['default']));?>
			</td>
			<td>
				<?=$form->select(array('paragraph'	=> false,
						       'name'		=> 'meetmeguest[sendemail][]',
						       'id'		=> false,
						       'label'		=> false,
						       'key'		=> false,
						       'bbf'		=> 'fm_bool-opt',
						       'bbfopt'		=> array('argmode'	=> 'paramvalue'),
						       'disabled'	=> true,
						       'default'	=> $element['sendemail']['default']),
						 $element['sendemail']['value']);?>
			</td>
			<td class="td-right">
				<?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
								  $this->bbf('opt_delete'),
								  'border="0"'),
						   '#',
						   null,
						   'onclick="dwho.dom.make_table_list(\'meetmeguest\',this,1); return(dwho.dom.free_focus());"',
						   $this->bbf('opt_delete'));?>
			</td>
		</tr>
		</tbody>
	</table>
</div>
