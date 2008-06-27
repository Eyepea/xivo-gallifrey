<div id="fd-ipbxapplication-wait" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_wait = $this->get_var('apparg_wait');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_wait-seconds'),
		          'name'	=> 'ipbxapplication[wait][seconds]',
		          'labelid'	=> 'ipbxapplication-wait-seconds',
		          'size'	=> 10,
		          'default'	=> $apparg_wait['seconds']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-wait',
			    'id'	=> 'it-add-ipbxapplication-wait',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_wait();"');

?>
</div>
