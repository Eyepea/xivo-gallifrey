<?php
	$form = &$this->get_module('form');

	$info = $this->get_var('info');
	$element = $this->get_var('element');
?>

<?=$form->text(array('desc' => $this->bbf('fm_name'),'name' => 'name','size' => 15,'default' => $element['name']['default'],'value' => $info['name']));?>

<?=$form->text(array('desc' => $this->bbf('fm_host'),'name' => 'host','size' => 15,'default' => $element['host']['default'],'value' => $info['host']));?>

<?=$form->text(array('desc' => $this->bbf('fm_port'),'name' => 'port','default' => $element['port']['default'],'value' => $info['port']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_ssl'),'name' => 'ssl','default' => $element['ssl']['default'],'checked' => $info['ssl']));?>

<?=$form->select(array('desc' => $this->bbf('fm_protocolversion'),'name' => 'protocolversion','labelid' => 'protocolversion','bbf' => array('paramkey','fm_protocolversion-opt'),'key' => false,'default' => $element['protocolversion']['default'],'value' => $info['protocolversion']),$element['protocolversion']['value']);?>

<div class="fm-field fm-description"><p><label id="lb-description" for="it-description"><?=$this->bbf('fm_description');?></label></p>
<?=$form->textarea(array('field' => false,'label' => false,'name' => 'description','id' => 'it-description','cols' => 60,'rows' => 5,'default' => $element['description']['default']),$info['description']);?>
</div>

