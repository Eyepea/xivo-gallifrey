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
$moh_list = $this->get_var('moh_list');
$context_list = $this->get_var('context_list');

if(dwho_has_len($info['meetmefeatures']['admin_internalid']) === true):
	$admin_suggest = $this->get_var('info','meetmeadmininternal','fullname');
else:
	$admin_suggest = null;
endif;

$dhtml = &$this->get_module('dhtml');
$dhtml->write_js('var xivo_fm_meetme_admin_suggest = \''.$dhtml->escape($admin_suggest).'\';');

if($this->get_var('fm_save') === false):
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_meetmeroom_confno'),
				  'name'	=> 'meetmeroom[confno]',
				  'labelid'	=> 'meetmeroom-confno',
				  'size'	=> 15,
				  'default'	=> $element['meetmeroom']['confno']['default'],
				  'value'	=> $info['meetmeroom']['confno'])),

		$form->text(array('desc'	=> $this->bbf('fm_meetmefeatures_number'),
				  'name'	=> 'meetmefeatures[number]',
				  'labelid'	=> 'meetmefeatures-number',
				  'label'	=> 'lb-meetmefeatures-number',
				  'size'	=> 15,
				  'default'	=> $element['meetmefeatures']['number']['default'],
				  'value'	=> $info['meetmefeatures']['number'])),

		$form->text(array('desc'	=> $this->bbf('fm_meetmeroom_pin'),
				  'name'	=> 'meetmeroom[pin]',
				  'labelid'	=> 'meetmeroom-pin',
				  'size'	=> 15,
				  'default'	=> $element['meetmeroom']['pin']['default'],
				  'value'	=> $info['meetmeroom']['pin']));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_context'),
				    'name'	=> 'meetmefeatures[context]',
				    'labelid'	=> 'meetmefeatures-context',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['meetmefeatures']['context']['default'],
				    'selected'	=> $info['meetmefeatures']['context']),
			      $context_list);
else:
	echo	'<div id="fd-meetmefeatures-context" class="txt-center">',
		$url->href_html($this->bbf('create_context'),
				'service/ipbx/system_management/context',
				'act=add'),
		'</div>';
endif;

	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_noplaymsgfirstenter'),
				      'name'	=> 'meetmefeatures[noplaymsgfirstenter]',
				      'labelid'	=> 'meetmefeatures-noplaymsgfirstenter',
				      'default'	=> $element['meetmefeatures']['noplaymsgfirstenter']['default'],
				      'checked' => $info['meetmefeatures']['noplaymsgfirstenter'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_record'),
				      'name'	=> 'meetmefeatures[record]',
				      'labelid'	=> 'meetmefeatures-record',
				      'default'	=> $element['meetmefeatures']['record']['default'],
				      'checked' => $info['meetmefeatures']['record'])),

		$form->text(array('desc'	=> $this->bbf('fm_meetmefeatures_maxuser'),
				  'name'	=> 'meetmefeatures[maxuser]',
				  'labelid'	=> 'meetmefeatures-maxuser',
				  'size'	=> 5,
				  'default'	=> $element['meetmefeatures']['maxuser']['default'],
				  'value'	=> $info['meetmefeatures']['maxuser'])),

		$form->text(array('desc'	=> $this->bbf('fm_meetmefeatures_preprocess-subroutine'),
				  'name'	=> 'meetmefeatures[preprocess_subroutine]',
				  'labelid'	=> 'meetmefeatures-preprocess-subroutine',
				  'size'	=> 15,
				  'default'	=> $element['meetmefeatures']['preprocess_subroutine']['default'],
				  'value'	=> $info['meetmefeatures']['preprocess_subroutine']));
?>
	<div class="fm-paragraph fm-description">
		<p>
			<label id="lb-meetmefeatures-description" for="it-meetmefeatures-description">
				<?=$this->bbf('fm_meetmefeatures_description');?>
			</label>
		</p>
		<?=$form->textarea(array('paragraph'	=> false,
					 'label'	=> false,
					 'name'		=> 'meetmefeatures[description]',
					 'id'		=> 'it-meetmefeatures-description',
					 'cols'		=> 60,
					 'rows'		=> 5,
					 'default'	=> $element['meetmefeatures']['description']['default']),
				   $info['meetmefeatures']['description']);?>
	</div>
</div>

<div id="sb-part-period" class="b-nodisplay">
<div class="sb-list">
<table cellspacing="0" cellpadding="0" border="0" class="fm-paragraph">
	<thead>
	<tr class="sb-top">
		<th colspan="2" class="th-single"><?=$this->bbf('col_startdate');?></th>
	</tr>
	</thead>
	<tbody>
	<tr>
		<td class="txt-left"><?=$this->bbf('meetmefeatures_startdate-mday');?></td>
		<td class="td-right">
		<?=$form->select(array('paragraph'	=> false,
				       'name'		=> 'meetmefeatures[startdate][2]',
				       'labelid'	=> 'it-meetmefeatures-startdate-mday',
				       'empty'		=> true,
				       'key'		=> false,
				       'optionf'	=> '%02u',
				       'default'	=> $element['meetmefeatures']['startdatemday']['default'],
				       'selected'	=> $this->get_var('info','meetmefeatures','startdate','mday')),
				 $element['meetmefeatures']['startdatemday']['value']);?>
		</td>
	</tr>
	<tr>
		<td class="txt-left"><?=$this->bbf('meetmefeatures_startdate-mon');?></td>
		<td class="td-right">
		<?=$form->select(array('paragraph'	=> false,
				       'name'		=> 'meetmefeatures[startdate][1]',
				       'labelid'	=> 'it-meetmefeatures-startdate-mon',
				       'empty'		=> true,
				       'bbf'		=> 'date_Month',
				       'bbfopt'		=> array('argmode' => 'paramvalue'),
				       'default'	=> $element['meetmefeatures']['startdatemon']['default'],
				       'selected'	=> $this->get_var('info','meetmefeatures','startdate','mon')),
				 $element['meetmefeatures']['startdatemon']['value']);?>
		</td>
	<tr>
	<tr>
		<td class="txt-left"><?=$this->bbf('meetmefeatures_startdate-year');?></td>
		<td class="td-right">
		<?=$form->select(array('paragraph'	=> false,
				       'name'		=> 'meetmefeatures[startdate][0]',
				       'labelid'	=> 'it-meetmefeatures-startdate-year',
				       'empty'		=> true,
				       'key'		=> false,
				       'default'	=> $element['meetmefeatures']['startdateyear']['default'],
				       'selected'	=> $this->get_var('info','meetmefeatures','startdate','year')),
				 $element['meetmefeatures']['startdateyear']['value']);?>
		</td>
	</tr>
	<tr>
		<td class="txt-left"><?=$this->bbf('meetmefeatures_startdate-hours');?></td>
		<td class="td-right">
		<?=$form->select(array('paragraph'	=> false,
				       'name'		=> 'meetmefeatures[startdate][3]',
				       'labelid'	=> 'it-meetmefeatures-startdate-hours',
				       'empty'		=> true,
				       'key'		=> false,
				       'optionf'	=> '%02u',
				       'default'	=> $element['meetmefeatures']['startdatehours']['default'],
				       'selected'	=> $this->get_var('info','meetmefeatures','startdate','hours')),
				 $element['meetmefeatures']['startdatehours']['value']);?>
		</td>
	</tr>
	<tr>
		<td class="txt-left"><?=$this->bbf('meetmefeatures_startdate-minutes');?></td>
		<td class="td-right">
		<?=$form->select(array('paragraph'	=> false,
				       'name'		=> 'meetmefeatures[startdate][4]',
				       'labelid'	=> 'it-meetmefeatures-startdate-minutes',
				       'empty'		=> true,
				       'key'		=> false,
				       'optionf'	=> '%02u',
				       'default'	=> $element['meetmefeatures']['startdateminutes']['default'],
				       'selected'	=> $this->get_var('info','meetmefeatures','startdate','minutes')),
				 $element['meetmefeatures']['startdateminutes']['value']);?>
		</td>
	</tr>
	</tbody>
</table>
</div>
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_meetmefeatures_durationm'),
				  'name'	=> 'meetmefeatures[durationm]',
				  'labelid'	=> 'meetmefeatures-durationm',
				  'size'	=> 5,
				  'default'	=> $element['meetmefeatures']['durationm']['default'],
				  'value'	=> $info['meetmefeatures']['durationm'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_closeconfdurationexceeded'),
				      'name'	=> 'meetmefeatures[closeconfdurationexceeded]',
				      'labelid'	=> 'meetmefeatures-closeconfdurationexceeded',
				      'default'	=> $element['meetmefeatures']['closeconfdurationexceeded']['default'],
				      'checked' => $info['meetmefeatures']['closeconfdurationexceeded'])),

		$form->text(array('desc'	=> $this->bbf('fm_meetmefeatures_nbuserstartdeductduration'),
				  'name'	=> 'meetmefeatures[nbuserstartdeductduration]',
				  'labelid'	=> 'meetmefeatures-nbuserstartdeductduration',
				  'size'	=> 5,
				  'default'	=> $element['meetmefeatures']['nbuserstartdeductduration']['default'],
				  'value'	=> $info['meetmefeatures']['nbuserstartdeductduration'])),

		$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_timeannounceclose'),
				    'name'	=> 'meetmefeatures[timeannounceclose]',
				    'labelid'	=> 'meetmefeatures-timeannounceclose',
				    'empty'	=> $this->bbf('fm_meetmefeatures_timeannounceclose-opt','default'),
				    'key'	=> false,
				    'bbf'	=> 'fm_meetmefeatures_timeannounceclose-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'selected'	=> $info['meetmefeatures']['timeannounceclose'],
				    'default'	=> $element['meetmefeatures']['timeannounceclose']['default']),
			      $element['meetmefeatures']['timeannounceclose']['value']);
?>
</div>

<div id="sb-part-administrator" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_admin-typefrom'),
				    'name'	=> 'meetmefeatures[admin_typefrom]',
				    'labelid'	=> 'meetmefeatures-admin-typefrom',
				    'key'	=> false,
				    'bbf'	=> 'fm_meetmefeatures_admin-typefrom-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue'),
				    'default'	=> $element['meetmefeatures']['admin_typefrom']['default'],
				    'selected'	=> $info['meetmefeatures']['admin_typefrom']),
			      $element['meetmefeatures']['admin_typefrom']['value']),

		$form->hidden(array('name'	=> 'meetmefeatures[admin_internalid]',
				    'id'	=> 'it-meetmefeatures-admin-internalid',
				    'value'	=> $info['meetmefeatures']['admin_internalid'])),

		$form->text(array('desc'	=> '&nbsp;',
				  'name'	=> 'meetme-admin-suggest',
				  'labelid'	=> 'meetme-admin-suggest',
				  'size'	=> 20,
				  'default'	=> $this->bbf('fm_meetme_admin-suggest-default'))),

		$form->text(array('desc'	=> $this->bbf('fm_meetmefeatures_admin-externalid'),
				  'name'	=> 'meetmefeatures[admin_externalid]',
				  'labelid'	=> 'meetmefeatures-admin-externalid',
				  'value'	=> $info['meetmefeatures']['admin_externalid'])),

		$form->text(array('desc'	=> $this->bbf('fm_meetmeroom_pinadmin'),
				  'name'	=> 'meetmeroom[pinadmin]',
				  'labelid'	=> 'meetmeroom-pinadmin',
				  'size'	=> 15,
				  'default'	=> $element['meetmeroom']['pinadmin']['default'],
				  'value'	=> $info['meetmeroom']['pinadmin'])),

		$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_admin-identification'),
				    'name'	=> 'meetmefeatures[admin_identification]',
				    'labelid'	=> 'meetmefeatures-admin-identification',
				    'key'	=> false,
				    'bbf'	=> 'fm_meetmefeatures_admin-identification-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue'),
				    'default'	=> $element['meetmefeatures']['admin_identification']['default'],
				    'selected'	=> $info['meetmefeatures']['admin_identification']),
			      $element['meetmefeatures']['admin_identification']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_admin-mode'),
				    'name'	=> 'meetmefeatures[admin_mode]',
				    'labelid'	=> 'meetmefeatures-admin-mode',
				    'key'	=> false,
				    'bbf'	=> 'fm_meetmefeatures_admin-mode-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue'),
				    'default'	=> $element['meetmefeatures']['admin_mode']['default'],
				    'selected'	=> $info['meetmefeatures']['admin_mode']),
			      $element['meetmefeatures']['admin_mode']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_admin-announceusercount'),
				      'name'	=> 'meetmefeatures[admin_announceusercount]',
				      'labelid'	=> 'meetmefeatures-admin-announceusercount',
				      'default'	=> $element['meetmefeatures']['admin_announceusercount']['default'],
				      'checked'	=> $info['meetmefeatures']['admin_announceusercount'])),

		$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_admin-announcejoinleave'),
				    'name'	=> 'meetmefeatures[admin_announcejoinleave]',
				    'labelid'	=> 'meetmefeatures-admin-announcejoinleave',
				    'key'	=> false,
				    'bbf'	=> 'fm_meetmefeatures_admin-announcejoinleave-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue'),
				    'default'	=> $element['meetmefeatures']['admin_announcejoinleave']['default'],
				    'selected'	=> $info['meetmefeatures']['admin_announcejoinleave']),
			      $element['meetmefeatures']['admin_announcejoinleave']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_admin-moderationmode'),
				      'name'	=> 'meetmefeatures[admin_moderationmode]',
				      'labelid'	=> 'meetmefeatures-admin-moderationmode',
				      'default'	=> $element['meetmefeatures']['admin_moderationmode']['default'],
				      'checked' => $info['meetmefeatures']['admin_moderationmode'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_admin-initiallymuted'),
				      'name'	=> 'meetmefeatures[admin_initiallymuted]',
				      'labelid'	=> 'meetmefeatures-admin-initiallymuted',
				      'default'	=> $element['meetmefeatures']['admin_initiallymuted']['default'],
				      'checked'	=> $info['meetmefeatures']['admin_initiallymuted']));

if($moh_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_admin-musiconhold'),
				    'name'	=> 'meetmefeatures[admin_musiconhold]',
				    'labelid'	=> 'meetmefeatures-admin-musiconhold',
				    'empty'	=> true,
				    'key'	=> 'category',
				    'invalid'	=> ($this->get_var('act') === 'edit'),
				    'default'	=> ($this->get_var('act') === 'add' ?
				    		    $element['meetmefeatures']['admin_musiconhold']['default'] :
						    null),
				    'selected'	=> $info['meetmefeatures']['admin_musiconhold']),
			      $moh_list);
endif;

	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_admin-poundexit'),
				      'name'	=> 'meetmefeatures[admin_poundexit]',
				      'labelid'	=> 'meetmefeatures-admin-poundexit',
				      'default'	=> $element['meetmefeatures']['admin_poundexit']['default'],
				      'checked'	=> $info['meetmefeatures']['admin_poundexit'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_admin-quiet'),
				      'name'	=> 'meetmefeatures[admin_quiet]',
				      'labelid'	=> 'meetmefeatures-admin-quiet',
				      'default'	=> $element['meetmefeatures']['admin_quiet']['default'],
				      'checked'	=> $info['meetmefeatures']['admin_quiet'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_admin-starmenu'),
				      'name'	=> 'meetmefeatures[admin_starmenu]',
				      'labelid'	=> 'meetmefeatures-admin-starmenu',
				      'default'	=> $element['meetmefeatures']['admin_starmenu']['default'],
				      'checked'	=> $info['meetmefeatures']['admin_starmenu'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_admin-closeconflastmarkedexit'),
				      'name'	=> 'meetmefeatures[admin_closeconflastmarkedexit]',
				      'labelid'	=> 'meetmefeatures-admin-closeconflastmarkedexit',
				      'default'	=> $element['meetmefeatures']['admin_closeconflastmarkedexit']['default'],
				      'checked' => $info['meetmefeatures']['admin_closeconflastmarkedexit']));

if($context_list !== false):
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_admin-enableexitcontext'),
				      'name'	=> 'meetmefeatures[admin_enableexitcontext]',
				      'labelid'	=> 'meetmefeatures-admin-enableexitcontext',
				      'default'	=> $element['meetmefeatures']['admin_enableexitcontext']['default'],
				      'checked' => $info['meetmefeatures']['admin_enableexitcontext'])),

		$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_admin-exitcontext'),
				    'name'	=> 'meetmefeatures[admin_exitcontext]',
				    'labelid'	=> 'meetmefeatures-admin-exitcontext',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['meetmefeatures']['admin_exitcontext']['default'],
				    'selected'	=> $info['meetmefeatures']['admin_exitcontext']),
			      $context_list);
endif;
?>
</div>

<div id="sb-part-user" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_user-mode'),
				    'name'	=> 'meetmefeatures[user_mode]',
				    'labelid'	=> 'meetmefeatures-user-mode',
				    'key'	=> false,
				    'bbf'	=> 'fm_meetmefeatures_user-mode-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue'),
				    'default'	=> $element['meetmefeatures']['user_mode']['default'],
				    'selected'	=> $info['meetmefeatures']['user_mode']),
			      $element['meetmefeatures']['user_mode']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_user-announceusercount'),
				      'name'	=> 'meetmefeatures[user_announceusercount]',
				      'labelid'	=> 'meetmefeatures-user-announceusercount',
				      'default'	=> $element['meetmefeatures']['user_announceusercount']['default'],
				      'checked'	=> $info['meetmefeatures']['user_announceusercount'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_user-hiddencalls'),
				      'name'	=> 'meetmefeatures[user_hiddencalls]',
				      'labelid'	=> 'meetmefeatures-user-hiddencalls',
				      'default'	=> $element['meetmefeatures']['user_hiddencalls']['default'],
				      'checked'	=> $info['meetmefeatures']['user_hiddencalls'])),

		$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_user-announcejoinleave'),
				    'name'	=> 'meetmefeatures[user_announcejoinleave]',
				    'labelid'	=> 'meetmefeatures-user-announcejoinleave',
				    'key'	=> false,
				    'bbf'	=> 'fm_meetmefeatures_user-announcejoinleave-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue'),
				    'default'	=> $element['meetmefeatures']['user_announcejoinleave']['default'],
				    'selected'	=> $info['meetmefeatures']['user_announcejoinleave']),
			      $element['meetmefeatures']['user_announcejoinleave']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_user-initiallymuted'),
				      'name'	=> 'meetmefeatures[user_initiallymuted]',
				      'labelid'	=> 'meetmefeatures-user-initiallymuted',
				      'default'	=> $element['meetmefeatures']['user_initiallymuted']['default'],
				      'checked'	=> $info['meetmefeatures']['user_initiallymuted']));

if($moh_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_user-musiconhold'),
				    'name'	=> 'meetmefeatures[user_musiconhold]',
				    'labelid'	=> 'meetmefeatures-user-musiconhold',
				    'empty'	=> true,
				    'key'	=> 'category',
				    'invalid'	=> ($this->get_var('act') === 'edit'),
				    'default'	=> ($this->get_var('act') === 'add' ?
				    		    $element['meetmefeatures']['user_musiconhold']['default'] :
						    null),
				    'selected'	=> $info['meetmefeatures']['user_musiconhold']),
			      $moh_list);
endif;

	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_user-poundexit'),
				      'name'	=> 'meetmefeatures[user_poundexit]',
				      'labelid'	=> 'meetmefeatures-user-poundexit',
				      'default'	=> $element['meetmefeatures']['user_poundexit']['default'],
				      'checked'	=> $info['meetmefeatures']['user_poundexit'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_user-quiet'),
				      'name'	=> 'meetmefeatures[user_quiet]',
				      'labelid'	=> 'meetmefeatures-user-quiet',
				      'default'	=> $element['meetmefeatures']['user_quiet']['default'],
				      'checked'	=> $info['meetmefeatures']['user_quiet'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_user-starmenu'),
				      'name'	=> 'meetmefeatures[user_starmenu]',
				      'labelid'	=> 'meetmefeatures-user-starmenu',
				      'default'	=> $element['meetmefeatures']['user_starmenu']['default'],
				      'checked'	=> $info['meetmefeatures']['user_starmenu']));

if($context_list !== false):
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_user-enableexitcontext'),
				      'name'	=> 'meetmefeatures[user_enableexitcontext]',
				      'labelid'	=> 'meetmefeatures-user-enableexitcontext',
				      'default'	=> $element['meetmefeatures']['user_enableexitcontext']['default'],
				      'checked' => $info['meetmefeatures']['user_enableexitcontext'])),

		$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_user-exitcontext'),
				    'name'	=> 'meetmefeatures[user_exitcontext]',
				    'labelid'	=> 'meetmefeatures-user-exitcontext',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['meetmefeatures']['user_exitcontext']['default'],
				    'selected'	=> $info['meetmefeatures']['user_exitcontext']),
			      $context_list);
endif;
?>
</div>

<div id="sb-part-email" class="b-nodisplay">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_meetmefeatures_emailfrom'),
				  'name'	=> 'meetmefeatures[emailfrom]',
				  'labelid'	=> 'meetmefeatures-emailfrom',
				  'size'	=> 20,
				  'default'	=> $element['meetmefeatures']['emailfrom']['default'],
				  'value'	=> $info['meetmefeatures']['emailfrom'])),

		$form->text(array('desc'	=> $this->bbf('fm_meetmefeatures_emailfromname'),
				  'name'	=> 'meetmefeatures[emailfromname]',
				  'labelid'	=> 'meetmefeatures-emailfromname',
				  'size'	=> 20,
				  'default'	=> $element['meetmefeatures']['emailfromname']['default'],
				  'value'	=> $info['meetmefeatures']['emailfromname'])),

		$form->text(array('desc'	=> $this->bbf('fm_meetmefeatures_emailsubject'),
				  'name'	=> 'meetmefeatures[emailsubject]',
				  'labelid'	=> 'meetmefeatures-emailsubject',
				  'size'	=> 20,
				  'default'	=> $this->bbf('meetmefeatures_emailsubject'),
				  'value'	=> $info['meetmefeatures']['emailsubject']));
?>
	<div class="fm-paragraph fm-description">
		<p>
			<label id="lb-meetmefeatures-emailbody" for="it-meetmefeatures-emailbody">
				<?=$this->bbf('fm_meetmefeatures_emailbody');?>
			</label>
		</p>
		<?=$form->textarea(array('paragraph'	=> false,
					 'name'		=> 'meetmefeatures[emailbody]',
					 'label'	=> false,
					 'id'		=> 'it-meetmefeatures-emailbody',
					 'cols'		=> 60,
					 'rows'		=> 10,
					 'default'	=> $this->bbf('meetmefeatures_emailbody')),
				   $info['meetmefeatures']['emailbody']);?>
	</div>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/meetme/guest');
?>
</div>
