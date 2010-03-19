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

$info = $this->get_var('info');
$element = $this->get_var('element');

$user_nb = $group_nb = $queue_nb = $meetme_nb = $incall_nb = 0;
$user_list = $group_list = $queue_list = $meetme_list = $incall_list = false;

$contextinc = $this->get_var('contextinc');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

if(dwho_issa('contextnumbers',$info) === true):

	$context_js = array();

	if(dwho_issa('user',$info['contextnumbers']) === true
	&& ($user_nb = count($info['contextnumbers']['user'])) > 0):
		$user_list = $info['contextnumbers']['user'];
		$context_js[] = 'dwho.dom.set_table_list(\'contextnumbers-user\','.$user_nb.');';
	endif;

	if(dwho_issa('group',$info['contextnumbers']) === true
	&& ($group_nb = count($info['contextnumbers']['group'])) > 0):
		$group_list = $info['contextnumbers']['group'];
		$context_js[] = 'dwho.dom.set_table_list(\'contextnumbers-group\','.$group_nb.');';
	endif;

	if(dwho_issa('queue',$info['contextnumbers']) === true
	&& ($queue_nb = count($info['contextnumbers']['queue'])) > 0):
		$queue_list = $info['contextnumbers']['queue'];
		$context_js[] = 'dwho.dom.set_table_list(\'contextnumbers-queue\','.$queue_nb.');';
	endif;

	if(dwho_issa('meetme',$info['contextnumbers']) === true
	&& ($meetme_nb = count($info['contextnumbers']['meetme'])) > 0):
		$meetme_list = $info['contextnumbers']['meetme'];
		$context_js[] = 'dwho.dom.set_table_list(\'contextnumbers-meetme\','.$meetme_nb.');';
	endif;

	if(dwho_issa('incall',$info['contextnumbers']) === true
	&& ($incall_nb = count($info['contextnumbers']['incall'])) > 0):
		$incall_list = $info['contextnumbers']['incall'];
		$context_js[] = 'dwho.dom.set_table_list(\'contextnumbers-incall\','.$incall_nb.');';
	endif;

	if(isset($context_js[0]) === true):
		$dhtml = &$this->get_module('dhtml');
		$dhtml->write_js($context_js);
	endif;

endif;

$incall_err = $this->get_var('error','contextnumbers','incall');

?>

<div id="sb-part-first">

<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_context_name'),
				  'name'	=> 'context[name]',
				  'labelid'	=> 'context-name',
				  'size'	=> 15,
				  'default'	=> $element['context']['name']['default'],
				  'value'	=> $info['context']['name'],
			          'error'	=> $this->bbf_args('error',
						   $this->get_var('error', 'context', 'name')) )),

		$form->text(array('desc'	=> $this->bbf('fm_context_displayname'),
				  'name'	=> 'context[displayname]',
				  'labelid'	=> 'context-displayname',
				  'size'	=> 15,
				  'default'	=> $element['context']['displayname']['default'],
				  'value'	=> $info['context']['displayname'],
			          'error'	=> $this->bbf_args('error',
						   $this->get_var('error', 'context', 'displayname')) ));

	if(($entities = $this->get_var('entities')) !== false):
		echo $form->select(array('desc'		=> $this->bbf('fm_context_entity'),
					 'name'		=> 'context[entity]',
					 'labelid'	=> 'context-entity',
					 'empty'	=> isset($info['deletable']) === true ? $info['deletable'] : true,
					 'key'		=> 'displayname',
					 'altkey'	=> 'name',
					 'invalid'	=> ($this->get_var('act') === 'edit'),
					 'default'	=> $element['context']['entity']['default'],
					 'selected'	=> $info['context']['entity']),
				   $entities,
				   'onchange="xivo_context_entity_status(this.form,this.value.length === 0);"');
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_entity'),'xivo/configuration/manage/entity','act=add'),
			'</div>';
	endif;

	if($contextinc['list'] !== false):
?>

<div id="contextlist" class="fm-paragraph fm-multilist">
	<p>
		<label id="lb-contextlist" for="it-contextlist" onclick="dwho_eid('it-contextlist').focus();">
			<?=$this->bbf('fm_context_context-include');?>
		</label>
	</p>
	<div class="slt-outlist">
<?php
		echo	$form->select(array('name'	=> 'contextlist',
					    'label'	=> false,
					    'id'	=> 'it-contextlist',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false,
					    'key'	=> 'identity',
					    'altkey'	=> 'name'),
				      $contextinc['list']);
?>
	</div>

	<div class="inout-list">
		<a href="#"
		   onclick="dwho.form.move_selected('it-contextlist',
						  'it-context');
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_incontext');?>">
			<?=$url->img_html('img/site/button/arrow-left.gif',
					  $this->bbf('bt_incontext'),
					  'class="bt-inlist" id="bt-incontext" border="0"');?></a><br />
		<a href="#"
		   onclick="dwho.form.move_selected('it-context',
						  'it-contextlist');
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_outcontext');?>">
			<?=$url->img_html('img/site/button/arrow-right.gif',
					  $this->bbf('bt_outcontext'),
					  'class="bt-outlist" id="bt-outcontext" border="0"');?></a>
	</div>

	<div class="slt-inlist">
<?php
		echo	$form->select(array('name'	=> 'contextinclude[]',
					    'label'	=> false,
					    'id'	=> 'it-context',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false,
					    'key'	=> 'identity',
					    'altkey'	=> 'name'),
				      $contextinc['slt']);
?>
		<div class="bt-updown">
			<a href="#"
			   onclick="dwho.form.order_selected('it-context',1);
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_upcontext');?>">
				<?=$url->img_html('img/site/button/arrow-up.gif',
						  $this->bbf('bt_upcontext'),
						  'class="bt-uplist" id="bt-upcontext" border="0"');?></a><br />
			<a href="#"
			   onclick="dwho.form.order_selected('it-context',-1);
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_downcontext');?>">
				<?=$url->img_html('img/site/button/arrow-down.gif',
						  $this->bbf('bt_downcontext'),
						  'class="bt-downlist" id="bt-downcontext" border="0"');?></a>
		</div>
	</div>
</div>
<div class="clearboth"></div>
<?php
	endif;
?>

<div class="fm-paragraph fm-description">
	<p>
		<label id="lb-context-description" for="it-context-description"><?=$this->bbf('fm_context_description');?></label>
	</p>
	<?=$form->textarea(array('paragraph'	=> false,
				 'label'	=> false,
				 'name'		=> 'context[description]',
				 'id'		=> 'it-context-description',
				 'cols'		=> 60,
				 'rows'		=> 5,
				 'default'	=> $element['context']['description']['default']),
			   $info['context']['description']);?>
</div>

</div>

<div id="sb-part-user" class="b-nodisplay">
	<div class="sb-list">
<?php
	$this->file_include('bloc/service/ipbx/asterisk/system_management/context/contextnumbers',
			    array('type'	=> 'user',
				  'count'	=> $user_nb,
				  'list'	=> $user_list));
?>
	</div>
</div>

<div id="sb-part-group" class="b-nodisplay">
	<div class="sb-list">
<?php
	$this->file_include('bloc/service/ipbx/asterisk/system_management/context/contextnumbers',
			    array('type'	=> 'group',
				  'count'	=> $group_nb,
				  'list'	=> $group_list));
?>
	</div>
</div>

<div id="sb-part-queue" class="b-nodisplay">
	<div class="sb-list">
<?php
	$this->file_include('bloc/service/ipbx/asterisk/system_management/context/contextnumbers',
			    array('type'	=> 'queue',
				  'count'	=> $queue_nb,
				  'list'	=> $queue_list));
?>
	</div>
</div>

<div id="sb-part-meetme" class="b-nodisplay">
	<div class="sb-list">
<?php
	$this->file_include('bloc/service/ipbx/asterisk/system_management/context/contextnumbers',
			    array('type'	=> 'meetme',
				  'count'	=> $meetme_nb,
				  'list'	=> $meetme_list));
?>
	</div>
</div>

<div id="sb-part-last" class="b-nodisplay">
	<div class="sb-list">
	<table cellspacing="0" cellpadding="0" border="0">
		<thead>
		<tr class="sb-top">
			<th class="th-left"><?=$this->bbf('col_contextnumbers_incall-numberbeg');?></th>
			<th class="th-center"><?=$this->bbf('col_contextnumbers_incall-numberend');?></th>
			<th class="th-center"><?=$this->bbf('col_contextnumbers_incall-didlength');?></th>
			<th class="th-right"><?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
									       $this->bbf('col_contextnumbers_incall-add'),
									       'border="0"'),
								'#',
								null,
								'onclick="xivo_context_entity_enable_add(\'incall\',this);
									  return(dwho.dom.free_focus());"',
								$this->bbf('col_contextnumbers_incall-add'));?></th>
		</tr>
		</thead>
		<tbody id="contextnumbers-incall">
<?php
if($incall_list !== false):
	for($i = 0;$i < $incall_nb;$i++):
		$ref = &$incall_list[$i];

		if(isset($incall_err[$i]) === true):
			$errdisplay = ' l-infos-error';
		else:
			$errdisplay = '';
		endif;
?>
		<tr class="fm-paragraph<?=$errdisplay?>">
			<td class="td-left txt-center">
				<?=$form->text(array('paragraph'	=> false,
						     'name'		=> 'contextnumbers[incall][numberbeg][]',
						     'id'		=> false,
						     'label'		=> false,
						     'size'		=> 15,
						     'value'		=> $ref['numberbeg'],
						     'default'		=> $element['contextnumbers']['numberbeg']['default']));?>
			</td>
			<td>
				<?=$form->text(array('paragraph'	=> false,
						     'name'		=> 'contextnumbers[incall][numberend][]',
						     'id'		=> false,
						     'label'		=> false,
						     'size'		=> 15,
						     'value'		=> $ref['numberend'],
						     'default'		=> $element['contextnumbers']['numberend']['default']));?>
			</td>
			<td>
				<?=$form->select(array('paragraph'	=> false,
						       'name'		=> 'contextnumbers[incall][didlength][]',
						       'id'		=> false,
						       'label'		=> false,
						       'key'		=> false,
						       'selected'	=> $ref['didlength'],
						       'default'	=> $element['contextnumbers']['didlength']['default']),
						 $element['contextnumbers']['didlength']['value']);?>
			</td>
			<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
									       $this->bbf('opt_contextnumbers_incall-delete'),
									       'border="0"'),
								'#',
								null,
								'onclick="dwho.dom.make_table_list(\'contextnumbers-incall\',this,1);
									  return(dwho.dom.free_focus());"',
								$this->bbf('opt_contextnumbers_incall-delete'));?></td>
		</tr>

<?php
	endfor;
endif;
?>
		</tbody>
		<tfoot>
		<tr id="no-contextnumbers-incall"<?=($incall_list !== false ? ' class="b-nodisplay"' : '')?>>
			<td colspan="4" class="td-single"><?=$this->bbf('no_contextnumbers-incall');?></td>
		</tr>
		</tfoot>
	</table>
	<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
		<tbody id="ex-contextnumbers-incall">
		<tr class="fm-paragraph">
			<td class="td-left txt-center">
				<?=$form->text(array('paragraph'	=> false,
						     'name'		=> 'contextnumbers[incall][numberbeg][]',
						     'id'		=> false,
						     'label'		=> false,
						     'disabled'		=> true,
						     'size'		=> 15,
						     'default'		=> $element['contextnumbers']['numberend']['default']));?>
			</td>
			<td>
				<?=$form->text(array('paragraph'	=> false,
						     'name'		=> 'contextnumbers[incall][numberend][]',
						     'id'		=> false,
						     'label'		=> false,
						     'disabled'		=> true,
						     'size'		=> 15,
						     'default'		=> $element['contextnumbers']['numberend']['default']));?>
			</td>
			<td>
				<?=$form->select(array('paragraph'	=> false,
						       'name'		=> 'contextnumbers[incall][didlength][]',
						       'id'		=> false,
						       'label'		=> false,
						       'key'		=> false,
						       'disabled'	=> true,
						       'default'	=> $element['contextnumbers']['didlength']['default']),
						 $element['contextnumbers']['didlength']['value']);?></td>
			<td class="td-right"><?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
									       $this->bbf('opt_contextnumbers_incall-delete'),
									       'border="0"'),
								'#',
								null,
								'onclick="dwho.dom.make_table_list(\'contextnumbers-incall\',this,1);
									  return(dwho.dom.free_focus());"',
								$this->bbf('opt_contextnumbers_incall-delete'));?></td>
		</tr>
		</tbody>
	</table>
	</div>
</div>
