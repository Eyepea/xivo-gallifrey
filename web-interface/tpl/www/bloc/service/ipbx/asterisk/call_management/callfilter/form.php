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

$info = $this->get_var('info');
$element = $this->get_var('element');
$bosslist = $this->get_var('bosslist');
$secretary = $this->get_var('secretary');
$context_list = $this->get_var('context_list');

if($this->get_var('act') === 'add'):
	$invalid_boss = false;
elseif(dwho_issa('callfiltermember',$info) === false
|| dwho_issa('boss',$info['callfiltermember']) === false):
	$invalid_boss = true;
else:
	$invalid_boss = false;
endif;

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_callfilter_name'),
				  'name'	=> 'callfilter[name]',
				  'labelid'	=> 'callfilter-name',
				  'size'	=> 15,
				  'default'	=> $element['callfilter']['name']['default'],
				  'value'	=> $info['callfilter']['name']));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_callfilter_context'),
				    'name'	=> 'callfilter[context]',
				    'labelid'	=> 'callfilter-context',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['callfilter']['context']['default'],
				    'selected'	=> $info['callfilter']['context']),
			      $context_list);
else:
	echo	'<div id="fd-callfilter-context" class="txt-center">',
		$url->href_html($this->bbf('create_context'),
				'service/ipbx/system_management/context',
				'act=add'),
		'</div>';
endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_callfilter_callfrom'),
				    'name'	=> 'callfilter[callfrom]',
				    'labelid'	=> 'callfilter-callfrom',
				    'key'	=> false,
				    'bbf'	=> 'fm_callfilter_callfrom-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['callfilter']['callfrom']['default'],
				    'selected'	=> $info['callfilter']['callfrom']),
			      $element['callfilter']['callfrom']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_callfilter_bosssecretary'),
				    'name'	=> 'callfilter[bosssecretary]',
				    'labelid'	=> 'callfilter-bosssecretary',
				    'key'	=> false,
				    'bbf'	=> 'fm_callfilter_bosssecretary-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['callfilter']['bosssecretary']['default'],
				    'selected'	=> $info['callfilter']['bosssecretary']),
			      $element['callfilter']['bosssecretary']['value'],
			      'onchange="xivo_callfilter_chg_mode(\'bosssecretary\',this);"'),

		$form->select(array('desc'	=> $this->bbf('fm_callfilter_ringseconds'),
				    'name'	=> 'callfilter[ringseconds]',
				    'labelid'	=> 'callfilter-ringseconds',
				    'key'	=> false,
				    'bbf'	=> 'fm_callfilter_ringseconds-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['callfilter']['ringseconds']['default'],
				    'selected'	=> $info['callfilter']['ringseconds']),
			      $element['callfilter']['ringseconds']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_callerid_mode'),
				    'name'	=> 'callerid[mode]',
				    'labelid'	=> 'callerid-mode',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_callerid_mode-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['callerid']['mode']['default'],
				    'selected'	=> $info['callerid']['mode']),
			      $element['callerid']['mode']['value'],
			      'onchange="xivo_ast_chg_callerid_mode(this);"'),

		$form->text(array('desc'	=> '&nbsp;',
				  'name'	=> 'callerid[callerdisplay]',
				  'labelid'	=> 'callerid-callerdisplay',
				  'size'	=> 15,
				  'notag'	=> false,
				  'default'	=> $element['callerid']['callerdisplay']['default'],
				  'value'	=> $info['callerid']['callerdisplay']));
?>

<fieldset id="fld-callfilter-boss">
	<legend><?=$this->bbf('fld-callfilter-boss');?></legend>
<?php
	if(empty($bosslist) === false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_callfiltermember-boss'),
					    'name'	=> 'callfiltermember[boss][id]',
					    'labelid'	=> 'callfiltermember-boss',
					    'key'	=> 'identity',
					    'altkey'	=> 'id',
					    'invalid'	=> $invalid_boss,
					    'selected'	=> $info['callfiltermember']['boss']['typeval']),
				      $bosslist),

			$form->select(array('desc'	=> $this->bbf('fm_callfiltermember_ringseconds-boss'),
					    'name'	=> 'callfiltermember[boss][ringseconds]',
					    'labelid'	=> 'callfiltermember-ringseconds-boss',
					    'key'	=> false,
					    'bbf'	=> 'fm_callfiltermember_ringseconds-boss-opt',
					    'bbfopt'	=> array('argmode' => 'paramvalue'),
					    'default'	=> $element['callfiltermember']['ringseconds']['default'],
					    'selected'	=> $info['callfiltermember']['boss']['ringseconds']),
				      $element['callfiltermember']['ringseconds']['value']);
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_user-boss'),
					'service/ipbx/pbx_settings/users',
					'act=add'),
			'</div>';
	endif;
?>
</fieldset>

<fieldset id="fld-callfilter-secretary">
	<legend><?=$this->bbf('fld-callfilter-secretary');?></legend>
<?php
	if($secretary['list'] !== false):
?>
	<div id="callfiltermember-secretarylist" class="fm-paragraph fm-multilist">
		<div class="slt-outlist">
			<?=$form->select(array('name'		=> 'callfiltermember-secretarylist',
					       'label'		=> false,
					       'id'		=> 'it-callfiltermember-secretarylist',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'paragraph'		=> false,
					       'key'		=> 'identity',
					       'altkey'		=> 'id'),
					 $secretary['list']);?>
		</div>

		<div class="inout-list">
			<a href="#"
			   onclick="dwho.form.move_selected('it-callfiltermember-secretarylist',
							  'it-callfiltermember-secretary');
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_insecretary');?>">
				<?=$url->img_html('img/site/button/arrow-left.gif',
						  $this->bbf('bt_insecretary'),
						  'class="bt-inlist" id="bt-insecretary" border="0"');?></a><br />
			<a href="#"
			   onclick="dwho.form.move_selected('it-callfiltermember-secretary',
							  'it-callfiltermember-secretarylist');
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_outsecretary');?>">
				<?=$url->img_html('img/site/button/arrow-right.gif',
						  $this->bbf('bt_outsecretary'),
						  'class="bt-outlist" id="bt-outsecretary" border="0"');?></a>
		</div>

		<div class="slt-inlist">
			<?=$form->select(array('name'		=> 'callfiltermember[secretary][]',
					       'label'		=> false,
					       'id'		=> 'it-callfiltermember-secretary',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'paragraph'		=> false,
					       'key'		=> 'identity',
					       'altkey'		=> 'id'),
					 $secretary['slt']);?>
			<div class="bt-updown">
				<a href="#"
				   onclick="dwho.form.order_selected('it-callfiltermember-secretary',1);
					    return(dwho.dom.free_focus());"
				   title="<?=$this->bbf('bt_upsecretary');?>">
					<?=$url->img_html('img/site/button/arrow-up.gif',
							  $this->bbf('bt_upsecretary'),
							  'class="bt-uplist" id="bt-upsecretary" border="0"');?></a><br />
				<a href="#"
				   onclick="dwho.form.order_selected('it-callfiltermember-secretary',-1);
					    return(dwho.dom.free_focus());"
				   title="<?=$this->bbf('bt_downsecretary');?>">
					<?=$url->img_html('img/site/button/arrow-down.gif',
							  $this->bbf('bt_downsecretary'),
							  'class="bt-downlist" id="bt-downsecretary" border="0"');?></a>
			</div>
		</div>
	</div>
	<div class="clearboth"></div>

<?php
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_user-secretary'),
					'service/ipbx/pbx_settings/users',
					'act=add'),
			'</div>';
	endif;
?>
</fieldset>

<div class="fm-paragraph fm-description">
	<p>
		<label id="lb-callfilter-description" for="it-callfilter-description"><?=$this->bbf('fm_callfilter_description');?></label>
	</p>
	<?=$form->textarea(array('paragraph'	=> false,
				 'label'	=> false,
				 'name'		=> 'callfilter[description]',
				 'id'		=> 'it-callfilter-description',
				 'cols'		=> 60,
				 'rows'		=> 5,
				 'default'	=> $element['callfilter']['description']['default']),
			   $info['callfilter']['description']);?>
</div>

</div>

<div id="sb-part-last" class="b-nodisplay">
	<fieldset id="fld-dialaction-noanswer">
		<legend><?=$this->bbf('fld-dialaction-noanswer');?></legend>
<?php
		$this->file_include('bloc/service/ipbx/asterisk/dialaction/all',
				    array('event'	=> 'noanswer'));
?>
	</fieldset>
</div>
