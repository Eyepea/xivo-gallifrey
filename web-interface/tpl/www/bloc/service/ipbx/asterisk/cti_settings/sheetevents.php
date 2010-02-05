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

$url = &$this->get_module('url');
$form = &$this->get_module('form');
$dhtml = &$this->get_module('dhtml');

$info = $this->get_var('info');
$element = $this->get_var('element');
$error = $this->get_var('error');
$customs = $this->get_var('customs');
$sheetactionslist = $this->get_var('sheetactionslist');

if(($fm_save = $this->get_var('fm_save')) === true):
	$dhtml->write_js('xivo_form_result(true,\''.$dhtml->escape($this->bbf('fm_success-save')).'\');');
elseif($fm_save === false):
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

if(isset($error_js[0]) === true)
	$dhtml->write_js($error_js);

?>
<div class="b-infos b-form">
<h3 class="sb-top xspan">
	<span class="span-left">&nbsp;</span>
	<span class="span-center"><?=$this->bbf('title_content_name');?></span>
	<span class="span-right">&nbsp;</span>
</h3>
<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">
<?php
	echo	
		$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),
		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1)),

		$form->select(array('desc'	=> $this->bbf('fm_sheetevents_agentlinked'),
				    'name'	=> 'ctisheetevents[agentlinked]',
				    'labelid'	=> 'agentlinked',
				    'key'	=> false,
				    'default'	=> $element['ctisheetevents']['agentlinked']['default'],
			      	'selected' => $info['ctisheetevents']['agentlinked']),
				$sheetactionslist),

		$form->select(array('desc'	=> $this->bbf('fm_sheetevents_agentunlinked'),
				    'name'	=> 'ctisheetevents[agentunlinked]',
				    'labelid'	=> 'agentunlinked',
				    'key'	=> false,
				    'default'	=> $element['ctisheetevents']['agentunlinked']['default'],
			      	'selected' => $info['ctisheetevents']['agentunlinked']),
				$sheetactionslist),

		$form->select(array('desc'	=> $this->bbf('fm_sheetevents_faxreceived'),
				    'name'	=> 'ctisheetevents[faxreceived]',
				    'labelid'	=> 'faxreceived',
				    'key'	=> false,
				    'default'	=> $element['ctisheetevents']['faxreceived']['default'],
			      	'selected' => $info['ctisheetevents']['faxreceived']),
				$sheetactionslist),

		$form->select(array('desc'	=> $this->bbf('fm_sheetevents_incomingqueue'),
				    'name'	=> 'ctisheetevents[incomingqueue]',
				    'labelid'	=> 'incomingqueue',
				    'key'	=> false,
				    'default'	=> $element['ctisheetevents']['incomingqueue']['default'],
			      	'selected' => $info['ctisheetevents']['incomingqueue']),
				$sheetactionslist),

		$form->select(array('desc'	=> $this->bbf('fm_sheetevents_incominggroup'),
				    'name'	=> 'ctisheetevents[incominggroup]',
				    'labelid'	=> 'incominggroup',
				    'key'	=> false,
				    'default'	=> $element['ctisheetevents']['incominggroup']['default'],
			      	'selected' => $info['ctisheetevents']['incominggroup']),
				$sheetactionslist),

		$form->select(array('desc'	=> $this->bbf('fm_sheetevents_incomingdid'),
				    'name'	=> 'ctisheetevents[incomingdid]',
				    'labelid'	=> 'incominggdid',
				    'key'	=> false,
				    'default'	=> $element['ctisheetevents']['incomingdid']['default'],
			      	'selected' => $info['ctisheetevents']['incomingdid']),
				$sheetactionslist),

		$form->select(array('desc'	=> $this->bbf('fm_sheetevents_dial'),
				    'name'	=> 'ctisheetevents[dial]',
				    'labelid'	=> 'dial',
				    'key'	=> false,
				    'default'	=> $element['ctisheetevents']['dial']['default'],
			      	'selected' => $info['ctisheetevents']['dial']),
				$sheetactionslist),

		$form->select(array('desc'	=> $this->bbf('fm_sheetevents_link'),
				    'name'	=> 'ctisheetevents[link]',
				    'labelid'	=> 'link',
				    'key'	=> false,
				    'default'	=> $element['ctisheetevents']['link']['default'],
			      	'selected' => $info['ctisheetevents']['link']),
				$sheetactionslist),

		$form->select(array('desc'	=> $this->bbf('fm_sheetevents_unlink'),
				    'name'	=> 'ctisheetevents[unlink]',
				    'labelid'	=> 'unlink',
				    'key'	=> false,
				    'default'	=> $element['ctisheetevents']['unlink']['default'],
			      	'selected' => $info['ctisheetevents']['unlink']),
				$sheetactionslist);

		$type = 'custom';
		$count = count($customs);
		$errdisplay = '';

?>
	<p>&nbsp;</p>
	<div class="sb-list">
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
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this); return(dwho.dom.free_focus());"',
							   $this->bbf('col_add'));?>
				</th>
			</tr>
			</thead>
			<tbody id="<?=$type?>">
		<?php
		if($count > 0):
			foreach($customs as $k => $v)
			{

		?>
			<tr class="fm-paragraph<?=$errdisplay?>">
				<td class="td-left txt-center">
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'customcol1[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $k,
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->select(array('paragraph' => false,
									'name' 		=> 'customcol2[]',
									'label'		=> false,
									'id'		=> false,
									'key'		=> false,
									'selected'	=> $v,
									'invalid'	=> true,
								),
								$sheetactionslist);

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
			}
		endif;
		?>
			</tbody>
			<tfoot>
			<tr id="no-<?=$type?>"<?=($count > 0 ? ' class="b-nodisplay"' : '')?>>
				<td colspan="7" class="td-single"><?=$this->bbf('no_'.$type);?></td>
			</tr>
			</tfoot>
		</table>
		<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
			<tbody id="ex-<?=$type?>">
			<tr class="fm-paragraph">
				<td class="td-left txt-center">
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'customcol1[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->select(array('paragraph' => false,
									'name' 		=> 'customcol2[]',
									'label'		=> false,
									'id'		=> false,
									'key'		=> false,
									'invalid'	=> true,
								),
								$sheetactionslist);
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

<?php
	echo	$form->submit(array('name'	=> 'submit',
				    'id'	=> 'it-submit',
				    'value'	=> $this->bbf('fm_bt-save')));
?>
</form>

	</div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>
