<div id="fd-ipbxapplication-responsetimeout" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_responsetimeout = $this->get_var('apparg_responsetimeout');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_responsetimeout-timeout'),
		          'name'	=> 'ipbxapplication[responsetimeout][timeout]',
		          'labelid'	=> 'ipbxapplication-responsetimeout-timeout',
		          'size'	=> 10,
		          'default'	=> $apparg_responsetimeout['timeout']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-responsetimeout',
			    'id'	=> 'it-add-ipbxapplication-responsetimeout',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_responsetimeout();"');

?>
</div>
