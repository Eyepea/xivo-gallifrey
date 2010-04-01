<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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
$data 		= $this->get_var('queueskills');

?>

<div id="sb-list">
<?php
	$type = 'disp';
	$count = $data?count($data):0;
	$errdisplay = '';
?>
	<p>&nbsp;</p>
	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<thead>
			<tr class="sb-top">

				<th class="th-left"><?=$this->bbf('col_1');?></th>
				<th class="th-center"><?=$this->bbf('col_2');?></th>
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
					echo	$form->select(array(
							'name'		=> 'queueskill-skill[]',
							'id'		=> "it-queueskill-skill[$i]",
							'key'		=> 'name',
							'altkey'	=> 'id',
							'empty'		=> true,
							'optgroup'	=> array(
								'key'		=> 'category', 
								'unique' 	=> true,
							),
							'selected'	=> $data[$i]['skillid'],
							'error'      	=> $this->bbf_args	('error_fm_queueskill_skillid', $this->get_var('error', 'queueskills', $i, 'skillid'))
						),
						$element['queueskills']);
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'	=> 'queueskill-weight[]',
								   'id'		=> false,
								   'label'	=> false,
								   'size'	=> 15,
								   'key'	=> false,
								   'default'	=> '0',
								   'value'	=> $data[$i]['weight'],
								   'error'      => $this->bbf_args	('error_fm_queueskill_weight', $this->get_var('error', 'queueskills', $i, 'weight'))));
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
					echo	$form->select(array(
							'name'		=> 'queueskill-skill[]',
							'id'		=> "it-queueskill-skill[]",
							'key'		=> 'name',
							'altkey'	=> 'id',
							'label'     => false,
							'empty'		=> true,
							'optgroup'	=> array(
								'key'		=> 'category', 
								'unique' 	=> true,
							),
						),
						$element['queueskills']);
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'	=> 'queueskill-weight[]',
								   'id'		=> false,
								   'label'	=> false,
								   'size'	=> 3,
								   'key'	=> false,
								   'default'	=> '0'));
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

