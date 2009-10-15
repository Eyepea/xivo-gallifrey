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

$info = $this->get_var('info');
$element = $this->get_var('element');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_phonebook_title'),
				    'name'	=> 'phonebook[title]',
				    'labelid'	=> 'phonebook-title',
				    'key'	=> false,
				    'bbf'	=> 'fm_phonebook_title-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $info['phonebook']['title'],
				    'default'	=> $element['phonebook']['title']['default']),
			      $element['phonebook']['title']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_phonebook_firstname'),
				  'name'	=> 'phonebook[firstname]',
				  'labelid'	=> 'phonebook-firstname',
				  'size'	=> 15,
				  'default'	=> $element['phonebook']['firstname']['default'],
				  'value'	=> $info['phonebook']['firstname'])),

		$form->text(array('desc'	=> $this->bbf('fm_phonebook_lastname'),
				  'name'	=> 'phonebook[lastname]',
				  'labelid'	=> 'phonebook-lastname',
				  'size'	=> 15,
				  'default'	=> $element['phonebook']['lastname']['default'],
				  'value'	=> $info['phonebook']['lastname'])),

		$form->text(array('desc'	=> $this->bbf('fm_phonebook_displayname'),
				  'name'	=> 'phonebook[displayname]',
				  'labelid'	=> 'phonebook-displayname',
				  'size'	=> 15,
				  'default'	=> $element['phonebook']['displayname']['default'],
				  'value'	=> $info['phonebook']['displayname'])),

		$form->text(array('desc'	=> $this->bbf('fm_phonebook_society'),
				  'name'	=> 'phonebook[society]',
				  'labelid'	=> 'phonebook-society',
				  'size'	=> 15,
				  'default'	=> $element['phonebook']['society']['default'],
				  'value'	=> $info['phonebook']['society'])),

		$form->text(array('desc'	=> $this->bbf('fm_phonebooknumber_mobile'),
				  'name'	=> 'phonebooknumber[mobile]',
				  'labelid'	=> 'phonebooknumber-mobile',
				  'size'	=> 15,
				  'default'	=> $element['phonebooknumber']['number']['default'],
				  'value'	=> $this->get_var('phonebooknumber','mobile','number'))),

		$form->text(array('desc'	=> $this->bbf('fm_phonebook_email'),
				  'name'	=> 'phonebook[email]',
				  'labelid'	=> 'phonebook-email',
				  'size'	=> 15,
				  'default'	=> $element['phonebook']['email']['default'],
				  'value'	=> $info['phonebook']['email'])),

		$form->text(array('desc'	=> $this->bbf('fm_phonebook_url'),
				  'name'	=> 'phonebook[url]',
				  'labelid'	=> 'phonebook-url',
				  'size'	=> 15,
				  'default'	=> $element['phonebook']['url']['default'],
				  'value'	=> $info['phonebook']['url']));
?>
	<div class="fm-paragraph fm-description">
		<p>
			<label id="lb-phonebook-description" for="it-phonebook-description"><?=$this->bbf('fm_phonebook_description');?></label>
		</p>
		<?=$form->textarea(array('paragraph'	=> false,
					 'label'	=> false,
					 'name'		=> 'phonebook[description]',
					 'id'		=> 'it-phonebook-description',
					 'cols'		=> 60,
					 'rows'		=> 5,
					 'default'	=> $element['phonebook']['description']['default']),
				   $info['phonebook']['description']);?>
	</div>
</div>

<div id="sb-part-office" class="b-nodisplay">
<?php
	$this->file_include('bloc/service/ipbx/asterisk/pbx_services/phonebook/type',
			    array('type'	=> 'office'));
?>
</div>

<div id="sb-part-home" class="b-nodisplay">
<?php
	$this->file_include('bloc/service/ipbx/asterisk/pbx_services/phonebook/type',
			    array('type'	=> 'home'));
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	$this->file_include('bloc/service/ipbx/asterisk/pbx_services/phonebook/type',
			    array('type'	=> 'other'));
?>
</div>
