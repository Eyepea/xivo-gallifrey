<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->vars('info');
	$element = $this->vars('element');
?>

<div id="sb-part-first">

<?=$form->select(array('desc' => $this->bbf('fm_phonebook_title'),'name' => 'phonebook[title]','labelid' => 'phonebook-title','key' => false,'bbf' => array('concatkey','fm_phonebook_title-opt-'),'value' => $info['phonebook']['title'],'default' => $element['phonebook']['title']['default']),$element['phonebook']['title']['value']);?>

<?=$form->text(array('desc' => $this->bbf('fm_phonebook_firstname'),'name' => 'phonebook[firstname]','labelid' => 'phonebook-firstname','size' => 15,'default' => $element['phonebook']['firstname']['default'],'value' => $info['phonebook']['firstname']));?>

<?=$form->text(array('desc' => $this->bbf('fm_phonebook_lastname'),'name' => 'phonebook[lastname]','labelid' => 'phonebook-lastname','size' => 15,'default' => $element['phonebook']['lastname']['default'],'value' => $info['phonebook']['lastname']));?>

<?=$form->text(array('desc' => $this->bbf('fm_phonebook_displayname'),'name' => 'phonebook[displayname]','labelid' => 'phonebook-displayname','size' => 15,'default' => $element['phonebook']['displayname']['default'],'value' => $info['phonebook']['displayname']));?>

<?=$form->text(array('desc' => $this->bbf('fm_phonebook_society'),'name' => 'phonebook[society]','labelid' => 'phonebook-society','size' => 15,'default' => $element['phonebook']['society']['default'],'value' => $info['phonebook']['society']));?>

<?=$form->text(array('desc' => $this->bbf('fm_phonebooknumber_mobile'),'name' => 'phonebooknumber[mobile]','labelid' => 'phonebooknumber-mobile','size' => 15,'default' => $element['phonebooknumber']['number']['default'],'value' => $this->varra('phonebooknumber',array('mobile','number'))));?>

<?=$form->text(array('desc' => $this->bbf('fm_phonebook_email'),'name' => 'phonebook[email]','labelid' => 'phonebook-email','size' => 15,'default' => $element['phonebook']['email']['default'],'value' => $info['phonebook']['email']));?>

<?=$form->text(array('desc' => $this->bbf('fm_phonebook_url'),'name' => 'phonebook[url]','labelid' => 'phonebook-url','size' => 15,'default' => $element['phonebook']['url']['default'],'value' => $info['phonebook']['url']));?>

<div class="fm-field fm-description"><p><label id="lb-phonebook-description" for="it-phonebook-description"><?=$this->bbf('fm_phonebook_description');?></label></p>
<?=$form->textarea(array('field' => false,'label' => false,'name' => 'phonebook[description]','id' => 'it-phonebook-description','cols' => 60,'rows' => 5,'default' => $element['phonebook']['description']['default']),$info['phonebook']['description']);?>
</div>

</div>

<div id="sb-part-office" class="b-nodisplay">

<?=$this->file_include('bloc/service/ipbx/asterisk/pbx_services/phonebook/type',array('type' => 'office'));?>

</div>

<div id="sb-part-home" class="b-nodisplay">

<?=$this->file_include('bloc/service/ipbx/asterisk/pbx_services/phonebook/type',array('type' => 'home'));?>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?=$this->file_include('bloc/service/ipbx/asterisk/pbx_services/phonebook/type',array('type' => 'other'));?>

</div>
