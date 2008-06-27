<div id="fd-ipbxapplication-absolutetimeout" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_absolutetimeout = $this->get_var('apparg_absolutetimeout');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_absolutetimeout-timeout'),
		          'name'	=> 'ipbxapplication[absolutetimeout][timeout]',
		          'labelid'	=> 'ipbxapplication-absolutetimeout-timeout',
		          'size'	=> 10,
		          'default'	=> $apparg_absolutetimeout['timeout']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-absolutetimeout',
			    'id'	=> 'it-add-ipbxapplication-absolutetimeout',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_absolutetimeout();"');

?>
</div>
