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
$dhtml = &$this->get_module('dhtml');

$element = $this->get_var('element');
$list = $this->get_var('destination_list','voicemail');
$event = $this->get_var('event');

$linked = $this->get_var('dialaction',$event,'linked');
$action = $this->get_var('dialaction',$event,'action');

if(empty($list) === false):
	echo	'<div id="fd-dialaction-'.$event.'-voicemail-actiontype" class="b-nodisplay">',
		$form->select(array('desc'	=> $this->bbf('fm_dialaction_voicemail-actionarg1'),
				    'name'	=> 'dialaction['.$event.'][actionarg1]',
				    'labelid'	=> 'dialaction-'.$event.'-voicemail-actionarg1',
				    'key'	=> 'identity',
				    'altkey'	=> 'uniqueid',
				    'invalid'	=> ($linked === false && $action === 'voicemail'),
				    'default'	=> $element['dialaction']['actionarg1']['default'],
				    'selected'	=> $this->get_var('dialaction',$event,'voicemail','actionarg1')),
			      $list),

		$form->checkbox(array('desc'	=> $this->bbf('fm_dialaction_voicemail-actionarg2-b'),
				      'name'	=> 'dialaction['.$event.'][actionarg2][b]',
				      'labelid'	=> 'dialaction-'.$event.'-voicemail-actionarg2-b',
				      'checked'	=> $this->get_var('dialaction',$event,'voicemail','actionarg2','b'),
				      'value'	=> 'b')),

		$form->checkbox(array('desc'	=> $this->bbf('fm_dialaction_voicemail-actionarg2-s'),
				      'name'	=> 'dialaction['.$event.'][actionarg2][s]',
				      'labelid'	=> 'dialaction-'.$event.'-voicemail-actionarg2-s',
				      'checked'	=> $this->get_var('dialaction',$event,'voicemail','actionarg2','s'),
				      'value'	=> 's')),

		$form->checkbox(array('desc'	=> $this->bbf('fm_dialaction_voicemail-actionarg2-u'),
				      'name'	=> 'dialaction['.$event.'][actionarg2][u]',
				      'labelid'	=> 'dialaction-'.$event.'-voicemail-actionarg2-u',
				      'checked'	=> $this->get_var('dialaction',$event,'voicemail','actionarg2','u'),
				      'value'	=> 'u')),

		$form->checkbox(array('desc'	=> $this->bbf('fm_dialaction_voicemail-actionarg2-j'),
				      'name'	=> 'dialaction['.$event.'][actionarg2][j]',
				      'labelid'	=> 'dialaction-'.$event.'-voicemail-actionarg2-j',
				      'checked'	=> $this->get_var('dialaction',$event,'voicemail','actionarg2','j'),
				      'value'	=> 'j'));

	if($event === 'voicemenuflow'):
		echo	$form->button(array('name'	=> 'add-defapplication-voicemail',
					    'id'	=> 'it-add-defapplication-voicemail',
					    'value'	=> $this->bbf('fm_bt-add')),
				      'onclick="xivo_ast_defapplication_voicemail(\''.$dhtml->escape($event).'\',\'it-voicemenu-flow\');"');
	elseif($event === 'voicemenuevent'):
		echo	$form->button(array('name'	=> 'select-defapplication-voicemail',
					    'id'	=> 'it-select-defapplication-voicemail',
					    'value'	=> $this->bbf('fm_bt-select')),
				      'onclick="xivo_ast_voicemenuevent_defapplication(\'voicemail\');"');
	endif;
	echo	'</div>';
else:
	echo	'<div id="fd-dialaction-'.$event.'-voicemail-actiontype" class="txt-center b-nodisplay">',
		$url->href_html($this->bbf('create_voicemail'),'service/ipbx/pbx_settings/voicemail','act=add'),
		'</div>';
endif;

?>
