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
$actionslist = $this->get_var('actionslist');

$actionsavail = array(
	'queueadd'        => $this->bbf('action-queueadd'), 
	'queueremove'     => $this->bbf('action-queueremove'), 
	'queuepause'      => $this->bbf('action-queuepause'), 
	'queueunpause'    => $this->bbf('action-queueunpause'),
	'enablevoicemail' => $this->bbf('action-enablevoicemail'), 
	'callrecord'      => $this->bbf('action-callrecord'), 
	'incallfilter'    => $this->bbf('action-incallfilter'), 
	'enablednd'       => $this->bbf('action-enablednd'), 
	'enableunc'       => $this->bbf('action-enableunc'), 
	'enablebusy'      => $this->bbf('action-enablebusy'), 
	'enablerna'       => $this->bbf('action-enablerna'),
);

$status = $this->get_var('status');

$type = 'actions';
$count = count($actionslist);

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_status_name'),
				  'name'	=> 'status[name]',
				  'labelid'	=> 'status-name',
				  'size'	=> 15,
				  'default'	=> $element['ctistatus']['name']['default'],
				  'value'	=> $info['status']['name']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_status_display_name'),
				  'name'	=> 'status[display_name]',
				  'labelid'	=> 'status-display_name',
				  'size'	=> 15,
				  'comment' => $this->bbf('cmt_status_display_name'),
				  'default'	=> $element['ctistatus']['display_name']['default'],
				  'value'	=> $info['status']['display_name']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_status_color'),
				  'name'	=> 'status[color]',
				  'labelid'	=> 'status-color',
				  'size'	=> 15,
				  'class'	=> 'color {hash:true}',
				  'comment' => $this->bbf('cmt_status_color'),
				  'default'	=> $element['ctistatus']['color']['default'],
				  'value'	=> $info['status']['color']));

?>
	<div class="fm-paragraph fm-description">
		<fieldset id="cti-access_status">
			<legend><?=$this->bbf('cti-access_status');?></legend>
			<div id="xivoserverlist" class="fm-paragraph fm-multilist">
				<div class="slt-outlist">
<?php
				echo    $form->select(array('name'  => 'access_statuslist',
							'label' => false,
							'id'    => 'it-access_statuslist',
							'key'   => 'display_name',
							'altkey'    => 'id',
							'multiple'  => true,
							'size'  => 5,
							'paragraph' => false),
						$info['access_status']['list']);
?>
				</div>
				<div class="inout-list">
					<a href="#"
					onclick="dwho.form.move_selected('it-access_statuslist','it-access_status');
					return(dwho.dom.free_focus());"
					title="<?=$this->bbf('bt_inaccess_status');?>">
					<?=$url->img_html('img/site/button/arrow-left.gif',
							$this->bbf('bt_inaccess_status'),
							'class="bt-inlist" id="bt-inaccess_status" border="0"');?></a><br />

					<a href="#"
					onclick="dwho.form.move_selected('it-access_status','it-access_statuslist');
					return(dwho.dom.free_focus());"
					title="<?=$this->bbf('bt_outaccess_status');?>">
					<?=$url->img_html('img/site/button/arrow-right.gif',
							$this->bbf('bt_outaccess_status'),
							'class="bt-outlist" id="bt-outaccess_status" border="0"');?></a>
				</div>
				<div class="slt-inlist">
<?php
				echo    $form->select(array('name'  => 'access_status[]',
						'label' => false,
						'id'    => 'it-access_status',
						'key'	=> 'display_name',
						'altkey'    => 'id',
						'multiple'  => true,
						'size'  => 5,
						'paragraph' => false),
					$info['access_status']['slt']);
?>
				</div>
			</div>
		</fieldset>
		<div class="clearboth"></div>
	</div>
	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<thead>
			<tr class="sb-top">
				<th class="th-left"><?=$this->bbf('col_'.$type.'-name');?></th>
				<th class="th-center"><?=$this->bbf('col_'.$type.'-args');?></th>
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
			<tbody id="actions">
		<?php
		if($count > 0):
			for($i = 0;$i < $count;$i++):
					$errdisplay = '';
					$pattern = '/^(.*)\((.*)\)/';
					$match = array();
					preg_match($pattern, $actionslist[$i], $match);
		?>
			<tr class="fm-paragraph<?=$errdisplay?>">
				<td class="td-left txt-center">
	<?php
					echo $form->select(array('paragraph'	=> false,
								   'name'		=> 'actionslist[]',
								   'id'		=> false,
								   'label'		=> false,
#								   'key'		=> false,
#								   'key'      => 'name',
#								   'altkey'   => 'id',
								   'selected'	=> $match[1],
								   'invalid'	=> true,
							 ),
							 $actionsavail);?>
				</td>
				<td>
					<?=$form->text(array('paragraph'	=> false,
								 'name'		=> 'actionsargs[]',
								 'id'		=> false,
								 'label'		=> false,
								 'size'		=> 15,
								 'value'		=> $match[2],
								 'default'		=> $match[2]));?>
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
	<?php
					echo $form->select(array('paragraph'	=> false,
								   'name'		=> 'actionslist[]',
								   'id'		=> false,
								   'label'		=> false,
								   'key'		=> false,
								   'invalid'	=> true
							 ),
							 $actionsavail);?>
				</td>
				<td>
					<?=$form->text(array('paragraph'	=> false,
								 'name'		=> 'actionsargs[]',
								 'id'		=> false,
								 'label'		=> false,
								 'size'		=> 15,
								 'disabled'	=> true,
								 'default'		=> ''));?>
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

