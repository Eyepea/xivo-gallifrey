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

$element = $this->get_var('element');
$info = $this->get_var('info');
$data = $this->get_var('data');
$urilist = $this->get_var('urilist');

$presence = $this->get_var('displays');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_displays_name'),
				  'name'	=> 'displays[name]',
				  'labelid'	=> 'displays-name',
				  'size'	=> 15,
				  'default'	=> $element['displays']['name']['default'],
				  'value'	=> $info['displays']['name']));

?>
<?php
	$type = 'disp';
	$count = count($data);
	$errdisplay = '';
?>
	<p>&nbsp;</p>
	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<thead>
			<tr class="sb-top">

				<th class="th-left"><?=$this->bbf('col_1');?></th>
				<th class="th-center"><?=$this->bbf('col_2');?></th>
				<th class="th-center"><?=$this->bbf('col_3');?></th>
				<th class="th-center"><?=$this->bbf('col_4');?></th>
				<th class="th-right">
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
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'dispcol1[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $data[$i][0],
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'dispcol2[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $data[$i][1],
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'dispcol3[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $data[$i][2],
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'dispcol4[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $data[$i][3],
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
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'dispcol1[]',
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
								   'name'		=> 'dispcol2[]',
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
								   'name'		=> 'dispcol3[]',
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
								   'name'		=> 'dispcol4[]',
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
<div class="fm-paragraph fm-description">
	<p>
		<label id="lb-description" for="it-description"><?=$this->bbf('fm_description');?></label>
	</p>
	<?=$form->textarea(array('paragraph'    => false,
				 'label'    => false,
				 'name'     => 'displays[description]',
				 'id'       => 'it-description',
				 'cols'     => 60,
				 'rows'     => 5,
				 'default'  => $element['displays']['description']['default']),
			   $info['displays']['description']);?>
</div>

