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

$info     = $this->get_var('info');

$order    = $this->get_var('order_list');
$softkeys = $this->get_var('softkeys_list');
$softkey  = $this->get_var('softkey');

$type     = 'softkeys_'.$softkey;
$values   = $info['protocol']['softkeys'][$softkey];
$count    = count($values);
$errdisplay = '';

?>
	<?= $this->bbf('softkey_event', $softkey); ?>:
	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<tr class="sb-top">
				<th class="th-left"><?=$this->bbf('softkey_order');?></th>
				<th class="th-center"><?=$this->bbf('softkey_key');?></th>
				<th class="th-right th-rule">
					<?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
									  $this->bbf('col_add'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this); return(dwho.dom.free_focus());"',
							   $this->bbf('col_add'));?>
				</th>
			</tr>
			</thead>
			<tbody id="<?= $type ?>">
		<?php
		if($count > 0):
			for($i = 0;$i < $count;$i++):
				$idx = $i+1;
		?>
			<tr class="fm-paragraph<?=$errdisplay?>">
				<td class="td-left">
	<?php
					echo	$form->select(array(
							'name'		=> "softkeys_order[$softkey][]",
							'id'			=> "'it-softkey_".$softkey."_order[$i]",
							'key'			=> false,
							'empty'		=> false,
							'selected'	=> $idx
						),
						$order);
	 ?>
				</td>
				<td class="td-center">
	<?php
					echo	$form->select(array(
							'name'		=> "softkeys_key[$softkey][]",
							'id'			=> "it-softkey_".$softkey."_key[$i]",
							'key'			=> 'name',
							'altkey'  => 'id',
							'empty'		=> false,
							'selected'	=> $values[$i]
						),
						$softkeys);
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
				<td colspan="5" class="td-single"><?=$this->bbf('no_softkeys');?></td>
			</tr>
			</tfoot>
		</table>
		<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
			<tbody id="ex-<?=$type?>">
			<tr class="fm-paragraph">
				<td class="td-left">
	<?php
					echo	$form->select(array(
							'name'		=> "softkeys_order[$softkey][]",
							'id'			=> "it-softkey_".$softkey."_order[$i]",
							'key'			=> false,
							'empty'		=> false,
						),
						$order);
	 ?>
				</td>
				<td class="td-center">
	<?php
					echo	$form->select(array(
							'name'		=> "softkeys_key[$softkey][]",
							'id'			=> "it-softkey_".$softkey."_key[$i]",
							'key'			=> 'name',
							'altkey'  => 'id',
							'empty'		=> false,
						),
						$softkeys);
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
<br/>
