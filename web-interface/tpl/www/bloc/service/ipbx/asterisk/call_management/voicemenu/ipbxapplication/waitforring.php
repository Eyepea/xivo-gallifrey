<div id="fd-ipbxapplication-waitforring" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_waitforring = $this->get_var('apparg_waitforring');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_waitforring-timeout'),
		          'name'	=> 'ipbxapplication[waitforring][timeout]',
		          'labelid'	=> 'ipbxapplication-waitforring-timeout',
		          'size'	=> 10,
		          'default'	=> $apparg_waitforring['timeout']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-waitforring',
			    'id'	=> 'it-add-ipbxapplication-waitforring',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_waitforring();"');

?>
</div>
