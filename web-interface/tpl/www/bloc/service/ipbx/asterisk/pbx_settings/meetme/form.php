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
$moh_list = $this->get_var('moh_list');
$context_list = $this->get_var('context_list');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_meetmefeatures_name'),
				  'name'	=> 'meetmefeatures[name]',
				  'labelid'	=> 'meetmefeatures-name',
				  'size'	=> 15,
				  'default'	=> $element['meetmefeatures']['name']['default'],
				  'value'	=> $info['meetmefeatures']['name'])),

		$form->text(array('desc'	=> $this->bbf('fm_meetmeroom_number'),
				  'name'	=> 'meetmeroom[number]',
				  'labelid'	=> 'meetmeroom-number',
				  'label'	=> 'lb-meetmeroom-number',
				  'size'	=> 15,
				  'default'	=> $element['meetmeroom']['number']['default'],
				  'value'	=> $info['meetmeroom']['number'])),

		$form->text(array('desc'	=> $this->bbf('fm_meetmeroom_pin'),
				  'name'	=> 'meetmeroom[pin]',
				  'labelid'	=> 'meetmeroom-pin',
				  'size'	=> 15,
				  'default'	=> $element['meetmeroom']['pin']['default'],
				  'value'	=> $info['meetmeroom']['pin'])),

		$form->text(array('desc'	=> $this->bbf('fm_meetmeroom_admin-pin'),
				  'name'	=> 'meetmeroom[admin-pin]',
				  'labelid'	=> 'meetmeroom-admin-pin',
				  'size'	=> 15,
				  'default'	=> $element['meetmeroom']['admin-pin']['default'],
				  'value'	=> $info['meetmeroom']['admin-pin'])),

		$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_mode'),
				    'name'	=> 'meetmefeatures[mode]',
				    'labelid'	=> 'meetmefeatures-mode',
				    'key'	=> false,
				    'bbf'	=> 'fm_meetmefeatures_mode-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue'),
				    'default'	=> $element['meetmefeatures']['mode']['default'],
				    'selected'	=> $info['meetmefeatures']['mode']),
			      $element['meetmefeatures']['mode']['value']);

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

if($moh_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_musiconhold'),
				    'name'	=> 'meetmefeatures[musiconhold]',
				    'labelid'	=> 'meetmefeatures-musiconhold',
				    'empty'	=> true,
				    'key'	=> 'category',
				    'invalid'	=> ($this->get_var('act') === 'edit'),
				    'default'	=> ($this->get_var('act') === 'add' ? $element['meetmefeatures']['musiconhold']['default'] : null),
				    'selected'	=> $info['meetmefeatures']['musiconhold']),
			      $moh_list);
endif;

	echo	$form->text(array('desc'	=> $this->bbf('fm_meetmefeatures_preprocess-subroutine'),
				  'name'	=> 'meetmefeatures[preprocess_subroutine]',
				  'labelid'	=> 'meetmefeatures-preprocess-subroutine',
				  'size'	=> 15,
				  'default'	=> $element['meetmefeatures']['preprocess_subroutine']['default'],
				  'value'	=> $info['meetmefeatures']['preprocess_subroutine']));
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_poundexit'),
				      'name'	=> 'meetmefeatures[poundexit]',
				      'labelid'	=> 'meetmefeatures-poundexit',
				      'default'	=> $element['meetmefeatures']['poundexit']['default'],
				      'checked' => $info['meetmefeatures']['poundexit'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_quiet'),
				      'name'	=> 'meetmefeatures[quiet]',
				      'labelid'	=> 'meetmefeatures-quiet',
				      'default'	=> $element['meetmefeatures']['quiet']['default'],
				      'checked' => $info['meetmefeatures']['quiet'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_record'),
				      'name'	=> 'meetmefeatures[record]',
				      'labelid'	=> 'meetmefeatures-record',
				      'default'	=> $element['meetmefeatures']['record']['default'],
				      'checked' => $info['meetmefeatures']['record'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_adminmode'),
				      'name'	=> 'meetmefeatures[adminmode]',
				      'labelid'	=> 'meetmefeatures-adminmode',
				      'default'	=> $element['meetmefeatures']['adminmode']['default'],
				      'checked' => $info['meetmefeatures']['adminmode'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_announceusercount'),
				      'name'	=> 'meetmefeatures[announceusercount]',
				      'labelid'	=> 'meetmefeatures-announceusercount',
				      'default'	=> $element['meetmefeatures']['announceusercount']['default'],
				      'checked' => $info['meetmefeatures']['announceusercount'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_announcejoinleave'),
				      'name'	=> 'meetmefeatures[announcejoinleave]',
				      'labelid'	=> 'meetmefeatures-announcejoinleave',
				      'default'	=> $element['meetmefeatures']['announcejoinleave']['default'],
				      'checked' => $info['meetmefeatures']['announcejoinleave'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_alwayspromptpin'),
				      'name'	=> 'meetmefeatures[alwayspromptpin]',
				      'labelid'	=> 'meetmefeatures-alwayspromptpin',
				      'default'	=> $element['meetmefeatures']['alwayspromptpin']['default'],
				      'checked' => $info['meetmefeatures']['alwayspromptpin'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_starmenu'),
				      'name'	=> 'meetmefeatures[starmenu]',
				      'labelid'	=> 'meetmefeatures-starmenu',
				      'default'	=> $element['meetmefeatures']['starmenu']['default'],
				      'checked' => $info['meetmefeatures']['starmenu']));

if($context_list !== false):
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_enableexitcontext'),
				      'name'	=> 'meetmefeatures[enableexitcontext]',
				      'labelid'	=> 'meetmefeatures-enableexitcontext',
				      'default'	=> $element['meetmefeatures']['enableexitcontext']['default'],
				      'checked' => $info['meetmefeatures']['enableexitcontext'])),

		$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_exitcontext'),
				    'name'	=> 'meetmefeatures[exitcontext]',
				    'labelid'	=> 'meetmefeatures-exitcontext',
				    'empty'	=> true,
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['meetmefeatures']['exitcontext']['default'],
				    'selected'	=> $info['meetmefeatures']['exitcontext']),
			      $context_list);
endif;
?>
</div>
