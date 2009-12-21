<?php
$form = &$this->get_module('form');
$gpltext = file_get_contents(XIVO_PATH_CONF . '/gpl-3.0.txt');

echo 
	$form->hidden(array('name' => 'fm_send',
						'value' => 1)),

	$form->hidden(array('name' => 'step',
						'value' => $this->get_var('step'))),
	
	$form->textarea(array('label' => false,
							'notag' => false,
							'name' => 'gplv3',
							'id' => 'gplv3',
							'cols' => 70,
							'rows' => 25),
						$gpltext),
						
	$form->checkbox(array('desc' => $this->bbf('wz-license-agree'),
							'name' => 'wz-license-agree',
							'labelid' => 'wz-license-agree',
							'checked' => false));
?>
