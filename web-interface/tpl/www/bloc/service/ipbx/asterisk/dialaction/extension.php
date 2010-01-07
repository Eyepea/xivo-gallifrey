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
$dhtml = &$this->get_module('dhtml');

$element = $this->get_var('element');
$event = $this->get_var('event');

echo	'<div id="fd-dialaction-',$event,'-extension-actiontype" class="b-nodisplay">',
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_extension-actionarg1'),
			  'name'	=> 'dialaction['.$event.'][actionarg1]',
			  'labelid'	=> 'dialaction-'.$event.'-extension-actionarg1',
			  'size'	=> 15,
			  'value'	=> $this->get_var('dialaction',$event,'extension','actionarg1'))),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_extension-actionarg2'),
			  'name'	=> 'dialaction['.$event.'][actionarg2]',
			  'labelid'	=> 'dialaction-'.$event.'-extension-actionarg2',
			  'size'	=> 15,
			  'value'	=> $this->get_var('dialaction',$event,'extension','actionarg2')));

	if($event === 'voicemenuflow'):
		echo	$form->button(array('name'	=> 'add-defapplication-extension',
					    'id'	=> 'it-add-defapplication-extension',
					    'value'	=> $this->bbf('fm_bt-add')),
				      'onclick="xivo_ast_defapplication_extension(\''.$dhtml->escape($event).'\',\'it-voicemenu-flow\');"');
	elseif($event === 'voicemenuevent'):
		echo	$form->button(array('name'	=> 'select-defapplication-extension',
					    'id'	=> 'it-select-defapplication-extension',
					    'value'	=> $this->bbf('fm_bt-select')),
				      'onclick="xivo_ast_voicemenuevent_defapplication(\'extension\');"');
	endif;

echo	'</div>';

?>
