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
$url = &$this->get_module('url');

$element = $this->get_var('element');
$info = $this->get_var('info');
$urilist = $this->get_var('urilist');

$presence = $this->get_var('directories');

#$queues = $this->get_var('queues');
#$qmember = $this->get_var('qmember');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_name'),
				  'name'	=> 'directories[name]',
				  'labelid'	=> 'directories-name',
				  'size'	=> 15,
				  'default'	=> $element['directories']['name']['default'],
				  'value'	=> $info['directories']['name']));

	echo	$form->select(array('desc'	=> $this->bbf('fm_directories_uri'),
				  'name'	=> 'directories-uri',
				  'labelid'	=> 'directories-uri',
				  'default'	=> $element['directories']['uri']['default'],
				  'key'	=> false,
				  'selected'	=> $info['directories']['uri']),
				  $urilist);

	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_delimiter'),
				  'name'	=> 'directories[delimiter]',
				  'labelid'	=> 'directories-delimiter',
				  'size'	=> 1,
				  'default'	=> $element['directories']['delimiter']['default'],
				  'value'	=> $info['directories']['delimiter']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_match_direct'),
				  'name'	=> 'directories[match_direct]',
				  'labelid'	=> 'directories-match_direct',
				  'size'	=> 40,
				  'default'	=> $element['directories']['match_direct']['default'],
				  'value'	=> $info['directories']['match_direct']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_match_reverse'),
				  'name'	=> 'directories[match_reverse]',
				  'labelid'	=> 'directories-match_reverse',
				  'size'	=> 40,
				  'default'	=> $element['directories']['match_reverse']['default'],
				  'value'	=> $info['directories']['match_reverse']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_field_phone'),
				  'name'	=> 'directories[field_phone]',
				  'labelid'	=> 'directories-field_phone',
				  'size'	=> 40,
				  'default'	=> $element['directories']['field_phone']['default'],
				  'value'	=> $info['directories']['field_phone']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_field_firstname'),
				  'name'	=> 'directories[field_firstname]',
				  'labelid'	=> 'directories-field_firstname',
				  'size'	=> 40,
				  'default'	=> $element['directories']['field_firstname']['default'],
				  'value'	=> $info['directories']['field_firstname']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_field_lastname'),
				  'name'	=> 'directories[field_lastname]',
				  'labelid'	=> 'directories-field_lastname',
				  'size'	=> 40,
				  'default'	=> $element['directories']['field_lastname']['default'],
				  'value'	=> $info['directories']['field_lastname']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_field_fullname'),
				  'name'	=> 'directories[field_fullname]',
				  'labelid'	=> 'directories-field_fullname',
				  'size'	=> 40,
				  'default'	=> $element['directories']['field_fullname']['default'],
				  'value'	=> $info['directories']['field_fullname']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_field_company'),
				  'name'	=> 'directories[field_company]',
				  'labelid'	=> 'directories-field_company',
				  'size'	=> 40,
				  'default'	=> $element['directories']['field_company']['default'],
				  'value'	=> $info['directories']['field_company']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_field_mail'),
				  'name'	=> 'directories[field_mail]',
				  'labelid'	=> 'directories-field_mail',
				  'size'	=> 40,
				  'default'	=> $element['directories']['field_mail']['default'],
				  'value'	=> $info['directories']['field_mail']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_directories_display_reverse'),
				  'name'	=> 'directories[display_reverse]',
				  'labelid'	=> 'directories-display_reverse',
				  'size'	=> 40,
				  'default'	=> $element['directories']['display_reverse']['default'],
				  'value'	=> $info['directories']['display_reverse']));

?>
</div>
<div class="fm-paragraph fm-description">
	<p>
		<label id="lb-description" for="it-description"><?=$this->bbf('fm_description');?></label>
	</p>
	<?=$form->textarea(array('paragraph'    => false,
				 'label'    => false,
				 'name'     => 'directories[description]',
				 'id'       => 'it-description',
				 'cols'     => 60,
				 'rows'     => 5,
				 'default'  => $element['directories']['description']['default']),
			   $info['directories']['description']);?>
</div>

