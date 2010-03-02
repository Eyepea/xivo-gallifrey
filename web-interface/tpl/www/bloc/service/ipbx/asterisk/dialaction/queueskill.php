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

$form 	 = &$this->get_module('form');
$url 	 = &$this->get_module('url');
$dhtml 	 = &$this->get_module('dhtml');

$element = $this->get_var('element');
$event 	 = $this->get_var('event');
$skills_tree = $this->get_var('skills_tree');

echo '<div id="fd-dialaction-',$event,'-queueskill-actiontype" class="b-nodisplay">',
     $form->select(array('desc'	=> $this->bbf('fm_dialaction_queueskill-skill'),
			    'name'	=> 'dialaction['.$event.'][skill]',
			    'labelid'	=> 'dialaction-'.$event.'-queueskill-skill',
			    'key'	=> 'name',
			    'altkey'	=> 'name',
			    'empty'     => true,
			    'optgroup'	=> array(
				'key'		=> 'category', 
				'unique' 	=> true,
			    )),
		      $skills_tree,
		     'onchange="xivo_ast_defapplication_queueskill_onskillchange(\''.$dhtml->escape($event).'\',\'it-voicemenu-flow\');"'),

     $form->text(array('desc'	=> $this->bbf('fm_dialaction_queueskill-varname'),
			  'name'	=> 'dialaction['.$event.'][varname]',
			  'labelid'	=> 'dialaction-'.$event.'-queueskill-varname',
			  'size'	=> 15,
			  'value'	=> $this->get_var('dialaction',$event,'queueskill','varname'))),

     $form->button(array('name'		=> 'add-defapplication-queueskill',
			 'id'		=> 'it-add-defapplication-queueskill',
			 'value'	=> $this->bbf('fm_bt-add')),
 		         'onclick="xivo_ast_defapplication_queueskill(\''.$dhtml->escape($event).'\',\'it-voicemenu-flow\');"'),

     '</div>';

?>
