<div id="fd-ipbxapplication-noop" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_noop = $this->get_var('apparg_noop');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_noop-data'),
		          'name'	=> 'ipbxapplication[noop][data]',
		          'labelid'	=> 'ipbxapplication-noop-data',
		          'size'	=> 15,
		          'default'	=> $apparg_noop['data']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-noop',
			    'id'	=> 'it-add-ipbxapplication-noop',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_noop();"');

?>
</div>
