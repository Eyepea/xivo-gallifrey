<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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
$url = &$this->get_module('url');

$element = $this->get_var('element');
$type = $this->get_var('type');
$list = $this->get_var('list');
$nb = $this->get_var('count');
$err = $this->get_varra('error',array('contextnumbers',$type));

?>
<table cellspacing="0" cellpadding="0" border="0">
	<thead>
	<tr class="sb-top">
		<th class="th-left"><?=$this->bbf('col_contextnumbers_'.$type.'-numberbeg');?></th>
		<th class="th-center"><?=$this->bbf('col_contextnumbers_'.$type.'-numberend');?></th>
		<th class="th-right"><?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
								       $this->bbf('col_contextnumbers_'.$type.'-add'),
								       'border="0"'),
							'#',
							null,
							'onclick="xivo_context_entity_enable_add(\''.$type.'\',this);
								  return(dwho.dom.free_focus());"',
							$this->bbf('col_contextnumbers_'.$type.'-add'));?></th>
	</tr>
	</thead>
	<tbody id="contextnumbers-<?=$type?>">
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
					     'name'	=> 'contextnumbers['.$type.'][numberbeg][]',
					     'id'	=> false,
					     'label'	=> false,
					     'size'	=> 15,
					     'value'	=> $ref['numberbeg'],
					     'default'	=> $element['contextnumbers']['numberbeg']['default']));?>
		</td>
		<td>
			<?=$form->text(array('field'	=> false,
					     'name'	=> 'contextnumbers['.$type.'][numberend][]',
					     'id'	=> false,
					     'label'	=> false,
					     'size'	=> 15,
					     'value'	=> $ref['numberend'],
					     'default'	=> $element['contextnumbers']['numberend']['default']));?>
		</td>
		<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
								       $this->bbf('opt_contextnumbers_'.$type.'-delete'),
								       'border="0"'),
							'#',
							null,
							'onclick="dwho.dom.make_table_list(\'contextnumbers-'.$type.'\',this,1);
								  return(dwho.dom.free_focus());"',
							$this->bbf('opt_contextnumbers_'.$type.'-delete'));?></td>
	</tr>
<?php
	endfor;
endif;
?>
	</tbody>
	<tfoot>
	<tr id="no-contextnumbers-<?=$type?>"<?=($list !== false ? ' class="b-nodisplay"' : '')?>>
		<td colspan="3" class="td-single"><?=$this->bbf('no_contextnumbers-'.$type);?></td>
	</tr>
	</tfoot>
</table>
<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
	<tbody id="ex-contextnumbers-<?=$type?>">
	<tr class="fm-field">
		<td class="td-left txt-center">
			<?=$form->text(array('field'	=> false,
					     'name'	=> 'contextnumbers['.$type.'][numberbeg][]',
					     'id'	=> false,
					     'label'	=> false,
					     'disabled'	=> true,
					     'size'	=> 15,
					     'default'	=> $element['contextnumbers']['numberbeg']['default']));?>
		</td>
		<td>
			<?=$form->text(array('field'	=> false,
					     'name'	=> 'contextnumbers['.$type.'][numberend][]',
					     'id'	=> false,
					     'label'	=> false,
					     'disabled'	=> true,
					     'size'	=> 15,
					     'default'	=> $element['contextnumbers']['numberend']['default']));?>
		</td>
		<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
								       $this->bbf('opt_contextnumbers_'.$type.'-delete'),
								       'border="0"'),
							'#',
							null,
							'onclick="dwho.dom.make_table_list(\'contextnumbers-'.$type.'\',this,1);
								  return(dwho.dom.free_focus());"',
							$this->bbf('opt_contextnumbers_'.$type.'-delete'));?></td>
	</tr>
	</tbody>
</table>
