<h1>XIVO Configuration is now complete</h1>

<?php

$url = &$this->get_module('url');
$form = &$this->get_module('form');

echo	$url->href_html('previous',
						'index',
						array('step' => 'ipbximportuser'),
						null,
						$this->bbf('page_previous')),

		$form->hidden(array('name'	=> 'fm_send',
							'value'	=> 1)),

		$form->hidden(array('name'	=> 'step',
							'value'	=> $this->get_var('step'))),

		$form->submit(array('name'	=> 'next',
							'value'	=> $this->bbf('validate')));
?>
