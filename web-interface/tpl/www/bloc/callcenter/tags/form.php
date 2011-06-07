<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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
$url  = &$this->get_module('url');
$act  = &$this->get_module('act');

$info 		= $this->get_var('info');

$element 	= $this->get_var('element');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_tag_name'),
				  'name'	=> 'tag[name]',
				  'labelid'	=> 'tag-name',
					'size'	=> 32,
				  'default'	=> $element['name']['default'],
				  'value'	=> $info['tag']['name'],
			    'error'	=> $this->bbf($this->get_var('error', 'name')) )),

			$form->text(array('desc'	=> $this->bbf('fm_tag_label'),
				  'name'	=> 'tag[label]',
				  'labelid'	=> 'tag-label',
				  'size'	=> 32,
				  'default'	=> $element['label']['default'],
				  'value'	=> $info['tag']['label'],
					'error'	=> $this->bbf($this->get_var('error', 'label')) )),

			$form->select(array('desc'	=> $this->bbf('fm_tag_action'),
				    'name'	    => 'tag[action]',
				    'labelid'	  => 'tag-action',
				    'key'	      => false,
				    'bbf'      	=> 'fm_tag_action-opt',
				    'bbfopt'	  => array('argmode' => 'paramvalue'),
				    'default'	  => $element['action']['default'],
				    'selected'	=> $info['tag']['action']),
			      $element['action']['value']);
?>

</div>

