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

// queueskill category name
$element 	= $this->get_var('element');

// queueskill values (if edit mode or redisplay view after error)
$data 		= $this->get_var('data');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	// skills category name -- HTML field
	echo	$form->text(array('desc'	=> $this->bbf('fm_queueskill_name'),
				  'name'	=> 'queueskill[name]',
				  'labelid'	=> 'queueskill-name',
				  'size'	=> 32,
				  'default'	=> $element['queueskill']['name']['default'],
				  'value'	=> $info['queueskill']['name'],
			          'error'	=> $this->bbf_args('queueskill-name',
							   $this->get_var('error', 'name'))));
	// *end*
?>

<?php
	$type = 'disp';
	$count = $data?count($data):0;
	$errdisplay = '';
?>
	<p>&nbsp;</p>
	<p><?=$this->bbf('fm_queueskill_items');?></p>
	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<thead>
			<tr class="sb-top">

				<th class="th-left"><?=$this->bbf('col_1');?></th>
				<th class="th-center"><?=$this->bbf('col_2');?></th>
				<th class="th-center"><?=$this->bbf('col_3');?></th>
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
					echo $form->hidden(array('name'		=> 'queueskill[values][id][]',
	                                             		 'value'     	=> $data[$i]['id']));

					echo $form->text(array('paragraph'	=> false,
							       'name'		=> 'queueskill[values][name][]',
							       'id'		=> false,
							       'label'		=> false,
							       'size'		=> 15,
							       'key'		=> false,
							       'value'		=> $data[$i]['name'],
							       'default'	=> '',
						               'error'		=> $this->bbf_args('queueskill-value-name', $this->get_var('error', 'values', $i, 'name'))));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
							       'name'		=> 'queueskill[values][desc][]',
							       'id'		=> false,
							       'label'		=> false,
							       'size'		=> 15,
							       'key'		=> false,
							       'value'		=> $data[$i]['description'],
							       'default'	=> '',
						               'error'		=> $this->bbf_args('queueskill-value-descr', $this->get_var('error', 'values', $i, 'description'))));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
							       'name'		=> 'queueskill[values][printscr][]',
							       'id'		=> false,
							       'label'		=> false,
							       'size'		=> 15,
							       'key'		=> false,
							       'value'		=> $data[$i]['printscreen'],
							       'default'	=> '',
						               'error'		=> $this->bbf_args('queueskill-value-printscr', $this->get_var('error', 'values', $i, 'printscreen'))));
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
					echo $form->hidden(array('name'		=> 'queueskill[values][id][]',
	                                             		 'default'     	=> '-1'));

					echo $form->text(array('paragraph'	=> false,
							       'name'		=> 'queueskill[values][name][]',
							       'id'		=> false,
							       'label'		=> false,
						    	       'size'		=> 15,
							       'key'		=> false,
							       'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
							       'name'		=> 'queueskill[values][desc][]',
							       'id'		=> false,
							       'label'		=> false,
							       'size'		=> 15,
							       'key'		=> false,
							       'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
							       'name'		=> 'queueskill[values][printscr][]',
							       'id'		=> false,
							       'label'		=> false,
							       'size'		=> 15,
							       'key'		=> false,
							       'default'	=> ''));
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

