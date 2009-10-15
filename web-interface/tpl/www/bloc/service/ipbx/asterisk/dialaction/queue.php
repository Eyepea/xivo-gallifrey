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
$list = $this->get_var('destination_list','queues');
$event = $this->get_var('event');

$linked = $this->get_var('dialaction',$event,'linked');
$action = $this->get_var('dialaction',$event,'action');

if(empty($list) === false):
	echo	'<div id="fd-dialaction-'.$event.'-queue-actiontype" class="b-nodisplay">',
		$form->select(array('desc'	=> $this->bbf('fm_dialaction_queue-actionarg1'),
				    'name'	=> 'dialaction['.$event.'][actionarg1]',
				    'labelid'	=> 'dialaction-'.$event.'-queue-actionarg1',
				    'key'	=> 'identity',
				    'altkey'	=> 'id',
				    'invalid'	=> ($linked === false && $action === 'queue'),
				    'default'	=> $element['dialaction']['actionarg1']['default'],
				    'selected'	=> $this->get_var('dialaction',$event,'queue','actionarg1')),
			      $list),
		$form->text(array('desc'	=> $this->bbf('fm_dialaction_queue-actionarg2'),
				  'name'	=> 'dialaction['.$event.'][actionarg2]',
				  'labelid'	=> 'dialaction-'.$event.'-queue-actionarg2',
				  'size'	=> 10,
				  'value'	=> $this->get_var('dialaction',$event,'queue','actionarg2')));

	if($event === 'voicemenuflow'):
		echo	$form->button(array('name'	=> 'add-defapplication-queue',
					    'id'	=> 'it-add-defapplication-queue',
					    'value'	=> $this->bbf('fm_bt-add')),
				      'onclick="xivo_ast_defapplication_queue(\''.$dhtml->escape($event).'\',\'it-voicemenu-flow\');"');
	elseif($event === 'voicemenuevent'):
		echo	$form->button(array('name'	=> 'select-defapplication-queue',
					    'id'	=> 'it-select-defapplication-queue',
					    'value'	=> $this->bbf('fm_bt-select')),
				      'onclick="xivo_ast_voicemenuevent_defapplication(\'queue\');"');
	endif;
	echo	'</div>';
else:
	echo	'<div id="fd-dialaction-'.$event.'-queue-actiontype" class="txt-center b-nodisplay">';
	if($this->get_var('dialaction_from') === 'queue'):
		echo	$this->bbf('dialaction_no-queue');
	else:
		echo	$url->href_html($this->bbf('create_queue'),'service/ipbx/pbx_settings/queues','act=add');
	endif;
	echo	'</div>';
endif;

?>
