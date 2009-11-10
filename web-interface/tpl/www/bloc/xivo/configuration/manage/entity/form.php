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

$info = $this->get_var('info');
$element = $this->get_var('element');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

echo	$form->text(array('desc'	=> $this->bbf('fm_name'),
			  'name'	=> 'name',
			  'labelid'	=> 'name',
			  'size'	=> 15,
			  'default'	=> $element['entity']['name']['default'],
			  'value'	=> $info['name'])),

	$form->text(array('desc'	=> $this->bbf('fm_displayname'),
			  'name'	=> 'displayname',
			  'labelid'	=> 'displayname',
			  'size'	=> 15,
			  'default'	=> $element['entity']['displayname']['default'],
			  'value'	=> $info['displayname'])),

	$form->text(array('desc'	=> $this->bbf('fm_phonenumber'),
			  'name'	=> 'phonenumber',
			  'labelid'	=> 'phonenumber',
			  'size'	=> 15,
			  'default'	=> $element['entity']['phonenumber']['default'],
			  'value'	=> $info['phonenumber'])),

	$form->text(array('desc'	=> $this->bbf('fm_faxnumber'),
			  'name'	=> 'faxnumber',
			  'labelid'	=> 'faxnumber',
			  'size'	=> 15,
			  'default'	=> $element['entity']['faxnumber']['default'],
			  'value'	=> $info['faxnumber'])),

	$form->text(array('desc'	=> $this->bbf('fm_email'),
			  'name'	=> 'email',
			  'labelid'	=> 'email',
			  'size'	=> 15,
			  'default'	=> $element['entity']['email']['default'],
			  'value'	=> $info['email'])),

	$form->text(array('desc'	=> $this->bbf('fm_url'),
			  'name'	=> 'url',
			  'labelid'	=> 'url',
			  'size'	=> 15,
			  'default'	=> $element['entity']['url']['default'],
			  'value'	=> $info['url'])),

	$form->text(array('desc'	=> $this->bbf('fm_address1'),
			  'name'	=> 'address1',
			  'labelid'	=> 'address1',
			  'size'	=> 15,
			  'default'	=> $element['entity']['address1']['default'],
			  'value'	=> $info['address1'])),

	$form->text(array('desc'	=> $this->bbf('fm_address2'),
			  'name'	=> 'address2',
			  'labelid'	=> 'address2',
			  'size'	=> 15,
			  'default'	=> $element['entity']['address2']['default'],
			  'value'	=> $info['address2'])),

	$form->text(array('desc'	=> $this->bbf('fm_city'),
			  'name'	=> 'city',
			  'labelid'	=> 'city',
			  'size'	=> 15,
			  'default'	=> $element['entity']['city']['default'],
			  'value'	=> $info['city'])),

	$form->text(array('desc'	=> $this->bbf('fm_state'),
			  'name'	=> 'state',
			  'labelid'	=> 'state',
			  'size'	=> 15,
			  'default'	=> $element['entity']['state']['default'],
			  'value'	=> $info['state'])),

	$form->text(array('desc'	=> $this->bbf('fm_zipcode'),
			  'name'	=> 'zipcode',
			  'labelid'	=> 'zipcode',
			  'size'	=> 15,
			  'default'	=> $element['entity']['zipcode']['default'],
			  'value'	=> $info['zipcode'])),

	$form->select(array('desc'	=> $this->bbf('fm_country'),
			    'name'	=> 'country',
			    'labelid'	=> 'country',
			    'empty'	=> true,
			    'default'	=> $element['entity']['country']['default'],
			    'selected'	=> $info['country']),
		      $this->get_var('territory'));
?>
<div class="fm-paragraph fm-description">
	<p>
		<label id="lb-description" for="it-description"><?=$this->bbf('fm_description');?></label>
	</p>
	<?=$form->textarea(array('paragraph'	=> false,
				 'label'	=> false,
				 'name'		=> 'description',
				 'id'		=> 'it-description',
				 'cols'		=> 60,
				 'rows'		=> 5,
				 'default'	=> $element['entity']['description']['default']),
			   $info['description']);?>
</div>
