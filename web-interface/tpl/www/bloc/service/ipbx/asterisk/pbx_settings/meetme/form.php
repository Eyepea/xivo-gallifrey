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
				  'name'	=> 'mfeatures[name]',
				  'labelid'	=> 'mfeatures-name',
				  'size'	=> 15,
				  'default'	=> $element['mfeatures']['name']['default'],
				  'value'	=> $info['mfeatures']['name'])),

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
				    'name'	=> 'mfeatures[mode]',
				    'labelid'	=> 'mfeatures-mode',
				    'bbf'	=> 'fm_meetmefeatures_mode-',
				    'key'	=> false,
				    'default'	=> $element['mfeatures']['mode']['default'],
				    'value'	=> $info['mfeatures']['mode']),
			      $element['mfeatures']['mode']['value']);

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_context'),
				    'name'	=> 'mfeatures[context]',
				    'labelid'	=> 'mfeatures-context',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['mfeatures']['context']['default'],
				    'value'	=> $info['mfeatures']['context']),
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
				    'name'	=> 'mfeatures[musiconhold]',
				    'labelid'	=> 'mfeatures-musiconhold',
				    'key'	=> 'category',
				    'empty'	=> true,
				    'invalid'	=> ($this->get_var('act') === 'edit'),
				    'default'	=> ($this->get_var('act') === 'add' ? $element['mfeatures']['musiconhold']['default'] : null),
				    'value'	=> $info['mfeatures']['musiconhold']),
			      $moh_list);
endif;

	echo	$form->text(array('desc'	=> $this->bbf('fm_meetmefeatures_preprocess-subroutine'),
				  'name'	=> 'mfeatures[preprocess_subroutine]',
				  'labelid'	=> 'mfeatures-preprocess-subroutine',
				  'size'	=> 15,
				  'default'	=> $element['mfeatures']['preprocess_subroutine']['default'],
				  'value'	=> $info['mfeatures']['preprocess_subroutine']));
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_poundexit'),
				      'name'	=> 'mfeatures[poundexit]',
				      'labelid'	=> 'mfeatures-poundexit',
				      'default'	=> $element['mfeatures']['poundexit']['default'],
				      'checked' => $info['mfeatures']['poundexit'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_quiet'),
				      'name'	=> 'mfeatures[quiet]',
				      'labelid'	=> 'mfeatures-quiet',
				      'default'	=> $element['mfeatures']['quiet']['default'],
				      'checked' => $info['mfeatures']['quiet'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_record'),
				      'name'	=> 'mfeatures[record]',
				      'labelid'	=> 'mfeatures-record',
				      'default'	=> $element['mfeatures']['record']['default'],
				      'checked' => $info['mfeatures']['record'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_adminmode'),
				      'name'	=> 'mfeatures[adminmode]',
				      'labelid'	=> 'mfeatures-adminmode',
				      'default'	=> $element['mfeatures']['adminmode']['default'],
				      'checked' => $info['mfeatures']['adminmode'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_announceusercount'),
				      'name'	=> 'mfeatures[announceusercount]',
				      'labelid'	=> 'mfeatures-announceusercount',
				      'default'	=> $element['mfeatures']['announceusercount']['default'],
				      'checked' => $info['mfeatures']['announceusercount'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_announcejoinleave'),
				      'name'	=> 'mfeatures[announcejoinleave]',
				      'labelid'	=> 'mfeatures-announcejoinleave',
				      'default'	=> $element['mfeatures']['announcejoinleave']['default'],
				      'checked' => $info['mfeatures']['announcejoinleave'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_alwayspromptpin'),
				      'name'	=> 'mfeatures[alwayspromptpin]',
				      'labelid'	=> 'mfeatures-alwayspromptpin',
				      'default'	=> $element['mfeatures']['alwayspromptpin']['default'],
				      'checked' => $info['mfeatures']['alwayspromptpin'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_starmenu'),
				      'name'	=> 'mfeatures[starmenu]',
				      'labelid'	=> 'mfeatures-starmenu',
				      'default'	=> $element['mfeatures']['starmenu']['default'],
				      'checked' => $info['mfeatures']['starmenu']));

if($context_list !== false):
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_meetmefeatures_enableexitcontext'),
				      'name'	=> 'mfeatures[enableexitcontext]',
				      'labelid'	=> 'mfeatures-enableexitcontext',
				      'default'	=> $element['mfeatures']['enableexitcontext']['default'],
				      'checked' => $info['mfeatures']['enableexitcontext'])),

		$form->select(array('desc'	=> $this->bbf('fm_meetmefeatures_exitcontext'),
				    'name'	=> 'mfeatures[exitcontext]',
				    'labelid'	=> 'mfeatures-exitcontext',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'empty'	=> true,
				    'default'	=> $element['mfeatures']['exitcontext']['default'],
				    'value'	=> $info['mfeatures']['exitcontext']),
			      $context_list);
endif;
?>
</div>
