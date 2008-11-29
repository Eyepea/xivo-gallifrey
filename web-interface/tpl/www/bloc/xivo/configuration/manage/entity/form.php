<?php

$form = &$this->get_module('form');

$info = $this->get_var('info');
$element = $this->get_var('element');

echo	$form->text(array('desc'	=> $this->bbf('fm_name'),
			  'name'	=> 'name',
			  'labelid'	=> 'name',
			  'size'	=> 15,
			  'default'	=> $element['name']['default'],
			  'value'	=> $info['name'])),

	$form->text(array('desc'	=> $this->bbf('fm_displayname'),
			  'name'	=> 'displayname',
			  'labelid'	=> 'displayname',
			  'size'	=> 15,
			  'default'	=> $element['displayname']['default'],
			  'value'	=> $info['displayname'])),

	$form->text(array('desc'	=> $this->bbf('fm_phonenumber'),
			  'name'	=> 'phonenumber',
			  'labelid'	=> 'phonenumber',
			  'size'	=> 15,
			  'default'	=> $element['phonenumber']['default'],
			  'value'	=> $info['phonenumber'])),

	$form->text(array('desc'	=> $this->bbf('fm_faxnumber'),
			  'name'	=> 'faxnumber',
			  'labelid'	=> 'faxnumber',
			  'size'	=> 15,
			  'default'	=> $element['faxnumber']['default'],
			  'value'	=> $info['faxnumber'])),

	$form->text(array('desc'	=> $this->bbf('fm_email'),
			  'name'	=> 'email',
			  'labelid'	=> 'email',
			  'size'	=> 15,
			  'default'	=> $element['email']['default'],
			  'value'	=> $info['email'])),

	$form->text(array('desc'	=> $this->bbf('fm_url'),
			  'name'	=> 'url',
			  'labelid'	=> 'url',
			  'size'	=> 15,
			  'default'	=> $element['url']['default'],
			  'value'	=> $info['url'])),

	$form->text(array('desc'	=> $this->bbf('fm_address1'),
			  'name'	=> 'address1',
			  'labelid'	=> 'address1',
			  'size'	=> 15,
			  'default'	=> $element['address1']['default'],
			  'value'	=> $info['address1'])),

	$form->text(array('desc'	=> $this->bbf('fm_address2'),
			  'name'	=> 'address2',
			  'labelid'	=> 'address2',
			  'size'	=> 15,
			  'default'	=> $element['address2']['default'],
			  'value'	=> $info['address2'])),

	$form->text(array('desc'	=> $this->bbf('fm_city'),
			  'name'	=> 'city',
			  'labelid'	=> 'city',
			  'size'	=> 15,
			  'default'	=> $element['city']['default'],
			  'value'	=> $info['city'])),

	$form->text(array('desc'	=> $this->bbf('fm_state'),
			  'name'	=> 'state',
			  'labelid'	=> 'state',
			  'size'	=> 15,
			  'default'	=> $element['state']['default'],
			  'value'	=> $info['state'])),

	$form->text(array('desc'	=> $this->bbf('fm_zipcode'),
			  'name'	=> 'zipcode',
			  'labelid'	=> 'zipcode',
			  'size'	=> 15,
			  'default'	=> $element['zipcode']['default'],
			  'value'	=> $info['zipcode'])),

	$form->select(array('desc'	=> $this->bbf('fm_country'),
			    'name'	=> 'country',
			    'labelid'	=> 'country',
			    'empty'	=> true,
			    'size'	=> 15,
			    'default'	=> $element['country']['default'],
			    'value'	=> $info['country']),
		      $this->get_var('territory'));
?>
<div class="fm-field fm-description">
	<p>
		<label id="lb-description" for="it-description"><?=$this->bbf('fm_description');?></label>
	</p>
	<?=$form->textarea(array('field'	=> false,
				 'label'	=> false,
				 'name'		=> 'description',
				 'id'		=> 'it-description',
				 'cols'		=> 60,
				 'rows'		=> 5,
				 'default'	=> $element['description']['default']),
			   $info['description']);?>
</div>
