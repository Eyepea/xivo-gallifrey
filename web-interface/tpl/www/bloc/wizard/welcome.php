<?php

$form = &$this->get_module('form');

echo	$form->hidden(array('name'	=> 'fm_send',
							'value'	=> 1)),

		$form->hidden(array('name'	=> 'step',
							'value'	=> $this->get_var('step')));

include(XIVO_PATH_CONF . '/welcome_text');
?>
