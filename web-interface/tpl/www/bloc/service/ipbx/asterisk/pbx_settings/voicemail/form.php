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
$context_list = $this->get_var('context_list');

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_voicemail_fullname'),
				  'name'	=> 'voicemail[fullname]',
				  'labelid'	=> 'voicemail-fullname',
				  'size'	=> 15,
				  'default'	=> $element['voicemail']['fullname']['default'],
				  'value' => $info['voicemail']['fullname'])),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail_mailbox'),
				  'name'	=> 'voicemail[mailbox]',
				  'labelid'	=> 'voicemail-mailbox',
				  'size'	=> 10,
				  'default'	=> $element['voicemail']['mailbox']['default'],
				  'value' => $info['voicemail']['mailbox'])),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail_password'),
				  'name'	=> 'voicemail[password]',
				  'labelid'	=> 'voicemail-password',
				  'size'	=> 10,
				  'default'	=> $element['voicemail']['password']['default'],
				  'value' => $info['voicemail']['password'])),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail_email'),
				  'name'	=> 'voicemail[email]',
				  'labelid'	=> 'voicemail-email',
				  'size'	=> 15,
				  'default'	=> $element['voicemail']['email']['default'],
				  'value' => $info['voicemail']['email']));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_voicemail_context'),
				    'name'	=> 'voicemail[context]',
				    'labelid'	=> 'voicemail-context',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['voicemail']['context']['default'],
				    'value'	=> $info['voicemail']['context']),
			      $context_list);
else:
	echo	'<div id="fd-voicemail-context" class="txt-center">',
			$url->href_html($this->bbf('create_context'),
					'service/ipbx/system_management/context',
					'act=add'),
		'</div>';
endif;

if(($tz_list = $this->get_var('tz_list')) !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_voicemail_tz'),
				    'name'	=> 'voicemail[tz]',
				    'labelid'	=> 'voicemail-tz',
				    'key'	=> 'name',
				    'default'	=> $element['voicemail']['tz']['default'],
				    'value'	=> $info['voicemail']['tz']),
			      $tz_list);
endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_voicemail_language'),
				    'name'	=> 'voicemail[language]',
				    'labelid'	=> 'voicemail-language',
				    'key'	=> false,
				    'empty'	=> true,
				    'default'	=> $element['voicemail']['language']['default'],
				    'value'	=> $info['voicemail']['language']),
			      $element['voicemail']['language']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemailfeatures_skipcheckpass'),
				      'name'	=> 'vmfeatures[skipcheckpass]',
				      'labelid'	=> 'vmfeatures-skipcheckpass',
				      'default'	=> $element['vmfeatures']['skipcheckpass']['default'],
				      'checked'	=> $info['vmfeatures']['skipcheckpass'])),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_maxmsg'),
				    'name'	=> 'voicemail[maxmsg]',
				    'labelid'	=> 'voicemail-maxmsg',
				    'empty'	=> true,
				    'key'	=> false,
				    'value'	=> $info['voicemail']['maxmsg'],
				    'default'	=> $element['voicemail']['maxmsg']['default']),
			      $element['voicemail']['maxmsg']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_attach'),
				    'name'	=> 'voicemail[attach]',
				    'labelid'	=> 'voicemail-attach',
				    'bbf'	=> 'fm_bool-opt-',
				    'empty'	=> true,
				    'key'	=> false,
				    'value'	=> $info['voicemail']['attach'],
				    'default'	=> $element['voicemail']['attach']['default']),
			      $element['voicemail']['attach']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail_deletevoicemail'),
				      'name'	=> 'voicemail[deletevoicemail]',
				      'labelid'	=> 'voicemail-deletevoicemail',
				      'default'	=> $element['voicemail']['deletevoicemail']['default'],
				      'checked'	=> $info['voicemail']['deletevoicemail']));
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_voicemail_pager'),
				  'name'	=> 'voicemail[pager]',
				  'labelid'	=> 'voicemail-pager',
				  'size'	=> 10,
				  'default'	=> $element['voicemail']['pager']['default'],
				  'value' => $info['voicemail']['pager'])),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_saycid'),
				    'name'	=> 'voicemail[saycid]',
				    'labelid'	=> 'voicemail-saycid',
				    'bbf'	=> 'fm_bool-opt-',
				    'empty'	=> true,
				    'key'	=> false,
				    'value'	=> $info['voicemail']['saycid'],
				    'default'	=> $element['voicemail']['saycid']['default']),
			      $element['voicemail']['saycid']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_review'),
				    'name'	=> 'voicemail[review]',
				    'labelid'	=> 'voicemail-review',
				    'bbf'	=> 'fm_bool-opt-',
				    'empty'	=> true,
				    'key'	=> false,
				    'value'	=> $info['voicemail']['review'],
				    'default'	=> $element['voicemail']['review']['default']),
			      $element['voicemail']['review']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_operator'),
				    'name'	=> 'voicemail[operator]',
				    'labelid'	=> 'voicemail-operator',
				    'bbf'	=> 'fm_bool-opt-',
				    'empty'	=> true,
				    'key'	=> false,
				    'value'	=> $info['voicemail']['operator'],
				    'default'	=> $element['voicemail']['operator']['default']),
			      $element['voicemail']['operator']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_envelope'),
				    'name'	=> 'voicemail[envelope]',
				    'labelid'	=> 'voicemail-envelope',
				    'bbf'	=> 'fm_bool-opt-',
				    'empty'	=> true,
				    'key'	=> false,
				    'value'	=> $info['voicemail']['envelope'],
				    'default'	=> $element['voicemail']['envelope']['default']),
			      $element['voicemail']['envelope']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_sayduration'),
				    'name'	=> 'voicemail[sayduration]',
				    'labelid'	=> 'voicemail-sayduration',
				    'bbf'	=> 'fm_bool-opt-',
				    'empty'	=> true,
				    'key'	=> false,
				    'value'	=> $info['voicemail']['sayduration'],
				    'default'	=> $element['voicemail']['sayduration']['default']),
			      $element['voicemail']['sayduration']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_saydurationm'),
				    'name'	=> 'voicemail[saydurationm]',
				    'labelid'	=> 'voicemail-saydurationm',
				    'bbf'	=> array('paramkey','fm_voicemail_saydurationm-opt'),
				    'empty'	=> true,
				    'key'	=> false,
				    'value'	=> $info['voicemail']['saydurationm'],
				    'default'	=> $element['voicemail']['saydurationm']['default']),
			      $element['voicemail']['saydurationm']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_sendvoicemail'),
				    'name'	=> 'voicemail[sendvoicemail]',
				    'labelid'	=> 'voicemail-sendvoicemail',
				    'bbf'	=> 'fm_bool-opt-',
				    'empty'	=> true,
				    'key'	=> false,
				    'value'	=> $info['voicemail']['sendvoicemail'],
				    'default'	=> $element['voicemail']['sendvoicemail']['default']),
			      $element['voicemail']['sendvoicemail']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_forcename'),
				    'name'	=> 'voicemail[forcename]',
				    'labelid'	=> 'voicemail-forcename',
				    'bbf'	=> 'fm_bool-opt-',
				    'empty'	=> true,
				    'key'	=> false,
				    'value'	=> $info['voicemail']['forcename'],
				    'default'	=> $element['voicemail']['forcename']['default']),
			      $element['voicemail']['forcename']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_forcegreetings'),
				    'name'	=> 'voicemail[forcegreetings]',
				    'labelid'	=> 'voicemail-forcegreetings',
				    'bbf'	=> 'fm_bool-opt-',
				    'empty'	=> true,
				    'key'	=> false,
				    'value'	=> $info['voicemail']['forcegreetings'],
				    'default'	=> $element['voicemail']['forcegreetings']['default']),
			      $element['voicemail']['forcegreetings']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail_hidefromdir'),
				      'name'	=> 'voicemail[hidefromdir]',
				      'labelid'	=> 'voicemail-hidefromdir',
				      'default'	=> $element['voicemail']['hidefromdir']['default'],
				      'checked' => $info['voicemail']['hidefromdir']));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_voicemail_dialout'),
				    'name'	=> 'voicemail[dialout]',
				    'labelid'	=> 'voicemail-dialout',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'empty'	=> true,
				    'default'	=> $element['voicemail']['dialout']['default'],
				    'value'	=> $info['voicemail']['dialout']),
			      $context_list),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_callback'),
				    'name'	=> 'voicemail[callback]',
				    'labelid'	=> 'voicemail-callback',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'empty'	=> true,
				    'default'	=> $element['voicemail']['callback']['default'],
				    'value'	=> $info['voicemail']['callback']),
			      $context_list),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_exitcontext'),
				    'name'	=> 'voicemail[exitcontext]',
				    'labelid'	=> 'voicemail-exitcontext',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'empty'	=> true,
				    'default'	=> $element['voicemail']['exitcontext']['default'],
				    'value'	=> $info['voicemail']['exitcontext']),
			      $context_list);
endif;

?>
</div>
