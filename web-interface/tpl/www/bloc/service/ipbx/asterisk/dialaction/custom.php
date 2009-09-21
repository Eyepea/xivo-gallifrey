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
$event = $this->get_var('event');

echo	'<div id="fd-dialaction-',$event,'-custom-actiontype" class="b-nodisplay">',
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_custom-actionarg1'),
			  'name'	=> 'dialaction['.$event.'][actionarg1]',
			  'labelid'	=> 'dialaction-'.$event.'-custom-actionarg1',
			  'size'	=> 20,
			  'value'	=> $this->get_varra('dialaction',array($event,'custom','actionarg1'))));

	if($event === 'voicemenuflow'):
		echo	$form->button(array('name'	=> 'add-defapplication-custom',
					    'id'	=> 'it-add-defapplication-custom',
					    'value'	=> $this->bbf('fm_bt-add')),
				      'onclick="xivo_ast_defapplication_custom();"');
	endif;
	echo	'</div>';

?>
