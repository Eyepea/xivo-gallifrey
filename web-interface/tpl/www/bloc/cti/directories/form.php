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
$info    = $this->get_var('info');
$urilist = $this->get_var('urilist');

$presence = $this->get_var('directories');

#$queues = $this->get_var('queues');
#$qmember = $this->get_var('qmember');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_name'),
				  'name'	=> 'directories[name]',
				  'labelid'	=> 'directories-name',
				  'size'	=> 15,
				  'default'	=> $element['directories']['name']['default'],
				  'value'	=> $info['directories']['name']));

	echo	$form->select(array('desc'	=> $this->bbf('fm_directories_uri'),
				  'name'	=> 'directories-uri',
				  'labelid'	=> 'directories-uri',
				  'default'	=> $element['directories']['uri']['default'],
				  'key'	=> false,
				  'selected'	=> $info['directories']['uri']),
				  $urilist);

	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_delimiter'),
				  'name'	=> 'directories[delimiter]',
				  'labelid'	=> 'directories-delimiter',
				  'size'	=> 1,
				  'default'	=> $element['directories']['delimiter']['default'],
				  'value'	=> $info['directories']['delimiter']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_match_direct'),
				  'name'	=> 'directories[match_direct]',
				  'labelid'	=> 'directories-match_direct',
				  'size'	=> 40,
				  'default'	=> $element['directories']['match_direct']['default'],
				  'value'	=> $info['directories']['match_direct']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_match_reverse'),
				  'name'	=> 'directories[match_reverse]',
				  'labelid'	=> 'directories-match_reverse',
				  'size'	=> 40,
				  'default'	=> $element['directories']['match_reverse']['default'],
				  'value'	=> $info['directories']['match_reverse']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_display_reverse'),
				  'name'	=> 'directories[display_reverse]',
				  'labelid'	=> 'directories-display_reverse',
				  'size'	=> 40,
				  'default'	=> $element['directories']['display_reverse']['default'],
				  'value'	=> $info['directories']['display_reverse']));

?>



<?php
	$type = 'disp';
	$errdisplay = '';
	$fields = $info['directories']['fields'];
	$count  = $fields === false?0:count($fields);
?>
	<p>&nbsp;</p>
	<div class="sb-list">
		<p><?= $this->bbf('fields_title');?></p>
		<table cellspacing="0" cellpadding="0" border="0">
			<thead>
			<tr class="sb-top">

				<th class="th-left"><?=$this->bbf('col_1');?></th>
				<th class="th-center"><?=$this->bbf('col_2');?></th>
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
								   'name'	  	=> 'field_fieldname[]',
								   'id'	    	=> false,
								   'label'		=> false,
								   'size'	  	=> 15,
								   'key'  		=> false,
								   'value'		=> $fields[$i]['fieldname'],
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'	   	=> 'field_value[]',
								   'id'		    => false,
								   'label'		=> false,
								   'size'	   	=> 30,
								   'key'		  => false,
								   'value'		=> $fields[$i]['value'],
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
				<td colspan="3" class="td-single"><?=$this->bbf('no_'.$type);?></td>
			</tr>
			</tfoot>
		</table>
		<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
			<tbody id="ex-<?=$type?>">
			<tr class="fm-paragraph">
				<td class="td-left">
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'field_fieldname[]',
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
								   'name'		=> 'field_value[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 30,
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
<br />

</div>
<div class="fm-paragraph fm-description">
	<p>
		<label id="lb-description" for="it-description"><?=$this->bbf('fm_description');?></label>
	</p>
	<?=$form->textarea(array('paragraph'    => false,
				 'label'    => false,
				 'name'     => 'directories[description]',
				 'id'       => 'it-description',
				 'cols'     => 60,
				 'rows'     => 5,
				 'default'  => $element['directories']['description']['default']),
			   $info['directories']['description']);?>
</div>

