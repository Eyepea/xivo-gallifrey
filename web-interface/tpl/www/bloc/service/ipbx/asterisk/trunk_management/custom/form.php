<?php
	$form = &$this->get_module('form');

	$info = $this->get_var('info');
	$element = $this->get_var('element');
?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_name'),'name' => 'protocol[name]','labelid' => 'protocol-name','size' => 15,'default' => $element['protocol']['interface'],'value' => $info['protocol']['name']));?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_interface'),'name' => 'protocol[interface]','labelid' => 'protocol-interface','size' => 15,'default' => $element['protocol']['interface'],'value' => $info['protocol']['interface']));?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_intfsuffix'),'name' => 'protocol[intfsuffix]','labelid' => 'protocol-intfsuffix','size' => 15,'default' => $element['protocol']['intfsuffix'],'value' => $info['protocol']['intfsuffix']));?>
