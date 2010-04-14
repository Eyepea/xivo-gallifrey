<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

$form = &$this->get_module('form');
$url  = &$this->get_module('url');

$info 		= $this->get_var('info');

// queueskillrule name & rule
$element 	= $this->get_var('element');
$rules	 	= $this->get_var('rules');


if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_queueskillrule_name'),
				  'name'	=> 'queueskillrule[name]',
				  'labelid'	=> 'queueskillrule-name',
				  'size'	=> 32,
				  'default'	=> $element['queueskillrule']['name']['default'],
				  'value'	=> $info['queueskillrule']['name'],
				  'error'	=> $this->bbf_args('queueskillrule-name',
								   $this->get_var('error','name'))
		))
?>

<?php
	$type = 'disp';
	$count = $rules?count($rules):0;
	$errdisplay = '';
?>
	<p>&nbsp;</p>
<?php
	$rules_err = $this->get_var('error',"all-rules");
	if($rules_err != false)
	{
		$msg = $this->bbf_args('queueskillrule-allrules', $rules_err);
?>
	<p>
		<a href="#" class="fm-error-icon" onclick="return(false);" 
			onfocus="dwho.form.create_error_div(this, '<?=$msg?>');"
			onblur="dwho.form.destroy_error_div();" 
			onmouseover="dwho.form.create_error_div(this,'<?=$msg?>');" 	
			onmouseout="dwho.form.destroy_error_div();">&nbsp;
		</a></span>
	</p>

<?php	} ?>

	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<thead>
			<tr class="sb-top">

				<th class="th-left"><?=$this->bbf('col_1');?></th>
				<th class="th-right th-rule">
					<?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
									  $this->bbf('col_add'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\'disp\',this); return(dwho.dom.free_focus());"',
							   $this->bbf('col_add'));?>
				</th>
			</tr>
			</thead>
			<tbody id="disp">
		<?php
		if($count > 0):
			for($i = 0;$i < $count;$i++):

		?>
			<tr class="fm-paragraph<?=$errdisplay?>">
				<td class="td-left">
	<?php
					echo $form->textarea(array('paragraph'	=> false,
						 'label'	=> false,
						 'name'		=> 'queueskillrule[rule][]',
						 'id'		=> 'it-queueskillrule-rule',
						 'cols'		=> 80,
						 'rows'		=> 3,
						 'notag'    => true,
						 'default'	=> $element['queuekillrule']['rule'][$i],
					         'error'	=> $this->bbf_args('queueskillrule-rule',
								   $this->get_var('error',"rule-$i"))),
						 $rules[$i]);

	 ?>
				</td>
				<td class="td-right">
					<?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
									  $this->bbf('opt_'.$type.'-delete'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this,1); return(dwho.dom.free_focus());"',
							   $this->bbf('opt_'.$type.'-delete'));?>
				</td>
			</tr>

		<?php
			endfor;
		endif;
		?>
			</tbody>
			<tfoot>
			<tr id="no-<?=$type?>"<?=($count > 0 ? ' class="b-nodisplay"' : '')?>>
				<td colspan="5" class="td-single"><?=$this->bbf('no_'.$type);?></td>
			</tr>
			</tfoot>
		</table>
		<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
			<tbody id="ex-<?=$type?>">
			<tr class="fm-paragraph">
				<td class="td-left">
	<?php
					echo $form->textarea(array('paragraph'	=> false,
						 'label'	=> false,
						 'name'		=> 'queueskillrule[rule][]',
						 'id'		=> 'it-queueskillrule-rule',
						 'cols'		=> 80,
						 'rows'		=> 3));
	 ?>
				</td>
				<td class="td-right">
					<?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
									  $this->bbf('opt_'.$type.'-delete'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this,1); return(dwho.dom.free_focus());"',
							   $this->bbf('opt_'.$type.'-delete'));?>
				</td>
			</tr>
			</tbody>
		</table>
	</div>
</div>
<br />

</div>

