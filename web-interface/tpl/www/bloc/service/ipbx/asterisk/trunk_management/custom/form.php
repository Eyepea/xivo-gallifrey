<?php
	$form = &$this->get_module('form');
	$url = $this->get_module('url');

	$info = $this->get_var('info');
	$element = $this->get_var('element');
?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_name'),'name' => 'trunk[name]','labelid' => 'trunk-name','size' => 15,'default' => $element['trunk']['interface'],'value' => $info['trunk']['name']));?>

<?=$form->text(array('desc' => $this->bbf('fm_trunk_interface'),'name' => 'trunk[interface]','labelid' => 'trunk-interface','size' => 15,'default' => $element['trunk']['interface'],'value' => $info['trunk']['interface']));?>

