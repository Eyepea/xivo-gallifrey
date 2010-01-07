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
$url = &$this->get_module('url');

$type = $this->get_var('type');
$trunkslist = $this->get_var('trunkslist');
$count = $this->get_var('count');
$element = $this->get_var('element');
$error = $this->get_var('error');
$info = $this->get_var('info');

?>
<table cellspacing="0" cellpadding="0" border="0">
	<thead>
	<tr class="sb-top">
		<th class="th-left"><?=$this->bbf('col_'.$type.'-trunk');?></th>
		<th class="th-center"><?=$this->bbf('col_'.$type.'-exten');?></th>
		<th class="th-right">
			<?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
							  $this->bbf('col_'.$type.'-add'),
							  'border="0"'),
					   '#',
					   null,
					   'onclick="dwho.dom.make_table_list(\''.$type.'\',this); return(dwho.dom.free_focus());"',
					   $this->bbf('col_'.$type.'-add'));?>
		</th>
	</tr>
	</thead>
	<tbody id="<?=$type?>">
<?php

if($count > 0):
	for($i = 0;$i < $count;$i++):
		$ref = &$info[$type][$i];

		if(isset($error[$type][$i]) === true):
			$errdisplay = ' l-infos-error';
		else:
			$errdisplay = '';
		endif;
?>
	<tr class="fm-paragraph<?=$errdisplay?>">
		<td class="td-left txt-center">
			<?=$form->select(array('paragraph'	=> false,
					       'name'		=> $type.'[trunkfeaturesid][]',
					       'id'		=> false,
					       'label'		=> false,
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'invalid'	=> true,
					       'selected'	=> $ref['trunkfeaturesid'],
					       'default'	=> $element['handynumbers']['trunkfeaturesid']['default'],
					       'optgroup'	=> array('key'		=> true,
									 'altkey'	=> 'protocol',
									 'unique'	=> true,
									 'bbf'		=> 'fm_'.$type.'-trunk-opt',
									 'bbfopt'	=> array('argmode' => 'paramvalue'))),
					 $trunkslist);?>
		</td>
		<td>
			<?=$form->text(array('paragraph'	=> false,
					     'name'		=> $type.'[exten][]',
					     'id'		=> false,
					     'label'		=> false,
					     'size'		=> 15,
					     'value'		=> $ref['exten'],
					     'default'		=> $element['handynumbers']['exten']['default']));?>
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
		<td colspan="3" class="td-single"><?=$this->bbf('no_'.$type);?></td>
	</tr>
	</tfoot>
</table>
<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
	<tbody id="ex-<?=$type?>">
	<tr class="fm-paragraph">
		<td class="td-left txt-center">
			<?=$form->select(array('paragraph'	=> false,
					       'name'		=> $type.'[trunkfeaturesid][]',
					       'id'		=> false,
					       'label'		=> false,
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'disabled'	=> true,
					       'default'	=> $element['handynumbers']['trunkfeaturesid']['default'],
					       'optgroup'	=> array('key'		=> true,
									 'altkey'	=> 'protocol',
									 'unique'	=> true,
									 'bbf'		=> 'fm_'.$type.'-trunk-opt',
									 'bbfopt'	=> array('argmode' => 'paramvalue'))),
					 $trunkslist);?>
		</td>
		<td>
			<?=$form->text(array('paragraph'	=> false,
					     'name'		=> $type.'[exten][]',
					     'id'		=> false,
					     'label'		=> false,
					     'disabled'		=> true,
					     'size'		=> 15,
					     'default'		=> $element['handynumbers']['exten']['default']));?>
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
