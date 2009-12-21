<?php
$form = &$this->get_module('form');
$dhtml = &$this->get_module('dhtml');

echo
	$form->hidden(array('name' => 'fm_send',
						'value' => 1)),

	$form->hidden(array('name' => 'step',
						'value' => $this->get_var('step'))),

	$this->file_include('bloc/service/ipbx/asterisk/system_management/context/submenu');
	$this->file_include('bloc/service/ipbx/asterisk/system_management/context/form');
?>
