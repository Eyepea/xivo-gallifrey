<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$element = $this->get_var('element');
	$list = $this->get_var('list');
	$type = $this->get_var('type');
	$territory = $this->get_var('territory');
?>

<?=$form->text(array('desc' => $this->bbf('fm_phonebooknumber_'.$type),'name' => 'phonebooknumber['.$type.']','labelid' => 'phonebooknumber-'.$type,'size' => 15,'default' => $element['phonebooknumber']['number']['default'],'value' => $this->get_varra('phonebooknumber',array($type,'number'))));?>

<?php
	if($type === 'office'):
		echo $form->text(array('desc' => $this->bbf('fm_phonebooknumber_fax'),'name' => 'phonebooknumber[fax]','labelid' => 'phonebooknumber-fax','size' => 15,'default' => $element['phonebooknumber']['number']['default'],'value' => $this->get_varra('phonebooknumber',array('fax','number'))));
	endif;
?>

<?=$form->text(array('desc' => $this->bbf('fm_phonebookaddress_address1'),'name' => 'phonebookaddress['.$type.'][address1]','labelid' => 'phonebookaddress-'.$type.'-address1','size' => 15,'default' => $element['phonebookaddress']['address1']['default'],'value' => $this->get_varra('phonebookaddress',array($type,'address1'))));?>

<?=$form->text(array('desc' => $this->bbf('fm_phonebookaddress_address2'),'name' => 'phonebookaddress['.$type.'][address2]','labelid' => 'phonebookaddress-'.$type.'-address2','size' => 15,'default' => $element['phonebookaddress']['address2']['default'],'value' => $this->get_varra('phonebookaddress',array($type,'address2'))));?>

<?=$form->text(array('desc' => $this->bbf('fm_phonebookaddress_city'),'name' => 'phonebookaddress['.$type.'][city]','labelid' => 'phonebookaddress-'.$type.'-city','size' => 15,'default' => $element['phonebookaddress']['city']['default'],'value' => $this->get_varra('phonebookaddress',array($type,'city'))));?>

<?=$form->text(array('desc' => $this->bbf('fm_phonebookaddress_state'),'name' => 'phonebookaddress['.$type.'][state]','labelid' => 'phonebookaddress-'.$type.'-state','size' => 15,'default' => $element['phonebookaddress']['state']['default'],'value' => $this->get_varra('phonebookaddress',array($type,'state'))));?>

<?=$form->text(array('desc' => $this->bbf('fm_phonebookaddress_zipcode'),'name' => 'phonebookaddress['.$type.'][zipcode]','labelid' => 'phonebookaddress-'.$type.'-zipcode','size' => 15,'default' => $element['phonebookaddress']['zipcode']['default'],'value' => $this->get_varra('phonebookaddress',array($type,'zipcode'))));?>

<?=$form->select(array('desc' => $this->bbf('fm_phonebookaddress_country'),'name' => 'phonebookaddress['.$type.'][country]','labelid' => 'phonebookaddress-'.$type.'-country','empty' => true,'size' => 15,'default' => $element['phonebookaddress']['country']['default'],'value' => $this->get_varra('phonebookaddress',array($type,'country'))),$territory);?>

