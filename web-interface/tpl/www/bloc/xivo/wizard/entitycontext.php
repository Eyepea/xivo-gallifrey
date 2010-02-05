<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

$element = $this->get_var('element');

if(($error_internal_numberbeg = $this->get_var('error','context_internal','contextnumbers')) !== null
&& is_array($error_internal_numberbeg) === true
&& isset($error_internal_numberbeg['user'][0]['numberbeg']) === true):
	$error_internal_numberbeg = $error_internal_numberbeg['user'][0]['numberbeg'];
endif;

if(($error_incall_numberbeg = $this->get_var('error','context_incall','contextnumbers')) !== null
&& is_array($error_incall_numberbeg) === true
&& isset($error_incall_numberbeg['incall'][0]['numberbeg']) === true):
	$error_incall_numberbeg = $error_incall_numberbeg['incall'][0]['numberbeg'];
endif;

?>
<fieldset id="fld-entity">
	<legend><?=$this->bbf('fld-entity');?></legend>
<?php

echo	$form->text(array('desc'	=> $this->bbf('fm_entity_name'),
			  'name'	=> 'entity[name]',
			  'labelid'	=> 'entity-name',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_entity_name'),
			  'comment'	=> $this->bbf('cmt_fm_entity_name'),
			  'default'	=> $element['entity']['name']['default'],
			  'value'	=> $this->get_var('info','entity','name'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','entity','name')))),

	$form->text(array('desc'	=> $this->bbf('fm_entity_displayname'),
			  'name'	=> 'entity[displayname]',
			  'labelid'	=> 'entity-displayname',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_entity_displayname'),
			  'comment'	=> $this->bbf('cmt_fm_entity_displayname'),
			  'default'	=> $element['entity']['displayname']['default'],
			  'value'	=> $this->get_var('info','entity','displayname'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','entity','displayname'))));

?>
</fieldset>

<fieldset id="fld-context-internal">
	<legend><?=$this->bbf('fld-context-internal');?></legend>
<?php

echo	$form->text(array('desc'	=> $this->bbf('fm_context_internal-name'),
			  'name'	=> 'context[internal][name]',
			  'labelid'	=> 'context-internal-name',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_context_internal-name'),
			  'comment'	=> $this->bbf('cmt_fm_context_internal-name'),
			  'default'	=> $element['context']['internal']['name']['default'],
			  'value'	=> $this->get_var('info','context_internal','context','name'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error',
									  'context_internal',
									  'context',
									  'name')))),

	$form->text(array('desc'	=> $this->bbf('fm_context_internal-displayname'),
			  'name'	=> 'context[internal][displayname]',
			  'labelid'	=> 'context-internal-displayname',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_context_internal-displayname'),
			  'comment'	=> $this->bbf('cmt_fm_context_internal-displayname'),
			  'default'	=> $this->bbf('fm_context_internal-displayname-default',
						      $element['context']['internal']['name']['default']),
			  'value'	=> $this->get_var('info','context_internal','context','displayname'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error',
									  'context_internal',
									  'context',
									  'displayname')))),

	$form->text(array('desc'	=> $this->bbf('fm_context_internal-numberbeg'),
			  'name'	=> 'context[internal][numberbeg]',
			  'labelid'	=> 'context-internal-numberbeg',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_context_internal-numberbeg'),
			  'comment'	=> $this->bbf('cmt_fm_context_internal-numberbeg'),
			  'default'	=> $element['context']['internal']['numberbeg']['default'],
			  'value'	=> $this->get_var('info','context_internal','contextnumbers','user',0,'numberbeg'),
			  'error'	=> $this->bbf_args('error_generic',$error_internal_numberbeg))),

	$form->text(array('desc'	=> $this->bbf('fm_context_internal-numberend'),
			  'name'	=> 'context[internal][numberend]',
			  'labelid'	=> 'context-internal-numberend',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_context_internal-numberend'),
			  'comment'	=> $this->bbf('cmt_fm_context_internal-numberend'),
			  'default'	=> $element['context']['internal']['numberend']['default'],
			  'value'	=> $this->get_var('info','context_internal','contextnumbers','user',0,'numberend'),
			  'error'	=> $this->bbf_args('error_fm_context_internal_numberend',
							   is_string($this->get_var('error',
										    'context_internal',
										    'contextnumbers',
										    'user',
										    0)))));

?>
</fieldset>

<fieldset id="fld-context-incall">
	<legend><?=$this->bbf('fld-context-incall');?></legend>
<?php

echo	$form->text(array('desc'	=> $this->bbf('fm_context_incall-name'),
			  'name'	=> 'context[incall][name]',
			  'labelid'	=> 'context-incall-name',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_context_incall-name'),
			  'comment'	=> $this->bbf('cmt_fm_context_incall-name'),
			  'default'	=> $element['context']['incall']['name']['default'],
			  'value'	=> $this->get_var('info','context_incall','context','name'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error',
									  'context_incall',
									  'context',
									  'name')))),

	$form->text(array('desc'	=> $this->bbf('fm_context_incall-displayname'),
			  'name'	=> 'context[incall][displayname]',
			  'labelid'	=> 'context-incall-displayname',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_context_incall-displayname'),
			  'comment'	=> $this->bbf('cmt_fm_context_incall-displayname'),
			  'default'	=> $this->bbf('fm_context_incall-displayname-default',
						      $element['context']['incall']['name']['default']),
			  'value'	=> $this->get_var('info','context_incall','context','displayname'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error',
									  'context_incall',
									  'context',
									  'displayname')))),

	$form->text(array('desc'	=> $this->bbf('fm_context_incall-numberbeg'),
			  'name'	=> 'context[incall][numberbeg]',
			  'labelid'	=> 'context-incall-numberbeg',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_context_incall-numberbeg'),
			  'comment'	=> $this->bbf('cmt_fm_context_incall-numberbeg'),
			  'default'	=> $element['context']['incall']['numberbeg']['default'],
			  'value'	=> $this->get_var('info','context_incall','contextnumbers','incall',0,'numberbeg'),
			  'error'	=> $this->bbf_args('error_generic',$error_incall_numberbeg))),

	$form->text(array('desc'	=> $this->bbf('fm_context_incall-numberend'),
			  'name'	=> 'context[incall][numberend]',
			  'labelid'	=> 'context-incall-numberend',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_context_incall-numberend'),
			  'comment'	=> $this->bbf('cmt_fm_context_incall-numberend'),
			  'default'	=> $element['context']['incall']['numberend']['default'],
			  'value'	=> $this->get_var('info','context_incall','contextnumbers','incall',0,'numberend'),
			  'error'	=> $this->bbf_args('error_fm_context_incall_numberend',
							   is_string($this->get_var('error',
										    'context_incall',
										    'contextnumbers',
										    'incall',
										    0))))),

	$form->select(array('desc'	=> $this->bbf('fm_context_incall-didlength'),
			    'name'	=> 'context[incall][didlength]',
			    'labelid'	=> 'context-incall-didlength',
			    'key'	=> false,
			    'help'	=> $this->bbf('hlp_fm_context_incall-didlength'),
			    'comment'	=> $this->bbf('cmt_fm_context_incall-didlength'),
			    'default'	=> $element['context']['incall']['didlength']['default'],
			    'selected'	=> $this->get_var('info','context_incall','contextnumbers','incall',0,'didlength')),
		      $element['context']['incall']['didlength']['value']);

?>
</fieldset>

<fieldset id="fld-context-outcall">
	<legend><?=$this->bbf('fld-context-outcall');?></legend>
<?php

echo	$form->text(array('desc'	=> $this->bbf('fm_context_outcall-name'),
			  'name'	=> 'context[outcall][name]',
			  'labelid'	=> 'context-outcall-name',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_context_outcall-name'),
			  'comment'	=> $this->bbf('cmt_fm_context_outcall-name'),
			  'default'	=> $element['context']['outcall']['name']['default'],
			  'value'	=> $this->get_var('info','context_outcall','context','name'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error',
									  'context_outcall',
									  'context',
									  'name')))),

	$form->text(array('desc'	=> $this->bbf('fm_context_outcall-displayname'),
			  'name'	=> 'context[outcall][displayname]',
			  'labelid'	=> 'context-outcall-displayname',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_context_outcall-displayname'),
			  'comment'	=> $this->bbf('cmt_fm_context_outcall-displayname'),
			  'default'	=> $this->bbf('fm_context_outcall-displayname-default',$element['context']['outcall']['name']['default']),
			  'value'	=> $this->get_var('info','context_outcall','context','displayname'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error',
									  'context_outcall',
									  'context',
									  'displayname'))));

?>
</fieldset>
