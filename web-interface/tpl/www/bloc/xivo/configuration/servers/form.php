<?php
	$form = &$this->get_module('form');

	$info = $this->vars('info');
	$element = $this->vars('element');
?>

<?=$form->text(array('desc' => $this->bbf('fm_name'),'name' => 'name','size' => 15,'default' => $element['name']['default'],'value' => $info['name']));?>

<?=$form->text(array('desc' => $this->bbf('fm_host'),'name' => 'host','size' => 15,'default' => $element['host']['default'],'value' => $info['host']));?>

<?=$form->text(array('desc' => $this->bbf('fm_port'),'name' => 'port','size' => 15,'default' => $element['port']['default'],'value' => $info['port']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_ssl'),'name' => 'ssl','default' => $element['ssl']['default'],'checked' => $info['ssl']));?>


<div class="fm-field fm-description"><p><label id="lb-description" for="it-description"><?=$this->bbf('fm_description');?></label></p>
<?=$form->textarea(array('field' => false,'label' => false,'name' => 'description','id' => 'it-description','cols' => 60,'rows' => 5,'default' => $element['description']['default']),$info['description']);?>
</div>

