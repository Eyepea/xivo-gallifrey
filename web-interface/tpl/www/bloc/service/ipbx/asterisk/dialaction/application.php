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
$dhtml = &$this->get_module('dhtml');

$element = $this->get_var('element');
$event = $this->get_var('event');

if($this->get_var('dialaction_from') === 'incall' && $event === 'answer'):
	$onchange = 'xivo_ast_incall_chg_dialaction_actionarg_answer_application();';
else:
	$onchange = 'xivo_ast_chg_dialaction_actionarg(\''.$dhtml->escape($event).'\',\'application\');';
endif;

echo	'<div id="fd-dialaction-',$event,'-application-actiontype" class="b-nodisplay">',
	$form->select(array('desc'	=> $this->bbf('fm_dialaction_application-action'),
			    'name'	=> 'dialaction['.$event.'][action]',
			    'labelid'	=> 'dialaction-'.$event.'-application-action',
			    'key'	=> false,
			    'bbf'	=> 'fm_dialaction_application-action-opt',
			    'bbfopt'	=> array('argmode' => 'paramvalue'),
			    'default'	=> $element['dialaction']['application']['default'],
			    'selected'	=> $this->get_var('dialaction',$event,'application','action')),
		      $element['dialaction']['application']['value'],
		      'onchange="'.$onchange.'"'),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_application-callbackdisa-actionarg1'),
			  'name'	=> 'dialaction['.$event.'][actionarg1]',
			  'labelid'	=> 'dialaction-'.$event.'-application-callbackdisa-actionarg1',
			  'size'	=> 15,
			  'value'	=> $this->get_var('dialaction',$event,'callbackdisa','actionarg1'))),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_application-callbackdisa-actionarg2'),
			  'name'	=> 'dialaction['.$event.'][actionarg2]',
			  'labelid'	=> 'dialaction-'.$event.'-application-callbackdisa-actionarg2',
			  'size'	=> 15,
			  'value'	=> $this->get_var('dialaction',$event,'callbackdisa','actionarg2'))),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_application-disa-actionarg1'),
			  'name'	=> 'dialaction['.$event.'][actionarg1]',
			  'labelid'	=> 'dialaction-'.$event.'-application-disa-actionarg1',
			  'size'	=> 15,
			  'value'	=> $this->get_var('dialaction',$event,'disa','actionarg1'))),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_application-disa-actionarg2'),
			  'name'	=> 'dialaction['.$event.'][actionarg2]',
			  'labelid'	=> 'dialaction-'.$event.'-application-disa-actionarg2',
			  'size'	=> 15,
			  'value'	=> $this->get_var('dialaction',$event,'disa','actionarg2'))),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_application-directory-actionarg1'),
			  'name'	=> 'dialaction['.$event.'][actionarg1]',
			  'labelid'	=> 'dialaction-'.$event.'-application-directory-actionarg1',
			  'size'	=> 15,
			  'value'	=> $this->get_var('dialaction',$event,'directory','actionarg1'))),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_application-faxtomail-actionarg1'),
			  'name'	=> 'dialaction['.$event.'][actionarg1]',
			  'labelid'	=> 'dialaction-'.$event.'-application-faxtomail-actionarg1',
			  'size'	=> 15,
			  'value'	=> $this->get_var('dialaction',$event,'faxtomail','actionarg1'))),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_application-voicemailmain-actionarg1'),
			  'name'	=> 'dialaction['.$event.'][actionarg1]',
			  'labelid'	=> 'dialaction-'.$event.'-application-voicemailmain-actionarg1',
			  'size'	=> 15,
			  'value'	=> $this->get_var('dialaction',$event,'voicemailmain','actionarg1')));

	if($event === 'voicemenuflow'):
		echo	$form->button(array('name'	=> 'add-defapplication-application',
					    'id'	=> 'it-add-defapplication-application',
					    'value'	=> $this->bbf('fm_bt-add')),
				      'onclick="xivo_ast_defapplication_application(\''.$dhtml->escape($event).'\',\'it-voicemenu-flow\');"');
	elseif($event === 'voicemenuevent'):
		echo	$form->button(array('name'	=> 'select-defapplication-application',
					    'id'	=> 'it-select-defapplication-application',
					    'value'	=> $this->bbf('fm_bt-select')),
				      'onclick="xivo_ast_voicemenuevent_defapplication(\'application\');"');
	endif;

echo	'</div>';
?>
