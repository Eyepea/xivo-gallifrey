<?php
	$form = &$this->get_module('form');

	$info = $this->get_var('info');
	$element = $this->get_var('element');
	$context_list = $this->get_var('context_list');
?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_name'),'name' => 'protocol[name]','labelid' => 'protocol-name','size' => 15,'default' => $element['protocol']['interface'],'value' => $info['protocol']['name']));?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_interface'),'name' => 'protocol[interface]','labelid' => 'protocol-interface','size' => 15,'default' => $element['protocol']['interface'],'value' => $info['protocol']['interface']));?>

<?=$form->text(array('desc' => $this->bbf('fm_protocol_intfsuffix'),'name' => 'protocol[intfsuffix]','labelid' => 'protocol-intfsuffix','size' => 15,'default' => $element['protocol']['intfsuffix'],'value' => $info['protocol']['intfsuffix']));?>

<?php

if($context_list !== false):
	echo $form->select(array('desc' => $this->bbf('fm_protocol_context'),'name' => 'protocol[context]','labelid' => 'protocol-context','key' => 'identity','altkey' => 'name','empty' => true,'default' => $element['protocol']['context']['default'],'value' => $info['protocol']['context']),$context_list);
endif;

?>
