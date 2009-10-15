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

$element = $this->get_var('element');
$event = $this->get_var('event');
$action = $this->get_var('dialaction',$event,'action');

if($this->get_var('dialaction_from') === 'incall' && $event === 'answer'):
	$onchange = 'xivo_ast_incall_chg_dialaction_answer(this);';
else:
	$onchange = 'xivo_ast_chg_dialaction(\''.$event.'\',this);';
endif;

echo $form->select(array('desc'		=> $this->bbf('fm_dialaction_actiontype'),
			 'name'		=> 'dialaction['.$event.'][actiontype]',
			 'labelid'	=> 'dialaction-'.$event.'-actiontype',
			 'key'		=> false,
			 'bbf'		=> 'fm_dialaction_actiontype-opt',
			 'bbfopt'	=> array('argmode'	=> 'paramvalue',
						 'paramsupp'	=> array('ipbx_label' => XIVO_SRE_IPBX_LABEL)),
			 'default'	=> $element['dialaction']['actiontype']['default'],
			 'selected'	=> $action),
		   $element['dialaction']['actiontype']['value'],
		   'onchange="'.$onchange.'"');

?>
